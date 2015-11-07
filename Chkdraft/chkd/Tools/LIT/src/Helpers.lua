function Hyper( playerGroup )
	local i = 0
	
	while i < 3 do
		local j = 0
		
		playerGroup:Conditions( )
		Always( )
		playerGroup:Actions( )
		while j < 63 do
			Wait( 0 )
			
			j = j + 1
		end
		Preserve( )
		
		i = i + 1
	end
end
