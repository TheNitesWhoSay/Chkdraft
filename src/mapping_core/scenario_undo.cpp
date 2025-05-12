#include "scenario.h"
#include <cross_cut/logger.h>

extern Logger logger;

void Scenario::undoAction()
{
    logger.info("undoAction()");
    //Tracked::undoAction();
}
