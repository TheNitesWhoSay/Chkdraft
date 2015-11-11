----------------------------------------------------------------------------------------------------
-- @file	Lock.lua
-- @author	Randy Gaul
-- @date	11/8/2015
----------------------------------------------------------------------------------------------------

UNIT_IDS = {}
BACKUP_IDS = UNIT_IDS
LOCK_ID = 1
SWITCH_ID = 1

function RECORD( t, unit )
	t[ unit ] = {}
	r = t[ unit ]
	
	local i = 1
	while i < 12 do
		r[ i ] = false
		i = i + 1
	end
end

RECORD( UNIT_IDS, "Alan Schezar (Goliath)" )
RECORD( UNIT_IDS, "Alan Turret" )
RECORD( UNIT_IDS, "Aldaris (Templar)" )
RECORD( UNIT_IDS, "Alexei Stukov (Ghost)" )
RECORD( UNIT_IDS, "Arcturus Mengsk (Battlecruiser)" )
RECORD( UNIT_IDS, "Artanis (Scout)" )
RECORD( UNIT_IDS, "Bengalaas (Jungle)" )
RECORD( UNIT_IDS, "Cantina" )
RECORD( UNIT_IDS, "Cave" )
RECORD( UNIT_IDS, "Cave-in" )
RECORD( UNIT_IDS, "Cocoon" )
RECORD( UNIT_IDS, "Danimoth (Arbiter)" )
RECORD( UNIT_IDS, "Dark Swarm" )
RECORD( UNIT_IDS, "Dark Templar (Hero)" )
RECORD( UNIT_IDS, "Data Disc" )
RECORD( UNIT_IDS, "Devouring One (Zergling)" )
RECORD( UNIT_IDS, "Disruption Field" )
RECORD( UNIT_IDS, "Duke Turret type 2" )
RECORD( UNIT_IDS, "Duke Turret type 1" )
RECORD( UNIT_IDS, "Edmund Duke (Siege Mode)" )
RECORD( UNIT_IDS, "Edmund Duke (Siege Tank)" )
RECORD( UNIT_IDS, "Fenix (Dragoon)" )
RECORD( UNIT_IDS, "Fenix (Zealot)" )
RECORD( UNIT_IDS, "Flag" )
RECORD( UNIT_IDS, "Floor Gun Trap" )
RECORD( UNIT_IDS, "Floor Hatch (UNUSED)" )
RECORD( UNIT_IDS, "Floor Missile Trap" )
RECORD( UNIT_IDS, "Gantrithor (Carrier)" )
RECORD( UNIT_IDS, "Gerard DuGalle (Ghost)" )
RECORD( UNIT_IDS, "Goliath Turret" )
RECORD( UNIT_IDS, "Gui Montag (Firebat)" )
RECORD( UNIT_IDS, "Hunter Killer (Hydralisk)" )
RECORD( UNIT_IDS, "Hyperion (Battlecruiser)" )
RECORD( UNIT_IDS, "Independent Command Center" )
RECORD( UNIT_IDS, "Independent Starport" )
RECORD( UNIT_IDS, "Infested Command Center" )
RECORD( UNIT_IDS, "Infested Duran" )
RECORD( UNIT_IDS, "Infested Kerrigan (Infested Terran)" )
RECORD( UNIT_IDS, "Infested Terran" )
RECORD( UNIT_IDS, "Ion Cannon" )
RECORD( UNIT_IDS, "Jim Raynor (Marine)" )
RECORD( UNIT_IDS, "Jim Raynor (Vulture)" )
RECORD( UNIT_IDS, "Jump Gate" )
RECORD( UNIT_IDS, "Kakaru (Twilight)" )
RECORD( UNIT_IDS, "Khalis Crystal" )
RECORD( UNIT_IDS, "Khaydarin Crystal" )
RECORD( UNIT_IDS, "Khaydarin Crystal Formation" )
RECORD( UNIT_IDS, "Kukulza (Guardian)" )
RECORD( UNIT_IDS, "Kukulza (Mutalisk)" )
RECORD( UNIT_IDS, "Kyadarin Crystal Formation" )
RECORD( UNIT_IDS, "Left Pit Door" )
RECORD( UNIT_IDS, "Left Upper Level Door" )
RECORD( UNIT_IDS, "Left Wall Flame Trap" )
RECORD( UNIT_IDS, "Left Wall Missile Trap" )
RECORD( UNIT_IDS, "Magellan (Science Vessel)" )
RECORD( UNIT_IDS, "Map Revealer" )
RECORD( UNIT_IDS, "Matriarch (Queen)" )
RECORD( UNIT_IDS, "Mature Crysalis" )
RECORD( UNIT_IDS, "Mineral Chunk (Type 1)" )
RECORD( UNIT_IDS, "Mineral Chunk (Type 2)" )
RECORD( UNIT_IDS, "Mineral Field (Type 1)" )
RECORD( UNIT_IDS, "Mineral Field (Type 2)" )
RECORD( UNIT_IDS, "Mineral Field (Type 3)" )
RECORD( UNIT_IDS, "Mining Platform" )
RECORD( UNIT_IDS, "Mojo (Scout)" )
RECORD( UNIT_IDS, "Norad II (Battlecruiser)" )
RECORD( UNIT_IDS, "Norad II (Crashed Battlecruiser)" )
RECORD( UNIT_IDS, "Nuclear Missile" )
RECORD( UNIT_IDS, "Overmind Cocoon" )
RECORD( UNIT_IDS, "Power Generator" )
RECORD( UNIT_IDS, "Protoss Arbiter" )
RECORD( UNIT_IDS, "Protoss Arbiter Tribunal" )
RECORD( UNIT_IDS, "Protoss Archon" )
RECORD( UNIT_IDS, "Protoss Assimilator" )
RECORD( UNIT_IDS, "Protoss Beacon" )
RECORD( UNIT_IDS, "Protoss Carrier" )
RECORD( UNIT_IDS, "Protoss Citadel of Adun" )
RECORD( UNIT_IDS, "Protoss Corsair" )
RECORD( UNIT_IDS, "Protoss Cybernetics Core" )
RECORD( UNIT_IDS, "Protoss Dark Archon" )
RECORD( UNIT_IDS, "Protoss Dark Templar" )
RECORD( UNIT_IDS, "Protoss Dragoon" )
RECORD( UNIT_IDS, "Protoss Flag Beacon" )
RECORD( UNIT_IDS, "Protoss Fleet Beacon" )
RECORD( UNIT_IDS, "Protoss Forge" )
RECORD( UNIT_IDS, "Protoss Gateway" )
RECORD( UNIT_IDS, "Protoss High Templar" )
RECORD( UNIT_IDS, "Protoss Interceptor" )
RECORD( UNIT_IDS, "Protoss Marker" )
RECORD( UNIT_IDS, "Protoss Nexus" )
RECORD( UNIT_IDS, "Protoss Observatory" )
RECORD( UNIT_IDS, "Protoss Observer" )
RECORD( UNIT_IDS, "Protoss Photon Cannon" )
RECORD( UNIT_IDS, "Protoss Probe" )
RECORD( UNIT_IDS, "Protoss Pylon" )
RECORD( UNIT_IDS, "Protoss Reaver" )
RECORD( UNIT_IDS, "Protoss Robotics Facility" )
RECORD( UNIT_IDS, "Protoss Robotics Support Bay" )
RECORD( UNIT_IDS, "Protoss Scarab" )
RECORD( UNIT_IDS, "Protoss Scout" )
RECORD( UNIT_IDS, "Protoss Shield Battery" )
RECORD( UNIT_IDS, "Protoss Shuttle" )
RECORD( UNIT_IDS, "Protoss Stargate" )
RECORD( UNIT_IDS, "Protoss Templar Archives" )
RECORD( UNIT_IDS, "Protoss Temple" )
RECORD( UNIT_IDS, "Protoss Unused type 2" )
RECORD( UNIT_IDS, "Protoss Unused type 1" )
RECORD( UNIT_IDS, "Protoss Zealot" )
RECORD( UNIT_IDS, "Psi Disrupter" )
RECORD( UNIT_IDS, "Psi Emitter" )
RECORD( UNIT_IDS, "Ragnasaur (Ash World)" )
RECORD( UNIT_IDS, "Raszagal (Dark Templar)" )
RECORD( UNIT_IDS, "Rhynadon (Badlands)" )
RECORD( UNIT_IDS, "Right Pit Door" )
RECORD( UNIT_IDS, "Right Upper Level Door" )
RECORD( UNIT_IDS, "Right Wall Flame Trap" )
RECORD( UNIT_IDS, "Right Wall Missile Trap" )
RECORD( UNIT_IDS, "Ruins" )
RECORD( UNIT_IDS, "Samir Duran (Ghost)" )
RECORD( UNIT_IDS, "Sarah Kerrigan (Ghost)" )
RECORD( UNIT_IDS, "Scanner Sweep" )
RECORD( UNIT_IDS, "Scantid (Desert)" )
RECORD( UNIT_IDS, "Start Location" )
RECORD( UNIT_IDS, "Stasis Cell/Prison" )
RECORD( UNIT_IDS, "Tank Turret type 2" )
RECORD( UNIT_IDS, "Tassadar (Templar)" )
RECORD( UNIT_IDS, "Tassadar/Zeratul (Archon)" )
RECORD( UNIT_IDS, "Terran Academy" )
RECORD( UNIT_IDS, "Terran Armory" )
RECORD( UNIT_IDS, "Terran Barracks" )
RECORD( UNIT_IDS, "Terran Battlecruiser" )
RECORD( UNIT_IDS, "Terran Beacon" )
RECORD( UNIT_IDS, "Terran Bunker" )
RECORD( UNIT_IDS, "Terran Civilian" )
RECORD( UNIT_IDS, "Terran Command Center" )
RECORD( UNIT_IDS, "Terran Comsat Station" )
RECORD( UNIT_IDS, "Terran Control Tower" )
RECORD( UNIT_IDS, "Terran Covert Ops" )
RECORD( UNIT_IDS, "Terran Dropship" )
RECORD( UNIT_IDS, "Terran Engineering Bay" )
RECORD( UNIT_IDS, "Terran Factory" )
RECORD( UNIT_IDS, "Terran Firebat" )
RECORD( UNIT_IDS, "Terran Flag Beacon" )
RECORD( UNIT_IDS, "Terran Ghost" )
RECORD( UNIT_IDS, "Terran Goliath" )
RECORD( UNIT_IDS, "Terran Machine Shop" )
RECORD( UNIT_IDS, "Terran Marine" )
RECORD( UNIT_IDS, "Terran Marker" )
RECORD( UNIT_IDS, "Terran Medic" )
RECORD( UNIT_IDS, "Terran Missile Turret" )
RECORD( UNIT_IDS, "Terran Nuclear Silo" )
RECORD( UNIT_IDS, "Terran Physics Lab" )
RECORD( UNIT_IDS, "Terran Refinery" )
RECORD( UNIT_IDS, "Terran Science Facility" )
RECORD( UNIT_IDS, "Terran Science Vessel" )
RECORD( UNIT_IDS, "Terran SCV" )
RECORD( UNIT_IDS, "Terran Siege Tank (Siege Mode)" )
RECORD( UNIT_IDS, "Terran Siege Tank (Tank Mode)" )
RECORD( UNIT_IDS, "Terran Starport" )
RECORD( UNIT_IDS, "Terran Supply Depot" )
RECORD( UNIT_IDS, "Terran Valkyrie" )
RECORD( UNIT_IDS, "Terran Vulture" )
RECORD( UNIT_IDS, "Terran Wraith" )
RECORD( UNIT_IDS, "Tom Kazansky (Wraith)" )
RECORD( UNIT_IDS, "Torrasque (Ultralisk)" )
RECORD( UNIT_IDS, "Unclean One (Defiler)" )
RECORD( UNIT_IDS, "Unused Terran Bldg type 2" )
RECORD( UNIT_IDS, "Unused Zerg Bldg" )
RECORD( UNIT_IDS, "Unused Zerg Bldg 5" )
RECORD( UNIT_IDS, "Uraj Crystal" )
RECORD( UNIT_IDS, "Ursadon (Ice World)" )
RECORD( UNIT_IDS, "Vespene Geyser" )
RECORD( UNIT_IDS, "Vespene Orb (Protoss Type 1)" )
RECORD( UNIT_IDS, "Vespene Orb (Protoss Type 2)" )
RECORD( UNIT_IDS, "Vespene Sac (Zerg Type 1)" )
RECORD( UNIT_IDS, "Vespene Sac (Zerg Type 2)" )
RECORD( UNIT_IDS, "Vespene Tank (Terran Type 1)" )
RECORD( UNIT_IDS, "Vespene Tank (Terran Type 2)" )
RECORD( UNIT_IDS, "Vulture Spider Mine" )
RECORD( UNIT_IDS, "Warbringer (Reaver)" )
RECORD( UNIT_IDS, "Warp Gate" )
RECORD( UNIT_IDS, "Xel'Naga Temple" )
RECORD( UNIT_IDS, "Yggdrasill (Overlord)" )
RECORD( UNIT_IDS, "Young Chrysalis" )
RECORD( UNIT_IDS, "Zeratul (Dark Templar)" )
RECORD( UNIT_IDS, "Zerg Beacon" )
RECORD( UNIT_IDS, "Zerg Broodling" )
RECORD( UNIT_IDS, "Zerg Cerebrate" )
RECORD( UNIT_IDS, "Zerg Cerebrate Daggoth" )
RECORD( UNIT_IDS, "Zerg Creep Colony" )
RECORD( UNIT_IDS, "Zerg Defiler" )
RECORD( UNIT_IDS, "Zerg Defiler Mound" )
RECORD( UNIT_IDS, "Zerg Devourer" )
RECORD( UNIT_IDS, "Zerg Drone" )
RECORD( UNIT_IDS, "Zerg Egg" )
RECORD( UNIT_IDS, "Zerg Evolution Chamber" )
RECORD( UNIT_IDS, "Zerg Extractor" )
RECORD( UNIT_IDS, "Zerg Flag Beacon" )
RECORD( UNIT_IDS, "Zerg Greater Spire" )
RECORD( UNIT_IDS, "Zerg Guardian" )
RECORD( UNIT_IDS, "Zerg Hatchery" )
RECORD( UNIT_IDS, "Zerg Hive" )
RECORD( UNIT_IDS, "Zerg Hydralisk" )
RECORD( UNIT_IDS, "Zerg Hydralisk Den" )
RECORD( UNIT_IDS, "Zerg Lair" )
RECORD( UNIT_IDS, "Zerg Larva" )
RECORD( UNIT_IDS, "Zerg Lurker" )
RECORD( UNIT_IDS, "Zerg Lurker Egg" )
RECORD( UNIT_IDS, "Zerg Marker" )
RECORD( UNIT_IDS, "Zerg Mutalisk" )
RECORD( UNIT_IDS, "Zerg Nydus Canal" )
RECORD( UNIT_IDS, "Zerg Overlord" )
RECORD( UNIT_IDS, "Zerg Overmind" )
RECORD( UNIT_IDS, "Zerg Overmind (With Shell)" )
RECORD( UNIT_IDS, "Zerg Queen" )
RECORD( UNIT_IDS, "Zerg Queen's Nest" )
RECORD( UNIT_IDS, "Zerg Scourge" )
RECORD( UNIT_IDS, "Zerg Spawning Pool" )
RECORD( UNIT_IDS, "Zerg Spire" )
RECORD( UNIT_IDS, "Zerg Spore Colony" )
RECORD( UNIT_IDS, "Zerg Sunken Colony" )
RECORD( UNIT_IDS, "Zerg Ultralisk" )
RECORD( UNIT_IDS, "Zerg Ultralisk Cavern" )
RECORD( UNIT_IDS, "Zerg Zergling" )
RECORD( UNIT_IDS, "Any unit" )
RECORD( UNIT_IDS, "Buildings" )
RECORD( UNIT_IDS, "Factories" )
RECORD( UNIT_IDS, "Men" )

