----------------------------------------------------------------------------------------------------
-- @file	Conditions.lua
-- @author	Randy Gaul
-- @date	08/11/2014
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- Resources
----------------------------------------------------------------------------------------------------
function Resources( )
	local object = {}
	
	function object:SetType( resource )
		if type( resource ) ~= "string" then
			print( "\nERROR: Resources:SetType( ) only accepts strings." )
			assert( type( resource ) == "string" )
		end
		
		properType = false
		
		if resource == "ore" then
			properType = true
		elseif resource == "gas" then
			properType = true
		elseif resource == "ore and gas" then
			properType = true
		end
		
		if not properType then
			print( "\nERROR: resource string must be one of the above strings." )
			assert( properType )
		end
		
		self.resource = resource
	end
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:TheMost( )
		tofile( "\tMost Resources(" .. self.resource .. ");\n" )
	end
	
	function object:TheLeast( )
		tofile( "\tLeast Resources(" .. self.resource .. ");\n" )
	end
	
	function object:AtLeast( )
		tofile( "\tAccumulate(" .. self.player .. ", At least, " .. self.count .. ", " .. self.resource .. ");\n" )
	end
	
	function object:AtMost( )
		tofile( "\tAccumulate(" .. self.player .. ", At Most, " .. self.count .. ", " .. self.resource .. ");\n" )
	end
	
	function object:Exactly( )
		tofile( "\tAccumulate(" .. self.player .. ", Exactly, " .. self.count .. ", " .. self.resource .. ");\n" )
	end
	
	-- @HACK placed some actions in here
	function object:Add( )
		tofile( "\tSet Resources(" .. self.player ..  ", Add, " .. self.count .. ", " .. self.resource .. ");\n" )
	end
	
	function object:SetTo( )
		tofile( "\tSet Resources(" .. self.player ..  ", Set To, " .. self.count .. ", " .. self.resource .. ");\n" )
	end
	
	function object:Subtract( )
		tofile( "\tSet Resources(" .. self.player ..  ", Subtract, " .. self.count .. ", " .. self.resource .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Always
----------------------------------------------------------------------------------------------------
function Always( )
	tofile( "\tAlways();\n" )
end

----------------------------------------------------------------------------------------------------
-- Bring
----------------------------------------------------------------------------------------------------
function Bring( )
	local object = {}
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:SetUnit( unit )
		self.unit = __WrapQuotes( unit )
	end
	
	function object:AtLeast( )
		tofile( "\tBring(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", At least, " .. self.count .. ");\n" )
	end
	
	function object:AtMost( )
		tofile( "\tBring(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", At Most, " .. self.count .. ");\n" )
	end
	
	function object:Exactly( )
		tofile( "\tBring(" .. self.player .. ", " .. self.unit .. ", " .. self.location .. ", Exactly, " .. self.count .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Command
----------------------------------------------------------------------------------------------------
function Command( )
	local object = {}
	
	function object:SetLocation( location )
		self.location = __WrapQuotes( location )
	end
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:SetUnit( unit )
		self.unit = __WrapQuotes( unit )
	end
	
	function object:AtLeast( )
		tofile( "\tCommand(" .. self.player ..  ", " .. self.unit .. ", At least, " .. self.count .. ");\n" )
	end
	
	function object:AtMost( )
		tofile( "\tCommand(" .. self.player ..  ", " .. self.unit .. ", At Most, " .. self.count .. ");\n" )
	end
	
	function object:Exactly( )
		tofile( "\tCommand(" .. self.player ..  ", " .. self.unit .. ", Exactly, " .. self.count .. ");\n" )
	end
	
	function object:TheMost( )
		tofile( "\tCommand the Most(" .. self.unit .. ");\n" )
	end
	
	function object:TheMostAt( )
		tofile( "\tCommands the Most At(" .. self.unit .. ", " .. self.location .. ");\n" )
	end
	
	function object:TheLeast( )
		tofile( "\tCommand the Least(" .. self.unit .. ");\n" )
	end
	
	function object:TheLeastAt( )
		tofile( "\tCommand the Least At(" .. self.unit .. ", " .. self.location .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Countdown Timer
----------------------------------------------------------------------------------------------------
function CountdownTimer( )
	local object = {}
	
	function object:SetCount( count )
		self.count = count
	end
	
	function object:AtLeast( )
		tofile( "\tCountdown Timer(At least, " .. self.count .. ");\n" )
	end
	
	function object:AtMost( )
		tofile( "\tCountdown Timer(At Most, " .. self.count .. ");\n" )
	end
	
	function object:Exactly( )
		tofile( "\tCountdown Timer(Exactly, " .. self.count .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Elapsed Scenario Time
----------------------------------------------------------------------------------------------------
function ElapsedTime( )
	local object = {}
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:AtLeast( )
		tofile( "\tElapsed Time(" .. "At least, " .. self.count .. ");\n" )
	end
	
	function object:AtMost( )
		tofile( "\tElapsed Time(" .. "At Most, " .. self.count .. ");\n" )
	end
	
	function object:Exactly( )
		tofile( "\tElapsed Time(" .. "Exactly, " .. self.count .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Score
----------------------------------------------------------------------------------------------------
function Score( )
	local object = {}
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:SetType( name )
		if type( name ) ~= "string" then
			print( "\nERROR: HighestScore:SetType( ) only accepts strings." )
			assert( type( name ) == "string" )
		end
		
		properType = false
		
		if name == "Buildings" then
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
		
		if not properType then
			print( "\nERROR: Name string must be one of the above strings." )
			assert( properType )
		end
		
		self.type = name
	end
	
	function object:Highest( )
		tofile( "\tHighest Score(" .. self.type .. ");\n" )
	end
	
	function object:Lowest( )
		tofile( "\tLowest Score(" .. self.type .. ");\n" )
	end
	
	function object:AtLeast( )
		tofile( "\tScore(" .. self.player .. ", " .. self.type ..  ", At least, " .. self.count .. ");\n" )
	end
	
	function object:AtMost( )
		tofile( "\tScore(" .. self.player .. ", " .. self.type ..  ", At Most, " .. self.count .. ");\n" )
	end
	
	function object:Exactly( )
		tofile( "\tScore(" .. self.player .. ", " .. self.type ..  ", Exactly, " .. self.count .. ");\n" )
	end
	
	-- @HACK placed some actions in here
	function object:Add( )
		tofile( "\tSet Score(" .. self.player ..  ", Add, " .. self.count .. ", " .. self.type .. ");\n" )
	end
	
	function object:SetTo( )
		tofile( "\tSet Score(" .. self.player ..  ", Set To, " .. self.count .. ", " .. self.type .. ");\n" )
	end
	
	function object:Subtract( )
		tofile( "\tSet Score(" .. self.player ..  ", Subtract, " .. self.count .. ", " .. self.type .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Kills
----------------------------------------------------------------------------------------------------
function Kills( )
	local object = {}
	
	function object:SetUnit( unit )
		self.unit = __WrapQuotes( unit )
	end
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:TheMost( )
		tofile( "\tMost Kills(" .. self.unit .. ");\n" )
	end
	
	function object:TheLeast( )
		tofile( "\tLeast Kills(" .. self.unit .. ");\n" )
	end
	
	function object:AtLeast( )
		tofile( "\tKill(" .. self.player .. ", " .. self.unit .. ", At least, " .. self.count .. ");\n" )
	end
	
	function object:AtMost( )
		tofile( "\tKill(" .. self.player .. ", " .. self.unit .. ", At Most, " .. self.count .. ");\n" )
	end
	
	function object:Exactly( )
		tofile( "\tKill(" .. self.player .. ", " .. self.unit .. ", Exactly, " .. self.count .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Memory
----------------------------------------------------------------------------------------------------
function Memory( )
	local object = {}
	
	function object:SetAddress( address )
		self.address = address
	end
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:AtLeast( )
		tofile( "\tMemory(" .. self.address .. ", At least, " .. self.count .. ");\n" )
	end
	
	function object:AtMost( )
		tofile( "\tMemory(" .. self.address .. ", At Most, " .. self.count .. ");\n" )
	end
	
	function object:Exactly( )
		tofile( "\tMemory(" .. self.address .. ", Exactly, " .. self.count .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Never
----------------------------------------------------------------------------------------------------
function Never( )
	tofile( "\tNever();\n" )
end

----------------------------------------------------------------------------------------------------
-- Opponents Remaining
----------------------------------------------------------------------------------------------------
function Opponents( )
	local object = {}
	
	function object:SetCount( count )
		self.count = tostring( count )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:AtLeast( )
		tofile( "\tOpponents(" .. self.player .. ", At least, " .. self.count .. ");\n" )
	end
	
	function object:AtMost( )
		tofile( "\tOpponents(" .. self.player .. ", At Most, " .. self.count .. ");\n" )
	end
	
	function object:Exactly( )
		tofile( "\tOpponents(" .. self.player .. ", Exactly, " .. self.count .. ");\n" )
	end
	
	return object
end

----------------------------------------------------------------------------------------------------
-- Switch
----------------------------------------------------------------------------------------------------
function Switch( )
	local object = {}
	
	function object:SetNumber( number )
		if number < 0 or number > 255 then
			print( "\nERROR: Switch:SetNumber( ) must take a number in the range of 0 to 255." )
			assert( false )
		end
		
		self.switch = __WrapQuotes( "Switch" .. tostring( number ) )
	end
	
	function object:IsSet( )
		tofile( "\tSwitch(" .. self.switch .. ", set);\n" )
	end
	
	function object:IsNotSet( )
		tofile( "\tSwitch(" .. self.switch .. ", not set);\n" )
	end
	
	-- @HACK placed some actions in here
	function object:Clear( )
		tofile( "\tSet Switch(" .. self.switch .. ", clear);\n" )
	end
	
	function object:Randomize( )
		tofile( "\tSet Switch(" .. self.switch .. ", randomize);\n" )
	end
	
	function object:Set( )
		tofile( "\tSet Switch(" .. self.switch .. ", set);\n" )
	end
	
	function object:Toggle( )
		tofile( "\tSet Switch(" .. self.switch .. ", toggle);\n" )
	end
	
	return object
end
