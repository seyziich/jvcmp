// File Author: kyeman

#ifdef WIN32
#include <stdio.h>
#include <windows.h>
#else
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#endif

#define  _us2ms(x) (x*1000)

#include "main.h"
#include "netgame.h"
#include "rcon.h"

#include "../raknet/BitStream.h"
#include "../raknet/RakPeerInterface.h"
#include "../raknet/RakNetworkFactory.h"

void fatal_exit(char * szError);

CNetGame	*pNetGame;
CRcon		*pRcon;
CConfig		*pServerConfig;
int			iLogState=1;
char		*szAdminPass;
BOOL		bDisableParkedCars=FALSE;
BOOL		bDebugMessages=FALSE;
CCarColManager *pCarColManager;

int main (int argc, char* argv[])
{
	SetConsoleTitle( TEXT("JVCMP Multiplayer Server") );
	pCarColManager = new CCarColManager();

	int iMaxPlayers=0;
	int iListenPort=0;
	char *szPass=NULL;
	int iCurrentGame=1;
	char szConfigFile[512];
	char szError[256];

	int iFriendlyFireOption;
	BYTE byteFriendlyFire=0;
	int iShowOnRadarOption;
	BYTE byteShowOnRadarOption=1;

	int iRconPort = 0;
	int iRconMaxUsers = 0;

	pServerConfig = new CConfig();

	if(argc > 1) {
		strcpy(szConfigFile,argv[1]);
	} else {
		strcpy(szConfigFile,DEFAULT_CONFIG_FILE);
	}
	if(strlen(szConfigFile) > 32)
	{
		fatal_exit("The config file is longer than 32 characters!");
	}

	// try and read the config file.
	if(pServerConfig->ReadFile(szConfigFile) != CCONF_ERR_SUCCESS) {
		logprintf("Config file %s error!\nCConfig::Error = %s",szConfigFile,pServerConfig->m_szErrorString);
		fatal_exit("");
	}

	logprintf("Loading config file %s...",szConfigFile);

	// get the max players setting
	if((iMaxPlayers=pServerConfig->GetConfigEntryAsInt("MaxPlayers"))==(-1)) {
		iMaxPlayers = DEFAULT_MAX_PLAYERS;
	}

	if(iMaxPlayers > 32) { iMaxPlayers = 32; };

	// get the listen port setting
	if((iListenPort=pServerConfig->GetConfigEntryAsInt("ListenPort"))==(-1)) {
		iListenPort = DEFAULT_LISTEN_PORT;
	}

	// set the server pass if one is required.
	if(pServerConfig->GetConfigEntryAsBool("NeedPassword")==1) {
		szPass=pServerConfig->GetConfigEntryAsString("Password");
	}
	if(pServerConfig->GetConfigEntryAsBool("DisableParkedCars")==1) {
		bDisableParkedCars = TRUE;
	}
	if(pServerConfig->GetConfigEntryAsBool("EnableDebugMessages")==1) {
		bDebugMessages = TRUE;
	}

	// get the admin pass
	szAdminPass = pServerConfig->GetConfigEntryAsString("AdminPassword");
	if(!szAdminPass || !strlen(szAdminPass)) {
		fatal_exit("The 'AdminPassword' in the config file must be set.\n");
	}
	if(!strcmp(szAdminPass,"invalid")) {
		fatal_exit("The 'AdminPassword' in the config file must not be 'invalid'.\n");
	}
	if(strlen(szAdminPass) > 32) {
		fatal_exit("The 'AdminPassword' must not be longer than 32 characters.\n");
	}
	if(!pServerConfig->LoadVehicleColours("carcols.conf"))
	{
		logprintf("The car colours file %s could not be loaded!","carcols.conf");
		fatal_exit("");
	}

	// get the ff option from config (default is OFF)
	iFriendlyFireOption = pServerConfig->GetConfigEntryAsBool("FriendlyFire");
	if(iFriendlyFireOption == 1) byteFriendlyFire = 1;

	// get the show on radar option (default is ON)
	iShowOnRadarOption = pServerConfig->GetConfigEntryAsBool("ShowOnRadar");
	if(iShowOnRadarOption == -1 || iShowOnRadarOption == 0)	{
		byteShowOnRadarOption = 0;
	}
	
	// create the NetGame.
	pNetGame = new CNetGame(iMaxPlayers,iListenPort,0,szPass,0,byteFriendlyFire,byteShowOnRadarOption);

	// Get the remote console port.
	if (iRconPort = pServerConfig->GetConfigEntryAsInt("RconPort") == -1) {
		iRconPort = DEFAULT_RCON_PORT;
	} else {
		iRconPort = pServerConfig->GetConfigEntryAsInt("RconPort");
	}

	// Get the remote console max users.
	if (iRconMaxUsers = pServerConfig->GetConfigEntryAsInt("RconMaxUsers") <= 0) {
		iRconMaxUsers = DEFAULT_RCON_MAXUSERS;
	} else {
		iRconMaxUsers = pServerConfig->GetConfigEntryAsInt("RconMaxUsers");
	}

	// create rcon
	pRcon = new CRcon(iRconPort, szAdminPass, iRconMaxUsers);

	Sleep(1000);
	system("cls");
	logprintf("Jack's Vice City Multiplayer server started on port %d.\nMax Players: %d.\n",iListenPort,iMaxPlayers);

	// Process the network game.
	while(pNetGame->GetGameState() == GAMESTATE_RUNNING)
	{
		pNetGame->Process();
		pRcon->Process();

#ifdef WIN32
		Sleep(5);
#else
		usleep(_us2ms(5));
#endif

	}
	system("cls");
	logprintf("Cleanup...\n");
	delete pRcon;
	delete pNetGame;
	return 0;
}

//----------------------------------------------------

void fatal_exit(char * szError)
{
	#ifdef WIN32
		CConsole::PrintTime("%s\n\nPress ENTER to close.",COLOUR_RED,25,szError);
		getc(stdin);
	#endif
		exit(1);
}

void logprintf(char * format, ...)
{
	char tmp_buf[512];
	va_list args;
	va_start(args, format);
	vsprintf(tmp_buf, format, args);
	va_end(args);
	//puts(tmp_buf);
	CConsole::PrintTime(tmp_buf,COLOUR_RED,25);
	/*
	char tmp_buf[512];
	if(iLogState) {
		va_list args;
		va_start(args, format);
		vsprintf(tmp_buf, format, args);
		va_end(args);
		puts(tmp_buf);
	}*/
}

void FilterInvalidNickChars(PCHAR szString)
{
	while(*szString) {
		if(*szString < ' ' || *szString > 'z') {
			*szString = '_';
		}
		szString++;
	}
}
