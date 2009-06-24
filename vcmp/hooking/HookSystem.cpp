
#include "../main.h"

BYTE * CreateJump ( DWORD dwFrom, DWORD dwTo, BYTE * ByteArray )
{
	ByteArray[0] = 0xE9;
    *(DWORD *)(&ByteArray[1]) = dwTo - (dwFrom + 5);
	return ByteArray;
}

BOOL HookInstall( DWORD dwInstallAddress,
				  DWORD dwHookHandler,
				  int iJmpCodeSize )
{
	BYTE JumpBytes[MAX_JUMPCODE_SIZE];
	memset ( JumpBytes, 0x90, MAX_JUMPCODE_SIZE );
	if ( CreateJump ( dwInstallAddress, dwHookHandler, JumpBytes ) )
	{
		memcpy ( (PVOID)dwInstallAddress, JumpBytes, iJmpCodeSize );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
