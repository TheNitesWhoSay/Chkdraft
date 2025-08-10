#ifndef COLORCYCLER_H
#define COLORCYCLER_H
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>

class ColorCycler
{
    class ColorCycleClock
    {
        std::chrono::system_clock::duration nsPerFrame = std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::nanoseconds(std::chrono::nanoseconds::rep(15615000))); // avg 15.615ms
        std::chrono::system_clock::time_point nextTick = std::chrono::system_clock::now();

    public:
        inline bool tick(std::chrono::system_clock::time_point now = std::chrono::system_clock::now())
        {
            if ( now < nextTick ) // Not a tick
                return false;

            if ( nextTick + nsPerFrame > now ) // This tick was started in real time
                nextTick += std::chrono::system_clock::duration(nsPerFrame); // Keep ticks steady
            else // The process is running behind, there may have been lag or the process/machine may have resumed after suspension
                nextTick = now + nsPerFrame; // Push out the next tick by msPerFrame from now

            return true;
        }
    };

    /**
        A tileset has a set of between zero and eight rotators, after every tick (one change in GetTickCount: ~15.615ms), each rotator's ticksRemaining is decremented

        If a rotator's ticksRemaining reaches zero, palette colors from min to max are each rotated one index to the right "palette[index+1] = palette[index]",
        the rightmost entry "palette[paletteIndexMax]" wraps around to become the leftmost entry "palette[paletteIndexMin]"
        following this, ticksRemaining gets set to ticksBetweenRotations, and anything using the palette should be redrawn
    */
    struct Rotator
    {
        enum class Enabled {
            No,
            Yes
        };

        Enabled enabled;
        std::uint8_t ticksBetweenRotations;
        std::uint8_t ticksRemaining; // When this reaches 0, palette colors from min to max are each rotated one index to the right, with the rightmost 
        std::uint16_t paletteIndexMin;
        std::uint16_t paletteIndexMax;
    };

    static constexpr size_t TotalRotatorSets = 4;
    static constexpr size_t MaxRotatersPerSet = 8;

    ColorCycleClock colorCycleClock {};

    static constexpr size_t TilesetRotationSet[8] {
        0, // badlands uses set 0
        1, // platform uses set 1
        1, // installation uses set 1
        2, // ashworld uses set 2
        0, // jungle uses set 0
        3, // desert uses set 3
        3, // iceworld uses set 3
        3  // twilight uses set 3
    }; // Index of the rotater to use for a given tileset, all values must be less than TotalRotaterSets
    Rotator NoRotators[MaxRotatersPerSet] {}; // An empty rotator set
    Rotator RotatorSets[TotalRotatorSets][MaxRotatersPerSet] {
        { // Rotator set 0: Three rotators act on badlands and jungle
            { Rotator::Enabled::Yes, 8, 0, 1, 6 }, // Every eight ticks rotate palette colors from indexes 1 to 6 (inclusive) rightwards
            { Rotator::Enabled::Yes, 8, 0, 7, 13 }, // Every eight ticks rotate palette colors from indexes 7 to 13 (inclusive) rightwards
            { Rotator::Enabled::Yes, 8, 0, 248, 254 } // Every eight ticks rotate palette colors from indexes 248 to 254 (inclusive) rightwards
        },
        {}, // Rotator set 1: No rotators act on platform or installation
        { // Rotator set 2: Three rotators act on ashworld
            { Rotator::Enabled::Yes, 8, 0, 1, 4 }, // Every eight ticks rotate palette colors from indexes 1 to 4 (inclusive) rightwards
            { Rotator::Enabled::Yes, 8, 0, 5, 8 }, // Every eight ticks rotate palette colors from indexes 5 to 8 (inclusive) rightwards
            { Rotator::Enabled::Yes, 8, 0, 9, 13 } // Every eight ticks rotate palette colors from indexes 9 to 13 (inclusive) rightwards
        },
        { // Rotator set 3: Two rotators act on desert, iceworld, and twilight
            { Rotator::Enabled::Yes, 8, 0, 1, 13 }, // Every eight ticks rotate palette colors from indexes 1 to 13 (inclusive) rightwards
            { Rotator::Enabled::Yes, 8, 0, 248, 254 } // Every eight ticks rotate palette colors from indexes 248 to 254 (inclusive) rightwards
        }
    }; // All rotator sets

    std::chrono::high_resolution_clock::time_point nextTick = std::chrono::high_resolution_clock::now();

public:
    inline bool cycleColors(size_t tileset, auto & palette) // Returns true if the map should be redrawn
    {
        bool redraw = false;
        if ( colorCycleClock.tick() )
        {
            size_t currentRotationSet = TilesetRotationSet[tileset];
            Rotator* rotatorSet = currentRotationSet < TotalRotatorSets ? RotatorSets[currentRotationSet] : NoRotators;
            for ( size_t rotatorIndex=0; rotatorIndex<8; rotatorIndex++ )
            {
                Rotator & rotator = rotatorSet[rotatorIndex];
                if ( rotator.enabled != Rotator::Enabled::No ) // Ensure this rotator is enabled/exists
                {
                    if ( rotator.ticksRemaining == 0 )
                    {
                        for ( auto paletteIndex = rotator.paletteIndexMax; paletteIndex > rotator.paletteIndexMin; paletteIndex-- )
                            std::swap(palette[paletteIndex], palette[paletteIndex-1]); // Swap adjacent colors starting from max and ending at min, net effect is each rotates one to the right

                        rotator.ticksRemaining = rotator.ticksBetweenRotations; // Reset the timer
                        redraw = true; // Flag that anything using the palette should be redrawn
                    }
                    else
                        rotator.ticksRemaining --; // Decrement the timer
                }
            }
        }

        return redraw;
    }
};

#endif