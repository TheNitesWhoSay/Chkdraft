#ifndef MAPTIMINGS_H
#define MAPTIMINGS_H
#include "renderer.h"
#include "sc_map.h"
#include <chrono>
#include <iostream>

// Helper structure for visualizing all the timings related to generated a map image
struct MapTimings
{
    int totalTimeMs = 0;
    int mapLoadTimeMs = 0;
    int loadSkinAndTilesetTimeMs = 0;
    int animRealTimeMs = 0;
    int animGameTimeMs = 0;
    int animTicks = 0;
    int renderTimeMs = 0;
    int encodeTimeMs = 0; // Time for WebPEncodeLosslessRGB https://developers.google.com/speed/webp/docs/api , usually dwarves other times
    int outFileTimeMs = 0; // Time to write to disk

    MapTimings(std::chrono::high_resolution_clock::time_point mapStartTime,
        std::chrono::high_resolution_clock::time_point mapLoadTime,
        ScMap::Simulation animSimTimes,
        std::optional<Renderer::SaveWebpResult> saveWebpResult,
        std::chrono::high_resolution_clock::time_point mapFinishTime) :
        totalTimeMs(std::chrono::duration_cast<std::chrono::milliseconds>(mapFinishTime-mapStartTime).count()),
        mapLoadTimeMs(std::chrono::duration_cast<std::chrono::milliseconds>(mapLoadTime-mapStartTime).count()),
        loadSkinAndTilesetTimeMs(saveWebpResult ? saveWebpResult->loadSkinAndTilesetTimeMs : -1),
        animRealTimeMs(animSimTimes.realTimeSpentMs),
        animGameTimeMs(animSimTimes.gameTimeSimulatedMs),
        animTicks(animSimTimes.ticks),
        renderTimeMs(saveWebpResult ? saveWebpResult->renderTimeMs : -1),
        encodeTimeMs(saveWebpResult ? saveWebpResult->encodeTimeMs : -1),
        outFileTimeMs(saveWebpResult ? saveWebpResult->outFileTimeMs : -1)
    {}

    inline friend std::ostream & operator<<(std::ostream & os, const MapTimings & time)
    {
        os << "Load skin+tileset: " << time.loadSkinAndTilesetTimeMs << "ms\n"
            << "Map load: " << time.mapLoadTimeMs << "ms\n"
            << "Animation: " << time.animRealTimeMs << "ms (simulated " << time.animTicks << " ticks = " << time.animGameTimeMs << "ms of game time)\n"
            << "Render: " << time.renderTimeMs << "ms\n"
            << "WebPEncodeLosslessRGB: " << time.encodeTimeMs << "ms\n"
            << "File output: " << time.outFileTimeMs << "ms\n"
            << "Total: " << time.totalTimeMs << "ms\n";

        return os;
    }
};

#endif