SWITCH_IDS = {}

local i = 1
while i < 256 do
	SWITCH_IDS[ i ] = false
	i = i + 1
end

function LockID( )
	local id = LOCK_ID
	LOCK_ID = LOCK_ID + 1
	return id
end

function SwitchID( )
	local id = SWITCH_ID
	SWITCH_ID = SWITCH_ID + 1
	return id
end

function __LockSwitch( index, id )
	local s = SWITCH_IDS[ index ]
	local okay = true
	
	if s then
		if id then
			okay = s == id
		else
			okay = false
		end
	end
	
	if not okay then
		__Error( "Attempted to use the switch " .. tostring( index ) .. " while previously locked." )
	end
end

function __AllocSwitch( num, id )
	local s = Switch( )
	s:SetNumber( num )
	s.id = id
	SWITCH_IDS[ num ] = id
	return s
end

function AllocSwitch( num )
	if not num then
		for k, v in pairs( SWITCH_IDS ) do
			if not v then
				return __AllocSwitch( k, LockID( ) )
			end
		end
	else
		local id = LockID( )
		__LockSwitch( num, id )
		return __AllocSwitch( num, id )
	end

	__Error( "Attempted to alloc a switch when there are no free switches available." )
end

function FreeSwitch( s )
	if type( s ) == "table" then
		__LockSwitch( s.number, s.id )
		SWITCH_IDS[ s.number ] = false
	else
		if SWITCH_IDS[ s ] then
			SWITCH_IDS[ s ] = false
		else
			__Warn( "Tried to free switch " .. tostring( s ) .. " when it wasn't locked." )
		end
	end
