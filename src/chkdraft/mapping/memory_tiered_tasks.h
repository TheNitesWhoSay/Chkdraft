#pragma once
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <thread>
#include <vector>

struct MemoryTier
{
    struct Range
    {
        size_t min;
        size_t max;
    };

    Range range;
    size_t totalBuffers = 1;
};

template <typename TaskInfo>
class MemoryTieredTasks
{
    // Parallel arrays sorted smallest range to largest range
    std::vector<MemoryTier::Range> memoryTiers {};
    std::vector<std::vector<ByteBuffer>> tierMemory {};
    std::vector<std::condition_variable> tierMemoryCv;
    std::vector<std::vector<TaskInfo>> tierTasks {};
    
    size_t totalThreads = 0;
    size_t remainingTasks = 0;
    std::condition_variable taskCv {};

    std::atomic<bool> doneAddingTasks = false;

    std::mutex mutex {};

public:
    MemoryTieredTasks(size_t totalThreads, std::vector<MemoryTier> memTiers) // Should only be one buffer in largest memory tier
        : totalThreads(totalThreads), tierMemoryCv(memTiers.size())
    {
        std::sort(memTiers.begin(), memTiers.end(), [](auto l, auto r) { return l.range.max < r.range.max; });

        tierTasks.assign(memTiers.size(), {});
        memoryTiers.reserve(memTiers.size());
        for ( size_t i=0; i<memTiers.size(); ++ i )
        {
            auto & memTier = memTiers[i];
            memoryTiers.push_back(memTier.range);
            tierMemory.emplace_back();
            for ( size_t j=0; j<memTier.totalBuffers; ++j )
            {
                tierMemory[i].emplace_back();
                tierMemory[i].back().expand(memTier.range.max);
            }
        }
    }

    void add_task(TaskInfo task)
    {
        std::unique_lock lock {mutex};
        for ( size_t i=0; i<memoryTiers.size(); ++i )
        {
            if ( task.requiredMemory < memoryTiers[i].max )
            {
                tierTasks[i].push_back(task);
                ++remainingTasks;
                lock.unlock();
                taskCv.notify_one();
                return;
            }
        }
        tierMemoryCv.back().wait(lock, [&]{ return !tierMemory.back().empty(); });
        tierMemory.back()[0].expand(task.requiredMemory);
        memoryTiers.back().max = task.requiredMemory;
        tierTasks.back().push_back(task);
        ++remainingTasks;
        lock.unlock();
        taskCv.notify_one();
    }

    void get_task()
    {
        std::unique_lock lock {mutex};
        taskCv.wait(lock, [&]{ return remainingTasks > 0 || doneAddingTasks; });
        if ( remainingTasks == 0 && doneAddingTasks )
            return;

        for ( int i=int(tierTasks.size()-1); i>=0; --i )
        {
            if ( !tierTasks[i].empty() )
            {
                // Pop a task
                auto task = tierTasks[i].back();
                --remainingTasks;
                tierTasks[i].pop_back();

                // Pop memory for the task
                tierMemoryCv[i].wait(lock, [&]{ return !tierMemory[i].empty(); });
                auto memory = std::move(tierMemory[i].back());
                tierMemory[i].pop_back();
                lock.unlock();

                task.perform(memory);

                // Return the memory
                lock.lock();
                tierMemory[i].push_back(std::move(memory));
                lock.unlock();
                tierMemoryCv[i].notify_one();
                return;
            }
        }
    }

    template <typename TaskProvider>
    void run_tasks(TaskProvider && taskProvider)
    {
        std::vector<std::jthread> workers {};
        for ( size_t i=0; i<totalThreads; ++i )
        {
            workers.emplace_back([&](){
                while ( !(doneAddingTasks && remainingTasks == 0) )
                    get_task();
            });
        }

        taskProvider();
        doneAddingTasks = true;
        taskCv.notify_all();

        for ( auto & worker : workers )
            worker.join();
    }
};