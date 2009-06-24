// File Author: kyeman && JackPowell && lucasc190

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
BOOL		bPrefixTime=FALSE;
CCarColManager *pCarColManager;

bool bQuitApp = false;

HANDLE hInputThread;

static void ProcessCommand(char* szCommand)
{
	if (!strcmp(szCommand,"quit") || !strcmp(szCommand,"exit")) 
	{
		bQuitApp = true;
	}
	else
	{
		logprintf("The command you entered was not recognised!");
	}
	//logprintf(szCommand);
}

static DWORD WINAPI InputThreadMain(LPVOID args)
{
	char buf[512];
	while (true)
	{
		DWORD dwRead;
		ReadConsole(GetStdHandle(STD_INPUT_HANDLE), buf, 255, &dwRead, NULL);
		if (dwRead > 2)
		{
			buf[dwRead-2] = 0;
			//WaitForSingleObject(hInputThread, INFINITE);
			ProcessCommand(buf);
		}
	}
    return 0;
}

static void StartInputThread(void)
{
	DWORD threadId;
	LPVOID value = (LPVOID)10;
	HANDLE hThread = CreateThread( NULL, 0, InputThreadMain, value, 0, &threadId);
	hInputThread = hThread;
}

static void StopInputThread(void)
{
	TerminateThread(hInputThread, 0);
	CloseHandle(hInputThread);
}

int main (int argc, char* argv[])
{
	SetConsoleTitle( TEXT("JVCMP Multiplayer Server") );
	pCarColManager = new CCarColManager();

	int iMaxPlayers=0;
	int iListenPort=0;
	char *szPass=NULL;
	char *szGameName=NULL;
	int iCurrentGame=1;
	char szConfigFile[512];

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
		fatal_exit("The config file path is longer than 32 characters!");
	}

	// try and read the config file.
	if(pServerConfig->ReadFile(szConfigFile) != CCONF_ERR_SUCCESS) {
		logprintf("Config file %s error!\n%s",szConfigFile,pServerConfig->m_szErrorString);
		fatal_exit("");
	}

	logprintf("Loading config file: %s...",szConfigFile);
	
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
	#ifdef DEBUG
		bDebugMessages = TRUE; //Set debug messages on in debug mode
	#endif
	
	// Gamename would only be used when a server browser is made
	// As the server browser should ping the server with a byte
	// And server returns the Gamename as bytes.
	// Server browser could be made in VB.Net
	szGameName = pServerConfig->GetConfigEntryAsString("GameName");
	if(!szGameName || !strlen(szGameName)) {
		fatal_exit("The 'GameName' in the config file must be set.\n");
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

	newline();
	logprintf("Game Name: %s",szGameName);
	logprintf("Port: %d",iListenPort);
	logprintf("Max Players: %d",iMaxPlayers);
	newline();
	bPrefixTime = TRUE; //lucasc190: Start prefixing time after this point.
	logprintf("Jack's Vice City Multiplayer server has successfully started.");

	StartInputThread();
	while(pNetGame->GetGameState() == GAMESTATE_RUNNING && !bQuitApp)
	{
		pNetGame->Process();
		pRcon->Process(); //Comment this if you don't want the RCon project to work
//The sleep function is differnt in linux so check
#ifdef IS_WINDOWS
		Sleep(5);
#else
		usleep(_us2ms(5));
#endif

	}
	logprintf("Cleaning up...");//lucasc190: Fixed spelling, grammar and tense.
	StopInputThread();
	delete pRcon;
	delete pNetGame;
	logprintf("Good bye...");//lucasc190: Added this to be friendly.
	return 0;
}

//----------------------------------------------------

void fatal_exit(char * szError)
{
	//Linux does not support the system function or getc function so check if its windows
	//But you can still output the error because linux consoles can stay open if told so
	#ifdef IS_WINDOWS
		CConsole::PrintTime("%s\nPress any key to close...",COLOUR_RED,25,szError);
		system("pause>nul");
	#else
		CConsole::PrintTime(szError,COLOUR_RED,25);
	#endif
	exit(1);
}

void newline(void)
{
	puts("");
}

void logprintf(char * format, ...)
{
	char tmp_buf[512];
	va_list args;
	va_start(args, format);
	vsprintf(tmp_buf, format, args);
	va_end(args);
	char szReadyBuf[8192] = {0};
	if (bPrefixTime)
	{
		strcat(szReadyBuf,"[");
		SYSTEMTIME myTime;
		GetLocalTime(&myTime);
		char time_buf[512];
		//lucasc190: Jack, try not to look at the following code; it'll probably confuse you.
		char* szZ1;
		char* szZ2;
		char* szZ3;
		if (myTime.wHour < 10)
			szZ1 = "0";
		else
			szZ1 = "";
		if (myTime.wMinute < 10)
			szZ2 = "0";
		else
			szZ2 = "";
		if (myTime.wSecond < 10)
			szZ3 = "0";
		else
			szZ3 = "";
		sprintf(time_buf,"%s%d:%s%d:%s%d",szZ1,myTime.wHour,szZ2,myTime.wMinute,szZ3,myTime.wSecond);
		strcat(szReadyBuf,time_buf);
		strcat(szReadyBuf,"] ");
	}
	strcat(szReadyBuf,tmp_buf);
	CConsole::PrintTime(szReadyBuf,COLOUR_RED,5);
	//Sleep(250);
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
