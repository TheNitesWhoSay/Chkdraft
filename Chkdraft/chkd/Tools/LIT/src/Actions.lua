----------------------------------------------------------------------------------------------------
-- @file	Actions.lua
-- @author	Randy Gaul
-- @date	08/11/2014
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- Center View
----------------------------------------------------------------------------------------------------
function CenterView( )
	local object = {}
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:Center( )
		tofile( "\tCenter View(" .. self.location .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Unit
----------------------------------------------------------------------------------------------------
function Unit( )
	local object = {}
	
	function object:SetUnit( unit )
		self.unit = __WrapQuotes( unit )
	end
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:SetDestination( destination )
		self.destination = __WrapQuotes( destination )
	end
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:SetPercent( percent )
		self.percent = tostring( percent )
	end
	
	function object:SetHangarCount( hangarCount )
		self.hangarCount = tostring( hangarCount )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:SetGiveToPlayer( player )
		self.playerGiveTo = __Player( player )
	end
	
	function object:Create( )
		tofile( "\tCreate Unit(" .. self.player .. ", " .. self.unit .. ", " .. self.count .. ", " .. self.location .. ");\n" )
	end
	
	function object:Give( )
		tofile( "\tGive Units to Player(" .. self.player .. ", " .. self.playerGiveTo .. ", " .. self.unit .. ", " .. self.count .. ", " .. self.location .. ");\n" )
	end
	
	function object:Kill( )
		tofile( "\tKill Unit(" .. self.player .. ", " .. self.unit .. ");\n" )
	end
	
	function object:KillAt( )
		tofile( "\tKill Unit At Location(" .. self.player .. ", " .. self.unit .. ", " .. self.count .. ", " .. self.location .. ");\n" )
	end
	
	function object:ModifyEnergy( )
		tofile( "\tModify Unit Energy(" .. self.player .. ", " .. self.unit .. ", " .. self.count .. ", " .. self.percent .. ", " .. self.location .. ");\n" )
	end
	
	function object:ModifyHangarCount( )
		tofile( "\tModify Unit Hanger Count(" .. self.player .. ", " .. self.unit .. ", " .. self.hangarCount .. ", " .. self.count .. ", " .. self.location .. ");\n" )
	end
	
	function object:ModifyHitPoints( )
		tofile( "\tModify Unit Hit Points(" .. self.player .. ", " .. self.unit .. ", " .. self.percent .. ", " .. self.count .. ", " .. self.location .. ");\n" )
	end
	
	function object:ModifyResourceAmount( )
		tofile( "\tModify Unit Resource Amount(" .. self.player .. ", " .. self.percent .. ", " .. self.count .. ", " .. self.location .. ");\n" )
	end
	
	function object:ModifyShields( )
		tofile( "\tModify Unit Shield Points(" .. self.player .. ", " .. self.unit .. ", " .. self.percent .. ", " .. self.count .. ", " .. self.location .. ");\n" )
	end
	
	function object:Move( )
		tofile( "\tMove Unit(" .. self.player .. ", " .. self.unit .. ", " .. self.count .. ", " .. self.location .. ", " .. self.destination .. ");\n" )
	end
	
	function object:Remove( )
		tofile( "\tRemove Unit(" .. self.player .. ", " .. self.unit .. ");\n" )
	end
	
	function object:RemoveAt( )
		tofile( "\tRemove Unit At Location(" .. self.player .. ", " .. self.unit .. ", " .. self.count .. ", " .. self.location .. ");\n" )
	end
	
	function object:Attack( )
		tofile( "\tOrder(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", " .. self.destination .. ", attack);\n" )
	end
	
	function object:Order( )
		tofile( "\tOrder(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", " .. self.destination .. ", move);\n" )
	end
	
	function object:Patrol( )
		tofile( "\tOrder(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", " .. self.destination .. ", patrol);\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Leaderboard
----------------------------------------------------------------------------------------------------
function Leaderboard( )
	local object = {}
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:SetLabel( label )
		self.label = __WrapQuotes( label )
	end
	
	function object:SetUnit( unit )
		self.unit = __WrapQuotes( unit )
	end
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:SetType( name )
		if type( name ) ~= "string" then
			print( "\nERROR: Leaderboard:SetType( ) only accepts strings." )
			assert( type( name ) == "string" )
		end
		
		properType = false
		
		if name == "ore" then
			properType = true
		elseif name == "gas" then
			properType = true
		elseif name == "ore and gas" then
			properType = true
		elseif name == "Buildings" then
			properType = true
		elseif name == "Custom" then
			properType = true
		elseif name == "Kills" then
			properType = true
		elseif name == "Kills and razings" then
			properType = true
		elseif name == "Razings" then
			properType = true
		elseif name == "Total" then
			properType = true
		elseif name == "Units" then
			properType = true
		elseif name == "Units and buildings" then
			properType = true
		end
		
		if properType == false then
			print( "\nERROR: Name string must be one of the above strings." )
			assert( properType )
		end
		
		self.type = name
	end
	
	function object:ControlAt( )
		tofile( "\tLeader Board Control At Location(" .. self.label .. ", " .. self.unit .. ", " .. self.location .. ");\n" )
	end
	
	function object:Control( )
		tofile( "\tLeader Board Control(" .. self.label .. ", " .. self.unit .. ");\n" )
	end
	
	function object:Greed( )
		tofile( "\tLeaderboard Greed(" .. self.count .. ");\n" )
	end
	
	function object:Kills( )
		tofile( "\tLeader Board Kills(" .. self.label .. ", " .. self.unit .. ");\n" )
	end
	
	function object:Score( )
		tofile( "\tLeader Board Points(" .. self.label .. ", " .. self.type .. ");\n" )
	end
	
	function object:Resources( )
		tofile( "\tLeader Board Resources(" .. self.label .. ", " .. self.type .. ");\n" )
	end
	
	function object:EnableComputerPlayers( )
		tofile( "\tLeaderboard Computer Players(enabled);\n" );
	end
	
	function object:DisableComputerPlayers( )
		tofile( "\tLeaderboard Computer Players(disabled);\n" );
	end
	
	function object:ToggleComputerPlayers( )
		tofile( "\tLeaderboard Computer Players(toggle);\n" );
	end
	
	function object:ControlAtGoal( )
		tofile( "\tLeaderboard Goal Control At Location(" .. self.label .. ", " .. self.unit .. ", " .. self.count .. ", " .. self.location .. ");\n" )
	end
	
	function object:ControlGoal( )
		tofile( "\tLeaderboard Goal Control(" .. self.label .. ", " .. self.unit .. ", " .. self.count .. ");\n" )
	end
	
	function object:KillsGoal( )
		tofile( "\tLeaderboard Goal Kills(" .. self.label .. ", " .. self.unit .. ", " .. self.count .. ");\n" )
	end
	
	function object:ScoreGoal( )
		tofile( "\tLeaderboard Goal Points(" .. self.label .. ", " .. self.type .. ", " .. self.count .. ");\n" )
	end
	
	function object:ResourcesGoal( )
		tofile( "\tLeaderboard Goal Resources(" .. self.label .. ", " .. self.count .. ", " .. self.type .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Minimap Ping
----------------------------------------------------------------------------------------------------
function MinimapPing( )
	local object = {}
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:Ping( )
		tofile( "\tMinimap Ping(" .. self.location .. ");\n" )
	end

	return object
end

----------------------------------------------------------------------------------------------------
-- Move Location
----------------------------------------------------------------------------------------------------
function MoveLocation( )
	local object = {}
	
	function object:SetUnit( unit )
		self.unit = __WrapQuotes( unit )
	end
	
	function object:SetDestination( destination )
		self.destination = __WrapQuotes( destination )
	end
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:Move( )
		tofile( "\tMove Location(" .. self.player .. ", " .. self.unit .. ", " .. self.destination .. ", " .. self.location .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Run AI Script
----------------------------------------------------------------------------------------------------
function Script( )
	local object = {}
	
	function object:SetScript( script )
		self.script = __WrapQuotes( script )
	end
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:Run( )
		tofile( "\tRun AI Script(" .. self.script .. ");\n" );
	end
	
	function object:RunAt( )
		tofile( "\tRun AI Script At Location(" .. self.script .. ", " .. self.location .. ");\n" );
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Set Alliance Status
----------------------------------------------------------------------------------------------------
function Alliance( )
	local object = {}
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:AlliedVictory( )
		tofile( "\tSet Alliance Status(" .. self.player .. ", Allied Victory);\n" )
	end
	
	function object:Allied( )
		tofile( "\tSet Alliance Status(" .. self.player .. ", Ally);\n" )
	end
	
	function object:Enemy( )
		tofile( "\tSet Alliance Status(" .. self.player .. ", Enemy);\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Set Countdown Timer
----------------------------------------------------------------------------------------------------
function CountdownTimer( )
	local object = {}
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:Add( )
		tofile( "\tSet Countdown Timer(Add, " .. self.count .. ");\n" );
	end
	
	function object:SetTo( )
		tofile( "\tSet Countdown Timer(Set To, " .. self.count .. ");\n" );
	end
	
	function object:Subtract( )
		tofile( "\tSet Countdown Timer(Subtract, " .. self.count .. ");\n" );
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Set Doodad State
----------------------------------------------------------------------------------------------------
function DoodadState( )
	local object = {}
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:SetUnit( unit )
		self.unit = __WrapQuotes( unit )
	end
	
	function object:Disable( )
		tofile( "\tSet Doodad State(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", disabled);\n" );
	end
	
	function object:Enable( )
		tofile( "\tSet Doodad State(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", enabled);\n" );
	end
	
	function object:Toggle( )
		tofile( "\tSet Doodad State(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", toggle);\n" );
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Set Invincibility
----------------------------------------------------------------------------------------------------
function Invincibility( )
	local object = {}
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:SetUnit( unit )
		self.unit = __WrapQuotes( unit )
	end
	
	function object:Disable( )
		tofile( "\tSet Invincibility(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", disabled);\n" );
	end
	
	function object:Enable( )
		tofile( "\tSet Invincibility(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", enabled);\n" );
	end
	
	function object:Toggle( )
		tofile( "\tSet Invincibility(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", toggle);\n" );
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Set Resources
----------------------------------------------------------------------------------------------------
-- See Conditions.lua

----------------------------------------------------------------------------------------------------
-- Set Score
----------------------------------------------------------------------------------------------------
-- See Conditions.lua

----------------------------------------------------------------------------------------------------
-- Set Switch
----------------------------------------------------------------------------------------------------
-- See Conditions.lua


----------------------------------------------------------------------------------------------------
-- Transmission
----------------------------------------------------------------------------------------------------
function Transmission( )
	local object = {}
	
	function object:SetFile( fileName )
		-- Retrieve file extension
        local ext = string.match( fileName, "([^.]+)$" )
		
		if ext ~= "wav" and ext ~= "WAV" then
			print( "\nERROR: PlayWAV:Play( ) only accepts strings for a file path to a .wav file." )
			assert( type( name ) == "string" )
		end
		
		local fileNameAndPath = "staredit/wav/" .. fileName
		fileNameAndPath = fileNameAndPath:gsub( "\\", "\\\\" )
		fileNameAndPath = fileNameAndPath:gsub( "/", "\\\\" )
		
		self.file = __WrapQuotes( fileNameAndPath )
	end
	
	function object:SetUnit( unit )
		self.unit = __WrapQuotes( unit )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:SetText( text )
		self.text = __WrapQuotes( text )
	end
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:Add( )
		tofile( "\tTransmission(Always Display, " .. self.text .. ", " .. self.unit .. ", " .. self.location .. ", Add, " .. self.count .. ", " .. self.file .. ", 0);\n" );
	end
	
	function object:SetTo( )
		tofile( "\tTransmission(Always Display, " .. self.text .. ", " .. self.unit .. ", " .. self.location .. ", Set To, " .. self.count .. ", " .. self.file .. ", 0);\n" );
	end
	
	function object:Subtract( )
		tofile( "\tTransmission(Always Display, " .. self.text .. ", " .. self.unit .. ", " .. self.location .. ", Subtract, " .. self.count .. ", " .. self.file .. ", 0);\n" );
	end
	
	return object
end
