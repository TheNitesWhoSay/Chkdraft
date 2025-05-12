#include "scenario.h"
#include <cross_cut/logger.h>

extern Logger logger;

void Scenario::redoAction()
{
    logger.info("redoAction()");
    //Tracked::redoAction();
}
