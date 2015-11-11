local s
local n = 0
local nMax = 0
local stack = {}
function Push( a )
	table.insert( stack, a )
end
function Pop( )
	table.remove( stack )
end

function Tabs( )
	local s = ""
	local i = 1
	while i < n do
		s = s .. "\t"
		i = i + 1
	end
	return s
end

function Which( line )
	if string.find( line, "^%s*#if" ) then
		return "if"
	elseif string.find( line, "^%s*#then" ) then
		return "then"
	elseif string.find( line, "^%s*#end" ) then
		return "end"
	else
		return nil
	end
end

function Preprocess( filename )
	local file, err = io.open( USER_PATH .. filename, "r" )
	if err then print( err ) end
	local chunk = {}
	local cleanup = {}
	local was_if = false
	local p
	for line in file:lines( ) do
		local w = Which( line )
		
		if not w then
			table.insert( chunk, line .. "\n" )
			
		elseif w == "if" then
			n = n + 1
			if was_if then __Error( "Cannot place an #if right after another #if." ) end
			local s0 = s
			s = "__s" .. tostring( n )
			p = line:gsub( "^%s*#if%s*", "" )
			table.insert( chunk, Tabs( ) .. s .. " = AllocSwitch( )\n" )
			table.insert( chunk, Tabs( ) .. p .. ":Conditions( )\n" )
			if n - 1 ~= 0 then table.insert( chunk, Tabs( ) .. s0 .. ":IsSet( )\n" ) end
			was_if = true
			Push( s )
		
		elseif w == "then" then
			table.insert( chunk, Tabs( ) .. p .. ":Actions( )\n" )
			table.insert( chunk, Tabs( ) .. s .. ":Set( )\n" )
			was_if = false
		
		elseif w == "end" then
			was_if = false
			if n ~= 0 then
				Pop( )
				table.insert( cleanup, s .. ":Clear( )\n" )
				table.insert( chunk, Tabs( ) .. s .. ":Clear( )\n" )
				table.insert( chunk, Tabs( ) .. "FreeSwitch( " .. s .. " )\n" )
				s = stack[ #stack ] or s
			end
			n = n - 1
		end
		
		if nMax > n then nMax = n end
	end
	
	if nMax > 63 then __Error( "Nested " .. nMax .. " #if statements together (63 nested #if)" ) end
	
	if p then
		table.insert( chunk, p .. ":Conditions( )\nAlways( )\n" .. p .. ":Actions( )\n" )
		for k, v in pairs( cleanup ) do
			cleanup[ k ] = nil
			table.insert( chunk, v )
		end
		table.insert( chunk, "Preserve( )\n" )
	end
	
	chunk = table.concat( chunk )
	--print( chunk )
	chunk, err = load( chunk  )
	if err then print( "Error in: " .. filename ); print( err ) end
	return chunk( )
end

function include( filename )
	return Preprocess( filename )
end

print( "LIT is running in " .. lfs.currentdir( ) )
print( "Running LIT on file " .. USER_PATH .. __Main .. "\n" )
Preprocess( __Main )
