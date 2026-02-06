#include "gui_map.h"
#include <chkdraft.h>
#include <chkdraft/mapping/chkd_profiles.h>

void GuiMap::after_action(std::size_t actionIndex)
{
    checkSelChangeFlags();
    nf::rendered_action<DescriptorIndex> action {};
    if ( skipEventRender )
    {
        render_action(actionIndex, action, false);
        skipEventRender = false;
    }
    else
        render_action(actionIndex, action, true);

    totalHistSizeInBytes += action.byte_count;
    chkd.mainPlot.leftBar.historyTree.RefreshActionHeaders(std::make_optional(actionIndex));
    chkd.mainPlot.leftBar.historyTree.InsertAction(actionIndex, action);
    if ( nonSelChangeCursor > tracked::previous_cursor_index() )
        nonSelChangeCursor = std::numeric_limits<std::size_t>::max(); // Previous nonSelChangeCursor was elided, reset indicating no change
    
    if ( !action.is_sel_change_action() && (actionIndex > nonSelChangeCursor || nonSelChangeCursor == std::numeric_limits<std::size_t>::max()) )
        nonSelChangeCursor = actionIndex+1; // Mark first non-selection change

    this->checkUnsavedChanges();

    if ( logger.getLogLevel() >= LogLevel::Trace )
    {
        std::stringstream ss {};
        ss << "New Action[" << actionIndex << "]\n";

        auto & changeEvents = action.change_events;
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

    auto maxHistActions = chkd.profiles().history.maxActions;
    auto maxHistMemoryUseMb = chkd.profiles().history.maxMemoryUseMb;

    // Trimming actions will invalidate the actionIndex, so do this after everything else
    if ( actionIndex+1 > maxHistActions || totalHistSizeInBytes >= static_cast<u64>(maxHistMemoryUseMb)*1048576 )
    {
        bool trimActionCountLimit = actionIndex+1 > maxHistActions;
        // If reached maxHistActions, trim first 20% of actions, e.g. max actions 500 = 100 trimmed
        // If reached maxHistMemoryUsageMb, find the actionIndex for which trimming the actions prior would bring memory use to 80% of maxHistMemoryUsageMb
        if ( trimActionCountLimit )
            tracked::trim_history(maxHistActions/5);
        else
            tracked::trim_history_to_size(maxHistMemoryUseMb*1048576*4/5);

        totalHistSizeInBytes = chkd.mainPlot.leftBar.historyTree.RebuildHistoryTree();
    }
}
