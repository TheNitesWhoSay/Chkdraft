The solution "edit_resource.sln" is solely for editing the "resource.rc" file.

In visual studios, resource script files (.rc) cannot be used with visual studios resource editor in CMake solutions (they can be edited as code, but not using the gui editor).

This issue is due to the resource.rc includes (either winres.h or afxres.h) not resolving and there being no reasonable way to make them do so - and at least thus-far microsoft is treating this as a low-priority/won't fix issue https://developercommunity.visualstudio.com/t/cmake-project-cant-open-rc-file/172331

Alternative solutions include using a native resource template (.rct) file, but in addition to requiring rebuilding the dialogs and menus from scratch (which would be a significant effort which is probably best avoided in favor of time spent on actual cross-platform solutions) there is no option to edit native resource templates as code, a significant drawback for certain edit-and-test scenarios, conversion to/from parts of the codebase, and debugging... thus use of the "edit_resource.sln" solution.