end

function __LockDC( name, player, id )
	local t = UNIT_IDS[ name ]
	local tid = t[ player ]
	local okay = true
	
	if tid then
		if id then
			okay = t[ player ] == id
		else
			okay = false
		end
	end
	
	if not okay then
		__Error( "Attempted to use the dc " .. name .. " for player " .. tostring( player ) .. " while previously locked." )
	end
end

function __AllocDC( p, u, id, t )
	local dc = Deaths( )
	dc:SetPlayer( p )
	dc:SetUnit( u )
	dc.id = id
	t[ p ] = id
	return dc
end

function AllocDC( player, unit )
	local id = LockID( )
		
	if not unit then
		for k, v in pairs( UNIT_IDS ) do
			if not v[ player ] then
				return __AllocDC( player, k, id, v )
			end
		end
	else
		__LockDC( unit, player, id )
		return __AllocDC( player, unit, id, UNIT_IDS[ unit ] )
	end

	__Error( "Attempted to alloc a DC for player " .. player .. " when there are no free DCs available." )
end

function FreeDC( dc )
	local u = dc.unit:gsub( "\"", "" )
	local t = UNIT_IDS[ u ]
	local s = dc.player:gsub( "Player ", "" )
	s = s:gsub( "\"", "" )
	local p = tonumber( s )
	assert( t[ p ] == dc.id )
	local locked = t[ p ]
	if not locked then __Warn( "Tried to free the dc " .. dc.unit .. " for player " .. tostring( p ) .. " though it isn't locked." ) end
	--print( "Freed DC: Player " .. s .. ", id " .. tostring( t[ p ] ) .. ", " .. u )
	t[ p ] = false
end
