----------------------------------------------------------------------------------------------------
-- @file	DeathCounter.lua
-- @author	Randy Gaul
-- @date	08/11/2014
----------------------------------------------------------------------------------------------------

function Deaths( )
	local object = {}
	
	function object:SetCount( count )
		self.count = __Count( count )
	end
	
	function object:SetPlayer( player )
		self.player = __Player( player )
	end
	
	function object:SetUnit( unit )
		self.unitNoQuote = unit
		self.unit = __Unit( unit )
	end
	
	function object:AtLeast( count )
		if ( count ~= nil ) then self:SetCount( count ) end
		__LockDC( self.unitNoQuote )
		tofile( "\tDeaths(" .. self.player .. ", " .. self.unit ..  ", At Least, " .. self.count .. ");\n" )
	end
	
	function object:AtMost( count )
		if ( count ~= nil ) then self:SetCount( count ) end
		__LockDC( self.unitNoQuote )
		tofile( "\tDeaths(" .. self.player .. ", " .. self.unit ..  ", At Most, " .. self.count .. ");\n" )
	end
	
	function object:Exactly( count )
		if ( count ~= nil ) then self:SetCount( count ) end
		__LockDC( self.unitNoQuote )
		tofile( "\tDeaths(" .. self.player .. ", " .. self.unit ..  ", Exactly, " .. self.count .. ");\n" )
	end
	
	function object:Add( count )
		if ( count ~= nil ) then self:SetCount( count ) end
		__LockDC( self.unitNoQuote )
		tofile( "\tSet Deaths(" .. self.player .. ", " .. self.unit ..  ", Add, " .. self.count .. ");\n" )
	end
	
	function object:SetTo( count )
		if ( count ~= nil ) then self:SetCount( count ) end
		__LockDC( self.unitNoQuote )
		tofile( "\tSet Deaths(" .. self.player .. ", " .. self.unit ..  ", Set To, " .. self.count .. ");\n" )
	end
	
	function object:Subtract( count )
		if ( count ~= nil ) then self:SetCount( count ) end
		__LockDC( self.unitNoQuote )
		tofile( "\tSet Deaths(" .. self.player .. ", " .. self.unit ..  ", Subtract, " .. self.count .. ");\n" )
	end
	
	return object
end
