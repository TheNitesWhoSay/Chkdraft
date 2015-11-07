----------------------------------------------------------------------------------------------------
-- @file	ShorthandActions.lua
-- @author	Randy Gaul
-- @date	08/14/2014
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- Resource manipulation
----------------------------------------------------------------------------------------------------
function AddResource( player, count, resource )
	tofile( "\tSet Resources(" .. __Player( player ) ..  ", Add, " .. __Count( count ) .. ", " .. __Resource( resource ) .. ");\n" )
end

function SetResource( player, count, resource )
	tofile( "\tSet Resources(" .. __Player( player ) ..  ", Set To, " .. __Count( count ) .. ", " .. __Resource( resource ) .. ");\n" )
end

function SubtractResource( player, count, resource )
	tofile( "\tSet Resources(" .. __Player( player ) ..  ", Subtract, " .. __Count( count ) .. ", " .. __Resource( resource ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Display Message
----------------------------------------------------------------------------------------------------
function Text( text )
	tofile( "\tDisplay Text Message(Always Display," .. __WrapQuotes( text ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Comment
----------------------------------------------------------------------------------------------------
function Comment( commentString )
	tofile( "\tComment(\"" .. commentString .. "\");\n" )
end

----------------------------------------------------------------------------------------------------
-- Mute Unit Speech
----------------------------------------------------------------------------------------------------
function MuteSpeech( )
	tofile( "\tMute Unit Speech();\n" )
end

----------------------------------------------------------------------------------------------------
-- Move Location
----------------------------------------------------------------------------------------------------
function CenterLocation( location, player, unit, destination )
	tofile( "\tMove Location(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ", " .. __Location( destination ) .. ", " .. __Location( location ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Give Unit
----------------------------------------------------------------------------------------------------
function GiveUnit( count, unit, location, playerFrom, playerTo )
	tofile( "\tGive Units to Player(" .. __Player( playerFrom ) .. ", " .. __Player( playerTo ) .. ", " .. __Unit( unit ) .. ", " .. __Count( count ).. ", " .. __Location( location ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Move Unit
----------------------------------------------------------------------------------------------------
function MoveUnit( player, unit, count, location, destination )
	tofile( "\tMove Unit(" .. __Player( player ).. ", " .. __Unit( unit  ).. ", " .. __Count( count ) .. ", " .. __Location( location ).. ", " .. __Location( destination ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Create Unit
----------------------------------------------------------------------------------------------------
function CreateUnit( player, unit, count, location )
	tofile( "\tCreate Unit(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ", " .. __Count( count ) .. ", " .. __Location( location ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Order
----------------------------------------------------------------------------------------------------
function Order( player, unit, location, destination, order )
	tofile( "\tOrder(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ", " .. __Location( location ) .. ", " .. __Location( destination ) .. ", " .. (order or "move") .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Center View
----------------------------------------------------------------------------------------------------
function MoveScreen( location )
	tofile( "\tCenter View(" .. __Location( location ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Set Invincibility
----------------------------------------------------------------------------------------------------
function SetInvincibility( player, unit, location, state )
	tofile( "\tSet Invincibility(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ", " .. __Location( location ) .. ", " .. __State( state ) .. ");\n" );
end

----------------------------------------------------------------------------------------------------
-- Kill Unit
----------------------------------------------------------------------------------------------------
function KillUnit( player, unit )
	tofile( "\tKill Unit(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Kill Unit At
----------------------------------------------------------------------------------------------------
function KillUnitAt( player, unit, count, location )
	tofile( "\tKill Unit At Location(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ", " .. __Count( count ) .. ", " .. __Location( location ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Remove Unit
----------------------------------------------------------------------------------------------------
function RemoveUnit( player, unit )
	tofile( "\tRemove Unit(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Remove Unit At
----------------------------------------------------------------------------------------------------
function RemoveUnitAt( player, unit, count, location )
	tofile( "\tRemove Unit At Location(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ", " .. __Count( count ) .. ", " .. __Location( location ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Pause Game
----------------------------------------------------------------------------------------------------
function PauseGame( )
	tofile( "\tPause Game();\n" )
end

----------------------------------------------------------------------------------------------------
-- Pause Countdown Timer
----------------------------------------------------------------------------------------------------
function PauseTimer( )
	tofile( "\tPause Timer();\n" )
end

----------------------------------------------------------------------------------------------------
-- Play WAV (for imported sounds)
----------------------------------------------------------------------------------------------------
function PlayWAV( fileName )
	tofile( "\tPlay WAV(" .. __WAV( fileName ) .. ", 0);\n" )
end

----------------------------------------------------------------------------------------------------
-- Play WAV 2 (for sounds within brood war itself)
----------------------------------------------------------------------------------------------------
function PlayWAV2( fileName )
	tofile( "\tPlay WAV(" .. __WrapQuotes( fileName ) .. ", 0);\n" )
end

----------------------------------------------------------------------------------------------------
-- Preserve Trigger
----------------------------------------------------------------------------------------------------
function Preserve( )
	tofile( "\tPreserve Trigger();\n" )
end

----------------------------------------------------------------------------------------------------
-- SetHP
----------------------------------------------------------------------------------------------------
function SetHP( player, unit, percent, count, location )
	tofile( "\tModify Unit Hit Points(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ", " .. __Count( percent ) .. ", " .. __Count( count ) .. ", " .. __Location( location ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Set Mission Objectives
----------------------------------------------------------------------------------------------------
function SetMissionObjectives( objectives )
	tofile( "\tSet Mission Objectives(\"" .. objectives .. "\");\n" )
end

----------------------------------------------------------------------------------------------------
-- Set Next Scenario
----------------------------------------------------------------------------------------------------
function SetNextScenario( scenario )
	tofile( "\tSet Next Scenario(\"" .. scenario .. "\");\n" )
end

----------------------------------------------------------------------------------------------------
-- Show Portait
----------------------------------------------------------------------------------------------------
function ShowPortrait( unit, milliseconds )
	tofile( "\tTalking Portrait(\"" .. unit .. "\", " .. tostring( milliseconds ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Dialog for shorthand Transmission
----------------------------------------------------------------------------------------------------
function Dialog( location, unit, player, milliseconds, text, sound )
	local trans = Transmission( )
	trans:SetFile( sound or "LTTP_Text_Done.wav" )
	trans:SetUnit( unit )
	trans:SetPlayer( player )
	text = text:gsub( "\n", "\\n" )
	text = text:gsub( "\r", "\\r" )
	trans:SetText( text )
	trans:SetLocation( location )
	trans:SetCount( milliseconds )
	trans:SetTo( )
end

----------------------------------------------------------------------------------------------------
-- Ping (on minimap)
----------------------------------------------------------------------------------------------------
function Ping( location )
	tofile( "\tMinimap Ping(" .. __WrapQuotes( location ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Run AI Script
----------------------------------------------------------------------------------------------------
function RunScript( script )
	tofile( "\tRun AI Script(" .. __WrapQuotes( script ) .. ");\n" );
end

function RunScriptAt( script, location )
		tofile( "\tRun AI Script At Location(" .. __WrapQuotes( script ) .. ", " .. __WrapQuotes( location ) .. ");\n" );
end

----------------------------------------------------------------------------------------------------
-- Set Alliance Status
----------------------------------------------------------------------------------------------------
function Ally( player )
	tofile( "\tSet Alliance Status(" .. __Player( player ) .. ", Ally);\n" )
end

function Enemy( )
	tofile( "\tSet Alliance Status(" .. __Player( player ) .. ", Enemy);\n" )
end

----------------------------------------------------------------------------------------------------
-- Unmute Speech
----------------------------------------------------------------------------------------------------
function UnmuteSpeech( )
	tofile( "\tUnmute Unit Speech();\n" );
end

----------------------------------------------------------------------------------------------------
-- Unpause Speech
----------------------------------------------------------------------------------------------------
function UnpauseGame( )
	tofile( "\tUnpause Game();\n" )
end

----------------------------------------------------------------------------------------------------
-- Unpause Countdown Timer
----------------------------------------------------------------------------------------------------
function UnpauseTimer( )
	tofile( "\tUnpause Timer();\n" )
end

----------------------------------------------------------------------------------------------------
-- Victory
----------------------------------------------------------------------------------------------------
function Victory( )
	tofile( "\tVictory();\n" )
end

----------------------------------------------------------------------------------------------------
-- Wait
----------------------------------------------------------------------------------------------------
function Wait( milliseconds )
	tofile( "\tWait(" .. tostring( milliseconds ) .. ");\n" )
end
