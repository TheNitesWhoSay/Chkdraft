#include "scenario.h"
#include "cross_cut/logger.h"

extern Logger logger;

void Scenario::printChangeHistory()
{
    Tracked::printChangeHistory(logger); // For a large structure like MapData, print should have its own TU/.cpp file
    logger.info() << "RedoCount: " << RareTs::whitebox(*this).redoCount << "\n";
    logger.info() << "RedoSize: " << RareTs::whitebox(*this).redoSize << "\n";
    logger.info() << "TotalActions: " << RareTs::whitebox(*this).actions.size() << "\n";
}
