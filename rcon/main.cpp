#include "main.h"

void logprintf(char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char buffer[512];
	vsprintf(buffer, format, ap);
	va_end(ap);
	CConsole::PrintTime(buffer,COLOUR_RED,25);
}

CRconClient* pRconClient = NULL;
bool bQuitApp = false;

BOOL WINAPI CtrlHandler(DWORD dwType)
{
	switch (dwType)
	{
		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
		case CTRL_LOGOFF_EVENT:
		case CTRL_SHUTDOWN_EVENT:
			bQuitApp = true;
			return TRUE;
	}
	return FALSE;
}

HANDLE hConsoleExecuteEvent;

DWORD WINAPI ConsoleInputThread(void* pParam)
{
	char buf[512];
	while (true)
	{
		DWORD dwRead;
		ReadConsole(GetStdHandle(STD_INPUT_HANDLE), buf, 255, &dwRead, NULL);
		if (dwRead > 2)
		{
			buf[dwRead-2] = 0;
			WaitForSingleObject(hConsoleExecuteEvent, INFINITE);
			pRconClient->Command(buf);
			SetEvent(hConsoleExecuteEvent);
		}
	}
}

int main (int argc, char** argv)
{
	// Print welcome message.
	logprintf("\n JVCMP Command Line Remote Console Client");
	logprintf(" ----------------------------------------");
	logprintf(" v2.0, 23rd June 2009\n");

	// If there's not enough command line args, print usage.
	if (argc < 3)
	{
		logprintf(" Usage:");
		logprintf("   rcon <ip/host> <port> <password>");
		getc(stdin);
		exit(0);
	}

	// Setup the console
	SetConsoleCtrlHandler(CtrlHandler, TRUE);
	hConsoleExecuteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ConsoleInputThread, NULL, 0, &dwThreadId);

	// Setup the Remote Console client.
	pRconClient = new CRconClient(argv[1], atoi(argv[2]), argv[3]);

	// While not wanting to quit...
	while (!bQuitApp)
	{
		// Process Remote Console client network messages.
		pRconClient->Process();
		// Get input.
		SetEvent(hConsoleExecuteEvent);
		WaitForSingleObject(hConsoleExecuteEvent, INFINITE);
		// Sleep for 100ms
		Sleep(100);
	}

	// Delete the Remote Console client.
	delete pRconClient;

	// Kill the input thread.
	TerminateThread(hThread, 0);
	CloseHandle(hConsoleExecuteEvent);

	return 0;
}