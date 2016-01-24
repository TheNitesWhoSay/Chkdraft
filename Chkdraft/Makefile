CC=g++
CFLAGS=-m32 -fleading-underscore -g -std=gnu++11 -Isrc -ISFmpq -fpermissive -DDEBUG -D_DEBUG -DMINGW_HAS_SECURE_API -DCHKDRAFT -DCHKD_DEBUG -DNOMINMAX -DCRT_SECURE_NO_WARNINGS

LDFLAGS=-m32 -mwindows -L. -LSFmpq -Lsrc/MappingCore
LIBS=resource.o -lcomdlg32 -lcomctl32 -lSFmpq
SOURCES=src/ChangePassword.cpp \
src/ChkdPlugins.cpp \
src/Chkdraft.cpp \
src/ChkdStringInput.cpp \
src/Clipboard.cpp \
src/CndActGrid.cpp \
src/CommonFiles/Constants.cpp \
src/CommonFiles/Debug.cpp \
src/CommonFiles/Structs.cpp \
src/Counters.cpp \
src/CUWPs.cpp \
src/EnterPassword.cpp \
src/Forces.cpp \
src/Graphics.cpp \
src/GuiMap.cpp \
src/LeftBar.cpp \
src/LIT.cpp \
src/LocationChange.cpp \
src/LocationCreateDel.cpp \
src/LocationMove.cpp \
src/LocationProperties.cpp \
src/LocationTree.cpp \
src/Main.cpp \
src/MainPlot.cpp \
src/MainToolbar.cpp \
src/MainTree.cpp \
src/MappingCore/Basics.cpp \
src/MappingCore/Buffer.cpp \
src/MappingCore/ChkAction.cpp \
src/MappingCore/ChkCondition.cpp \
src/MappingCore/ChkCuwp.cpp \
src/MappingCore/ChkDoodad.cpp \
src/MappingCore/ChkLocation.cpp \
src/MappingCore/ChkSprite.cpp \
src/MappingCore/ChkTrigger.cpp \
src/MappingCore/ChkUnit.cpp \
src/MappingCore/DefaultCHK.cpp \
src/MappingCore/EscapeStrings.cpp \
src/MappingCore/FileIO.cpp \
src/MappingCore/MapFile.cpp \
src/MappingCore/Math.cpp \
src/MappingCore/Scenario.cpp \
src/MappingCore/sha256.cpp \
src/MappingCore/StringTableNode.cpp \
src/MappingCore/StringUsage.cpp \
src/MapProperties.cpp \
src/Maps.cpp \
src/MapSettings.cpp \
src/MiniMap.cpp \
src/MoveTo.cpp \
src/NewMap.cpp \
src/PeerListBox.cpp \
src/PlayerDropdown.cpp \
src/PreservedUnitStats.cpp \
src/Reversibles.cpp \
src/ScData.cpp \
src/Selections.cpp \
src/StaticTrigComponentParser.cpp \
src/StringEditor.cpp \
src/StringGuide.cpp \
src/StringPreview.cpp \
src/Suggestions.cpp \
src/Switches.cpp \
src/TechSettings.cpp \
src/TechTree.cpp \
src/Templates.cpp \
src/TerrainPalette.cpp \
src/TextTrig.cpp \
src/TextTrigCompiler.cpp \
src/TextTrigGenerator.cpp \
src/TileChange.cpp \
src/TileProperties.cpp \
src/TrigActions.cpp \
src/TrigConditions.cpp \
src/TrigEditor.cpp \
src/TrigGeneral.cpp \
src/Triggers.cpp \
src/TrigModify.cpp \
src/TrigModifyText.cpp \
src/TrigPlayers.cpp \
src/Undo.cpp \
src/UnitChange.cpp \
src/UnitCreateDel.cpp \
src/UnitIndexMove.cpp \
src/UnitProperties.cpp \
src/UnitSettings.cpp \
src/UnitTree.cpp \
src/UpgradeSettings.cpp \
src/UpgradeTree.cpp \
src/WavEditor.cpp \
src/WindowsUI/ButtonControl.cpp \
src/WindowsUI/CheckBoxControl.cpp \
src/WindowsUI/ClassDialog.cpp \
src/WindowsUI/ClassWindow.cpp \
src/WindowsUI/DragListControl.cpp \
src/WindowsUI/DropdownControl.cpp \
src/WindowsUI/EditControl.cpp \
src/WindowsUI/GridItem.cpp \
src/WindowsUI/GridView.cpp \
src/WindowsUI/GroupBoxControl.cpp \
src/WindowsUI/ListBoxControl.cpp \
src/WindowsUI/ListViewControl.cpp \
src/WindowsUI/MdiClient.cpp \
src/WindowsUI/RadioButtonControl.cpp \
src/WindowsUI/StatusControl.cpp \
src/WindowsUI/TabControl.cpp \
src/WindowsUI/TextControl.cpp \
src/WindowsUI/ToolbarControl.cpp \
src/WindowsUI/TreeViewControl.cpp \
src/WindowsUI/WindowControl.cpp \
src/WindowsUI/WindowMenu.cpp \
src/WindowsUI/WindowsItem.cpp 

OBJECTS=$(subst src/,build/,$(SOURCES:.cpp=.o))
OUTDIR=bin
EXECUTABLE=$(OUTDIR)/chkdraft.exe

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) $(OUTDIR)/SFmpq.dll 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

$(OUTDIR)/SFmpq.dll:
	cp SFmpq/SFmpq.dll $(OUTDIR)/

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
.PHONY: clean

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)