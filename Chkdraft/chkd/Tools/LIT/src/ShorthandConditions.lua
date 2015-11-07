----------------------------------------------------------------------------------------------------
-- @file	ShorthandConditions.lua
-- @author	Randy Gaul
-- @date	08/14/2014
----------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------
-- Bring
----------------------------------------------------------------------------------------------------
function BringAtLeast( player, unit, location, count )
	tofile( "\tBring(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ", " .. __Location( location ) .. ", At Least, " .. __Count( count ) .. ");\n" )
end

function BringAtMost( player, unit, location, count )
	tofile( "\tBring(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ", " .. __Location( location ) .. ", At Most, " .. __Count( count ) .. ");\n" )
end

function BringExactly( player, unit, location, count )
	tofile( "\tBring(" .. __Player( player ) .. ", " .. __Unit( unit ) .. ", " .. __Location( location ) .. ", Exactly, " .. __Count( count ) .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Accumulate
----------------------------------------------------------------------------------------------------
function AccumulateAtLeast( player, count, resource )
	tofile( "\tAccumulate(" .. __Player( player ) .. ", At least, " .. __Count( count ) .. ", " .. resource .. ");\n" )
end

function AccumulateAtMost( player, count, resource )
	tofile( "\tAccumulate(" .. __Player( player ) .. ", At most, " .. __Count( count ) .. ", " .. resource .. ");\n" )
end

function AccumulateExactly( player, count, resource )
	tofile( "\tAccumulate(" .. __Player( player ) .. ", Exactly, " .. __Count( count ) .. ", " .. resource .. ");\n" )
end

----------------------------------------------------------------------------------------------------
-- Deaths (shorthand version, see DeathCounter.lua for the verbose version)
----------------------------------------------------------------------------------------------------
function DC( player, unit, qmod, count )
	tofile( "\tDeaths(" .. __Player( player ) .. ", " .. __Unit( unit ) ..  ", " .. qmod .. ", " .. __Count( count ) .. ");\n" )
end

function DCAtLeast( player, unit, count )
	DC( player, unit, "At Least", count )
end

function DCAtMost( player, unit, count )
	DC( player, unit, "At Most", count )
end

function DCExactly( player, unit, count )
	DC( player, unit, "Exactly", count )
end

function DCAdd( player, unit, count )
	DC( player, unit, "Add", count )
end

function DCSet( player, unit, count )
	DC( player, unit, "Set To", count )
end

function DCSubtract( player, unit, count )
	DC( player, unit, "Subtract", count )
end

----------------------------------------------------------------------------------------------------
-- Switch
----------------------------------------------------------------------------------------------------
function SwitchIsSet( number )
	__LockSwitch( number )
	tofile( "\tSwitch(" .. tostring( number ) .. ", set);\n" )
end

function SwitchIsNotSet( number )
	__LockSwitch( number )
	tofile( "\tSwitch(" .. tostring( number ) .. ", not set);\n" )
end
