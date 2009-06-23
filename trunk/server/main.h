// File Author: kyeman

#pragma once

#ifndef _MAIN_H_
#define _MAIN_H_

#ifndef GAME_REVISION
#define GAME_REVISION "3"
#endif

#ifndef NETGAME_VERSION
#define NETGAME_VERSION 3
#endif

#include <string.h>
#include <math.h>

#if defined( WIN32 ) || defined( _WIN64 ) && !defined( IS_WINDOWS )
#define IS_WINDOWS
#endif

#ifdef IS_WINDOWS

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

#define MAX_PLAYER_NAME		24
#define MAX_PLAYERS			50
#define MAX_VEHICLES		200

#include "../Console/Console.h"

#include "../raknet/RakServer.h"
#include "../raknet/RakNetworkFactory.h"
#include "../raknet/BitStream.h"

#include "system.h"
#include "config.h"
#include "CarCols.h"
#include "../tinyxml/tinyxml.h"

#define DEFAULT_VEHICLES_FILE "vehicles.xml"
#define DEFAULT_CONFIG_FILE "server.conf"
#define DEFAULT_MAX_PLAYERS 32
#define DEFAULT_LISTEN_PORT 8192
#define DEFAULT_RCON_PORT	8190
#define DEFAULT_RCON_MAXUSERS	8

void logprintf(char * format, ...);

#endif