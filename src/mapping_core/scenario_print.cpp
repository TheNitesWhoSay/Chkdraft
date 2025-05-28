#include "scenario.h"
#include <cross_cut/logger.h>

extern Logger logger;

void Scenario::printChangeHistory()
{
    logger.info("printChangeHistory(std::cout)");
    //Tracked::printChangeHistory(std::cout);
}

void Scenario::afterAction(std::size_t actionIndex)
{
    if ( logger.getLogLevel() >= LogLevel::Trace )
    {
        RareEdit::Action action {};
        Tracked::renderAction(actionIndex, action);

        std::stringstream ss {};
        ss << "New Action[" << actionIndex << "]\n";

        auto & changeEvents = action.changeEvents;
        for ( std::size_t i=0; i<changeEvents.size(); ++i )
        {
            if ( i > 99 )
            {
                ss << "  (" << changeEvents.size()-100 << " additional events omitted)\n";
                break;
            }
            auto & changeEvent = changeEvents[i];
            ss << "  " << i << ": " << changeEvent.summary << '\n';
        }
        logger.trace() << ss.str();
    }
}
