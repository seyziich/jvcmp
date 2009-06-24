#pragma once

#ifndef _HOOKS_H
#define _HOOKS_H

struct CallHook {
	BYTE bOriginalCode[5];
	PBYTE pPlace;
	PVOID pOriginal;

	void initialize(const char* pOriginalCode, PBYTE place);
	int installHook(void (*hookToInstall)(), bool unprotect);
	int releaseHook(bool unprotect);
};

struct PointerHook {
	PVOID* pPlace;
	PVOID pOriginal;

	void initialize(PVOID* place);
	int installHook(void (*hookToInstall)(), bool unprotect);
	int releaseHook(bool unprotect);
};

struct StompHook {
	BYTE bOriginalCode[15];
	BYTE bCountBytes;
	PBYTE pPlace;

	void initialize(const char* pOriginalCode, BYTE countBytes, PBYTE place);
	int installHook(void (*hookToInstall)(), bool useJump, bool unprotect);
	int releaseHook(bool unprotect);
};

#endif