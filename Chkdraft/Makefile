CC=g++
CFLAGS=-m32 -g -std=gnu++11 -Isrc -I../SFmpqapi -fpermissive \
 -DDEBUG -D_DEBUG -DMINGW_HAS_SECURE_API -DCHKDRAFT -DCHKD_DEBUG -DNOMINMAX -DCRT_SECURE_NO_WARNINGS \
 -DUSESTATICSFMPQ

LDFLAGS=-m32 -static -mwindows -L../SFmpqapi -L../SComp -L../SComp/zlib -L ../SComp/bzip2
LIBS=resource.o -lcomdlg32 -lcomctl32 -lSFmpq_static -lSComp_static -lz -lbz2

OBJECTS=build/ChangePassword.o \
build/ChkdPlugins.o \
build/Chkdraft.o \
build/ChkdStringInput.o \
build/Clipboard.o \
build/CndActGrid.o \
build/CommonFiles/Constants.o \
build/CommonFiles/Debug.o \
build/CommonFiles/Structs.o \
build/Counters.o \
build/CUWPs.o \
build/EnterPassword.o \
build/Forces.o \
build/Graphics.o \
build/GuiMap.o \
build/LeftBar.o \
build/LIT.o \
build/LocationChange.o \
build/LocationCreateDel.o \
build/LocationMove.o \
build/LocationProperties.o \
build/LocationTree.o \
build/Main.o \
build/MainPlot.o \
build/MainToolbar.o \
build/MainTree.o \
build/MappingCore/Basics.o \
build/MappingCore/Buffer.o \
build/MappingCore/ChkAction.o \
build/MappingCore/ChkCondition.o \
build/MappingCore/ChkCuwp.o \
build/MappingCore/ChkDoodad.o \
build/MappingCore/ChkLocation.o \
build/MappingCore/ChkSprite.o \
build/MappingCore/ChkTrigger.o \
build/MappingCore/ChkUnit.o \
build/MappingCore/DefaultCHK.o \
build/MappingCore/EscapeStrings.o \
build/MappingCore/FileIO.o \
build/MappingCore/MapFile.o \
build/MappingCore/Math.o \
build/MappingCore/Scenario.o \
build/MappingCore/sha256.o \
build/MappingCore/StringTableNode.o \
build/MappingCore/StringUsage.o \
build/MapProperties.o \
build/Maps.o \
build/MapSettings.o \
build/MiniMap.o \
build/MoveTo.o \
build/NewMap.o \
build/PeerListBox.o \
build/PlayerDropdown.o \
build/PreservedUnitStats.o \
build/Reversibles.o \
build/ScData.o \
build/Selections.o \
build/StaticTrigComponentParser.o \
build/StringEditor.o \
build/StringGuide.o \
build/StringPreview.o \
build/Suggestions.o \
build/Switches.o \
build/TechSettings.o \
build/TechTree.o \
build/Templates.o \
build/TerrainPalette.o \
build/TextTrig.o \
build/TextTrigCompiler.o \
build/TextTrigGenerator.o \
build/TileChange.o \
build/TileProperties.o \
build/TrigActions.o \
build/TrigConditions.o \
build/TrigEditor.o \
build/TrigGeneral.o \
build/Triggers.o \
build/TrigModify.o \
build/TrigModifyText.o \
build/TrigPlayers.o \
build/Undo.o \
build/UnitChange.o \
build/UnitCreateDel.o \
build/UnitIndexMove.o \
build/UnitProperties.o \
build/UnitSettings.o \
build/UnitTree.o \
build/UpgradeSettings.o \
build/UpgradeTree.o \
build/WavEditor.o \
build/WindowsUI/ButtonControl.o \
build/WindowsUI/CheckBoxControl.o \
build/WindowsUI/ClassDialog.o \
build/WindowsUI/ClassWindow.o \
build/WindowsUI/DragListControl.o \
build/WindowsUI/DropdownControl.o \
build/WindowsUI/EditControl.o \
build/WindowsUI/GridItem.o \
build/WindowsUI/GridView.o \
build/WindowsUI/GroupBoxControl.o \
build/WindowsUI/ListBoxControl.o \
build/WindowsUI/ListViewControl.o \
build/WindowsUI/MdiClient.o \
build/WindowsUI/RadioButtonControl.o \
build/WindowsUI/StatusControl.o \
build/WindowsUI/TabControl.o \
build/WindowsUI/TextControl.o \
build/WindowsUI/ToolbarControl.o \
build/WindowsUI/TreeViewControl.o \
build/WindowsUI/WindowControl.o \
build/WindowsUI/WindowMenu.o \
build/WindowsUI/WindowsItem.o 

OUTDIR=bin
EXECUTABLE=$(OUTDIR)/chkdraft.exe

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) $(DEPS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	
.PHONY: clean

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(DEPS)