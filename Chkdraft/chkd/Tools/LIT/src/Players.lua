----------------------------------------------------------------------------------------------------
-- @file	Players.lua
-- @author	Randy Gaul
-- @date	08/14/2014
----------------------------------------------------------------------------------------------------

PLAYER_STATE = true
TRIGGER_COUNT = 0

function Conditions( playerGroup )
	if PLAYER_STATE == false then
		PLAYER_STATE = true
		tofile( "}\n\n" )
		tofile( "//-----------------------------------------------------------------//\n\n" )
	end

	tofile( "Trigger(" );
  
	local i = false
	
	for k, v in ipairs( playerGroup.players ) do
		if i == false then
			i = true
		else
			tofile( "," )
		end
    
		tofile( "\"Player " .. tostring( v ) .. "\"" )
	end
	
	tofile( "){\n" )
	tofile( "Conditions:\n" )
	
	TRIGGER_COUNT = TRIGGER_COUNT + 1
end

function Actions( playerGroup )
	tofile( "\n" )
	tofile( "Actions:\n" )
  
	PLAYER_STATE = false
end

function PlayerGroup( ... )
	local playerGroup = {}
	playerGroup.players = {}
  
	for i, v in ipairs( { ... } ) do
		playerGroup.players[ i ] = v
	end
  
	playerGroup.Conditions = Conditions
	playerGroup.Actions = Actions
  
	return playerGroup
end

local i = 1
while i < 255 do
	_G[ "Player" .. tostring( i ) ] = i
	i = i + 1
end
