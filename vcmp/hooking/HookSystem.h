
#ifndef __HOOKSYSTEM_H
#define __HOOKSYSTEM_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define		MAX_JUMPCODE_SIZE			20

BOOL HookInstall( DWORD dwInstallAddress, DWORD dwHookHandler, int iJmpCodeSize );
BYTE * CreateJump ( DWORD dwFrom, DWORD dwTo, BYTE * ByteArray );

#endif
