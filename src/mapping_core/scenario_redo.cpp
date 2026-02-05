#include "scenario.h"
#include "cross_cut/logger.h"

extern Logger logger;

std::size_t Scenario::redoAction()
{
    return tracked::redo_action(); // For a large structure like MapData, undo/redo should have their own TU/.cpp file
}
