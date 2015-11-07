extern "C"
{
	#include "../lua/lua.h"
	#include "../lua/lua.hpp"
	#include "../lua/lauxlib.h"
	#include "../lua/luaconf.h"
	#include "../lua/lualib.h"
	#include "../lua/lfs.h"
}

lua_State* L;
int errors;

void Dofile( const char* name )
{
	if ( luaL_dofile( L, name ) )
	{
		fprintf( stderr, lua_tostring( L, -1 ) );
		fprintf( stderr, "\n" );
		++errors;
	}
}

int Include( lua_State* L )
{
	if ( !lua_isstring( L, 1 ) )
	{
		fprintf( stderr, "This function expected a string as the only argument." );
	}

	const char *name = lua_tostring( L, 1 );

	Dofile( name );

	return 0;
}

void OpenTriggerFile( const char* name )
{
	lua_getglobal( L, "opentriggerfile" );
	lua_pushstring( L, name );
	lua_call( L, 1, 0 );
}

void CloseTriggerFile( )
{
	lua_getglobal( L, "closetriggerfile" );
	lua_call( L, 0, 0 );
}

#include <string>

// on Windows copy the triggers to clipboard
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

int main( int argc, char** argv )
{
	if ( argc < 2 )
	{
		printf( "\n" );
		printf( "  EXAMPLE USAGE: ./LIT [inputFileName] [outputDirectory] [outputFileName]\n\n" );
		printf( "    [inputFileName]\n" );
		printf( "      -- Name of the file to interpret into triggers.\n\n" );
		printf( "    [outputDirectory] (default = ./)\n" );
		printf( "      -- Name of the output directory which contains the output triggers files.\n\n" );
		printf( "    [outputFileName] (default = out.txt)\n" );
		printf( "      -- Name of the output text file which contains output triggers.\n\n" );

		return 0;
	}

	L = luaL_newstate( );
	luaL_openlibs( L );
	luaopen_lfs( L );
	lua_pop( L, -1 );

	std::string inFile = argv[ 1 ];
	std::string outPath = (argc < 3) ? "./" : argv[ 2 ];
	const char *outputFileName = (argc < 4) ? "out.txt" : argv[ 3 ];
	std::string outFilePath = outPath + outputFileName;

	lua_pushcfunction( L, Include );
	lua_setglobal( L, "include" );

	Dofile( "../src/LIT.lua" );
	OpenTriggerFile( outFilePath.c_str( ) );
	Dofile( "../src/players.lua" );
	Dofile( "../src/conditions.lua" );
	Dofile( "../src/actions.lua" );
	Dofile( "../src/deathcounter.lua" );
	Dofile( "../src/ShorthandConditions.lua" );
	Dofile( "../src/ShorthandActions.lua" );
	Dofile( "../src/Lock.lua" );
	Dofile( "../src/Helpers.lua" );
	Dofile( inFile.c_str( ) );
	CloseTriggerFile( );

	if ( errors )
	{
		fprintf( stderr, "\n\n\t WARNING\n", errors );
		fprintf( stderr, "\t  LIT has detected %d errors (listed above).\n", errors );
	}

	else
	{
		fprintf( stdout, "LIT has completed successfully.\n" );

		// on Windows copy the triggers to clipboard
#ifdef WIN32
		FILE* fp = fopen( outFilePath.c_str( ), "rb" );
		fseek( fp, 0, SEEK_END );
		int size = ftell( fp );
		fseek( fp, 0, SEEK_SET );
		char* data = (char*)malloc( size + 1 );
		fread( data, size, 1, fp );
		fclose( fp );
		data[ size ] = 0;

		HGLOBAL hMem = GlobalAlloc( GMEM_MOVEABLE, size );
		memcpy( GlobalLock( hMem ), data, size );
		GlobalUnlock( hMem );
		OpenClipboard( 0 );
		EmptyClipboard( );
		SetClipboardData( CF_TEXT, hMem );
		CloseClipboard( );

		fprintf( stdout, "Triggers successfully copied to your clipboard :) (you can use ctrl + v)\n" );
#endif
	}

	return 0;
}
