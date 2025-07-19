#include "gui_map.h"
#include <chkdraft.h>
#include <chkdraft/mapping/settings.h>

void GuiMap::afterAction(std::size_t actionIndex)
{
    checkSelChangeFlags();
    RareEdit::RenderAction<DescriptorIndex> action {};
    if ( skipEventRender )
    {
        Tracked::renderAction(actionIndex, action, false);
        skipEventRender = false;
    }
    else
        Tracked::renderAction(actionIndex, action, true);

    totalHistSizeInBytes += action.byteCount;
    chkd.mainPlot.leftBar.historyTree.RefreshActionHeaders(std::make_optional(actionIndex));
    chkd.mainPlot.leftBar.historyTree.InsertAction(actionIndex, action);
    if ( nonSelChangeCursor > Tracked::previousCursorIndex() )
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

    // Trimming actions will invalidate the actionIndex, so do this after everything else
    if ( actionIndex+1 > Settings::maxHistActions || totalHistSizeInBytes >= static_cast<u64>(Settings::maxHistMemoryUsageMb)*1048576 ) // TODO: Reset
    {
        bool trimActionCountLimit = actionIndex+1 > Settings::maxHistActions;
        // If reached maxHistActions, trim first 20% of actions, e.g. max actions 500 = 100 trimmed
        // If reached maxHistMemoryUsageMb, find the actionIndex for which trimming the actions prior would bring memory use to 80% of maxHistMemoryUsageMb
        if ( trimActionCountLimit )
            Tracked::trimHistory(Settings::maxHistActions/5);
        else
            Tracked::trimHistoryToSize(Settings::maxHistMemoryUsageMb*1048576*4/5);

        totalHistSizeInBytes = chkd.mainPlot.leftBar.historyTree.RebuildHistoryTree();
    }
}
