#ifndef FPS_H
#define FPS_H

#include <array>
#include <cstddef>
#include <chrono>
#include <sstream>
#include <string>

namespace gl
{
    struct Fps
    {
        static constexpr size_t framesPerSample = 500;
        static constexpr size_t msPerDisplayUpdate = 100;
        std::chrono::system_clock::time_point oldestFrameTs;
        std::chrono::system_clock::time_point lastUpdateTs;
        std::array<std::chrono::system_clock::time_point, framesPerSample> frameTs;
        size_t currFrameIndex = 0;
        bool cycled = false;

        std::string displayNumber = "";

        Fps() : oldestFrameTs(std::chrono::system_clock::now()), lastUpdateTs()
        {
            frameTs.fill(oldestFrameTs);
        }
            
        inline void update(std::chrono::system_clock::time_point now)
        {
            if ( currFrameIndex == frameTs.size() )
            {
                currFrameIndex = 0;
                if ( !cycled )
                    cycled = true;
            }
                
            oldestFrameTs = frameTs[currFrameIndex];
            auto msSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now-lastUpdateTs).count();
            if ( msSinceLastUpdate >= msPerDisplayUpdate )
            {
                lastUpdateTs = now;
                double msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now-oldestFrameTs).count();
                std::stringstream ssFps {};
                ssFps.precision(2);
                ssFps << std::fixed << ((cycled ? framesPerSample : currFrameIndex)*1000./double(msElapsed));
                displayNumber = ssFps.str();
            }

            frameTs[currFrameIndex] = now;
            ++currFrameIndex;
        }
    };
}

#endif