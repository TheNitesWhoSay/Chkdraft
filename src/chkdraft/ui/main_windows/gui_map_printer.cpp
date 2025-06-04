#include "gui_map.h"
#include <chkdraft.h>

void GuiMap::afterAction(std::size_t actionIndex)
{
    RareEdit::RenderAction<DescriptorIndex> action {};
    Tracked::renderAction(actionIndex, action, true);
    chkd.mainPlot.leftBar.mainTree.historyTree.RefreshActionHeaders(std::make_optional(actionIndex));
    chkd.mainPlot.leftBar.mainTree.historyTree.InsertAction(actionIndex, action);
    if ( nonSelChangeCursor >= Tracked::previousCursorIndex() )
        nonSelChangeCursor = std::numeric_limits<std::size_t>::max(); // Previous nonSelChangeCursor was elided, reset indicating no change
    
    if ( !action.isSelChangeAction() && (actionIndex > nonSelChangeCursor || nonSelChangeCursor == std::numeric_limits<std::size_t>::max()) )
        nonSelChangeCursor = actionIndex+1; // Mark first non-selection change

    this->checkUnsavedChanges();

    if ( logger.getLogLevel() >= LogLevel::Trace )
    {
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
