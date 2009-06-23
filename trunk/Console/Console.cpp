/*
	File Author: VRocker
*/

#include "Console.h"
#include <iostream>

#include <string.h>
#include <math.h>

#if defined( WIN32 ) || defined( _WIN64 )

#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <process.h>

#else

#include <dlfcn.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>

typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef char* PCHAR;
typedef long long __int64;
typedef int HANDLE;

#endif

void CConsole::Print( const char* szText, CONSOLE_COLOURS cCol, ... )
{
	char sz[ MAX_CONSOLELINE_LEN ] = {0};

	va_list marker;
	va_start( marker, cCol );
	vsnprintf( sz, sizeof( sz ), szText, marker );
	va_end( marker );

	switch ( cCol )
	{
		case COLOUR_WHITE:
			ConsoleWhite
			break;

		case COLOUR_GREY:
			ConsoleGrey
			break;

		case COLOUR_RED:
			ConsoleRed
			break;

		case COLOUR_GREEN:
			ConsoleGreen
			break;

		case COLOUR_BLUE:
			ConsoleBlue
			break;

		case COLOUR_YELLOW:
			ConsoleYellow
			break;

		case COLOUR_CYAN:
			ConsoleCyan
			break;

		case COLOUR_MAGENTA:
			ConsoleMagenta
			break;

		case COLOUR_DARKGREEN:
			ConsoleDarkGreen
			break;

		default:
			ConsoleWhite
			break;
	};

	printf( sz );
}

void CConsole::Print2( const char* szText, CONSOLE_COLOURS cCol )
{
	switch ( cCol )
	{
		case COLOUR_WHITE:
			ConsoleWhite
			break;

		case COLOUR_GREY:
			ConsoleGrey
			break;

		case COLOUR_RED:
			ConsoleRed
			break;

		case COLOUR_GREEN:
			ConsoleGreen
			break;

		case COLOUR_BLUE:
			ConsoleBlue
			break;

		case COLOUR_YELLOW:
			ConsoleYellow
			break;

		case COLOUR_CYAN:
			ConsoleCyan
			break;

		case COLOUR_MAGENTA:
			ConsoleMagenta
			break;

		case COLOUR_DARKGREEN:
			ConsoleDarkGreen
			break;

		default:
			ConsoleWhite
			break;
	};

	std::cout << szText;
}

void CConsole::PrintC( const char* szNote, CONSOLE_COLOURS cCol, const char* szText, ... )
{
	char sz[ MAX_CONSOLELINE_LEN ] = {0};
	va_list marker;
	va_start ( marker, szText );
	vsnprintf( sz, sizeof( sz ), szText, marker );
	va_end ( marker );

	strcat_s( sz, "\n" );

	Print( "%s: ", cCol, szNote );
	Print( sz, COLOUR_WHITE );
}

void CConsole::PrintC2( const char* szNote, CONSOLE_COLOURS cCol, const char* szText )
{
	char sz[ MAX_CONSOLELINE_LEN ] = {0};
	strcpy_s( sz, szText );
	strcat_s( sz, "\n" );

	Print( "%s: ", cCol, szNote );
	Print2( sz, COLOUR_WHITE );
}

void CConsole::PrintTime( const char* szText, CONSOLE_COLOURS cCol, unsigned int iTime, ... )
{
	char sz[ MAX_CONSOLELINE_LEN ] = {0};
	va_list marker;
	va_start ( marker, iTime );
	vsnprintf( sz, sizeof( sz ), szText, marker );
	va_end ( marker );

	int i = 0;
	for ( i = 0; i < (int)strlen(sz); i++ )
	{
		Print( "%c", cCol, sz[i]);
#if defined( WIN32 ) || defined( WIN64 )
		Sleep( iTime );
#else
		usleep( ( iTime * 1000 ) );
#endif
	}

	Print( "\n", COLOUR_WHITE );
}
