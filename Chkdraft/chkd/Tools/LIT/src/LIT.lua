----------------------------------------------------------------------------------------------------
-- @file	LIT.lua
-- @author	Randy Gaul
-- @date	08/14/2014
----------------------------------------------------------------------------------------------------

function opentriggerfile( s )
  TRIGGER_FILE = io.open( s, "w" )
end

function tofile( s )
  TRIGGER_FILE:write( s )
end

function closetriggerfile( )
  tofile( "}\n\n" )
  tofile( "//-----------------------------------------------------------------//\n\n" )
  TRIGGER_FILE:close( )
  
  print( "Number of triggers generated: " .. tostring( TRIGGER_COUNT ) )
end

function __WrapQuotes( theString )
	return "\"" .. theString .. "\""
end

function __Location( location )
	return __WrapQuotes( location )
end

function __Count( count )
	return tostring( count )
end

function __Player( player )
	if ( type( player ) == "string" ) then
		return __WrapQuotes( player )
	elseif ( type( player ) == "number" ) then
		return __WrapQuotes( "Player " .. tostring( player ) )
	else
		-- No acceptable type passed to this function
		assert( false )
	end
end

function __Unit( unit )
	return __WrapQuotes( unit )
end

function __WAV( fileName )
	-- Retrieve file extension
	local ext = string.match( fileName, "([^.]+)$" )
	
	if ext ~= "wav" and ext ~= "WAV" then
		print( "\nERROR: PlayWAV:Play( ) only accepts strings for a file path to a .wav file." )
		assert( type( name ) == "string" )
	end
	
	local fileNameAndPath = "staredit/wav/" .. fileName
	fileNameAndPath = fileNameAndPath:gsub( "\\", "\\\\" )
	fileNameAndPath = fileNameAndPath:gsub( "/", "\\\\" )
	
	return __WrapQuotes( fileNameAndPath )
end

function __State( s )
	if s ~= "enabled" and s ~= "disabled" then
		print( "\nERROR: state must be \"enabled\" or \"disabled\"." )
		print( "You gave: " .. tostring( s ) )
		assert( false )
	end
	
	return s
end

function __Resource( s )
	if s ~= "ore" and s ~= "gas" and "ore and gas" then
		print( "\nERROR: resource must be \"ore\" or \"gas\" or \"ore and gas\"." )
		assert( false )
	end
	
	return s
end
