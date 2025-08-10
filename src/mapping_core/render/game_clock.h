#ifndef GAMECLOCK_H
#define GAMECLOCK_H
#include <chrono>
#include <cstdint>

class GameClock
{
    enum class TickSpeed : std::chrono::milliseconds::rep {
        Slowest = 167,
        Slower = 111,
        Slow = 83,
        Normal = 67,
        Fast = 56,
        Faster = 48,
        Fastest = 42
    };

    std::chrono::milliseconds msPerFrame = std::chrono::milliseconds(std::chrono::milliseconds::rep(TickSpeed::Fastest));
    std::chrono::system_clock::time_point epoch = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point nextTick = epoch;
    std::uint64_t tickCount = 0;

public:
    inline bool tick(std::chrono::system_clock::time_point now = std::chrono::system_clock::now())
    {
        if ( now < nextTick ) // Not a tick
            return false;

        if ( nextTick + msPerFrame > now ) // This tick was started in real time
            nextTick += msPerFrame; // Keep ticks steady
        else // The process is running behind, there may have been lag or the process/machine may have resumed after suspension
            nextTick = now + msPerFrame; // Push out the next tick by msPerFrame from now

        ++tickCount;

        return true;
    }

    inline std::uint64_t currentTick() const { return tickCount; }
};

#endif