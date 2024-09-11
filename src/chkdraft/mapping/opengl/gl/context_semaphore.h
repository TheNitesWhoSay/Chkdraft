#pragma once
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>

namespace gl
{
    struct RenderContext
    {
        virtual ~RenderContext() {}
        virtual void claim() = 0;
        virtual void release() = 0;
    };

    class ContextSemaphore // Enables threads to take ownership of a given OpenGL context
    {
        std::mutex mutex {};
        bool contextAvailable = true;
        std::condition_variable contextAvailableCv {};
        std::unique_lock<std::mutex> primaryOpenGlThreadLock;
        std::thread::id primaryOpenGlThreadId;
        RenderContext* renderContext;

        struct Locker
        {
            std::unique_lock<std::mutex> lock;
            ContextSemaphore* context = nullptr;

            Locker() {}

            Locker(ContextSemaphore & context) : lock(context.mutex), context(&context)
            {
                context.contextAvailableCv.wait(lock, [&]{ return context.contextAvailable; });
                context.contextAvailable = false;
                #pragma warning(suppress: 26115)
                context.renderContext->claim();
            }

            ~Locker()
            {
                if ( context != nullptr )
                {
                    context->renderContext->release();
                    context->contextAvailable = true;
                    lock.unlock();
                    context->contextAvailableCv.notify_one();
                }
            }
        };

    public:
        // Constructs a ContextSemaphore which can claim and release the OpenGL context with the given claim and release functions
        // The constructing thread is made the primary OpenGL thread and the context is initially unowned
        ContextSemaphore(RenderContext* renderContext) :
            contextAvailable(false),
            primaryOpenGlThreadLock(mutex),
            primaryOpenGlThreadId(std::this_thread::get_id()),
            renderContext(renderContext)
        {

        }

        ~ContextSemaphore()
        {
            if ( primaryOpenGlThreadLock.owns_lock() )
                primaryOpenGlThreadLock.unlock();

            primaryOpenGlThreadLock.release();
        }

        void claimContext() // Only the thread which created the ContextSemaphore may explicitly claim
        {
            if ( std::this_thread::get_id() == primaryOpenGlThreadId )
            {
                if ( !primaryOpenGlThreadLock.owns_lock() )
                    primaryOpenGlThreadLock.lock();

                contextAvailable = false;
                renderContext->claim();
            }
            else
                throw std::logic_error("Only the thread which created ContextSemaphore may explicitly claim");
        }

        void releaseContext() // Only the thread which created the ContextSemaphore may explicitly release
        {
            if ( std::this_thread::get_id() == primaryOpenGlThreadId )
            {
                if ( !primaryOpenGlThreadLock.owns_lock() )
                    return;

                renderContext->release();
                contextAvailable = true;
                #pragma warning(suppress: 26110)
                primaryOpenGlThreadLock.unlock();
                contextAvailableCv.notify_one();
            }
            else
                throw std::logic_error("Only the thread which created ContextSemaphore may explicitly release");
        }

        // Worker threads should use this to aquire an OpenGL context; when the returned "unique lock" goes out of scope the context is released
        // auto contextLock = gl::ContextSemaphore::aquire_unique_context_lock(contextSemaphore);
        static inline auto aquire_unique_context_lock(gl::ContextSemaphore* contextSemaphore)
        {
            if ( contextSemaphore != nullptr )
                return gl::ContextSemaphore::Locker { *contextSemaphore };
            else
                return gl::ContextSemaphore::Locker {};
        }
    };
}
