#include "scenario.h"
#include <cross_cut/logger.h>

extern Logger logger;

void Scenario::undoAction()
{
    Tracked::undoAction(); // For a large structure like MapData, undo/redo should have their own TU/.cpp file
}
