// File Author: kyeman

#ifndef _GENERIC_H_
#define _GENERIC_H_

#include "../main.h"

#define MAX_SPAWNS 100

class CGameModeGeneric
{
private:
	BOOL				m_bGameStated;
	int					m_iLastSpawnIssued;
	PLAYER_SPAWN_INFO	m_AvailableSpawns[MAX_SPAWNS];
	int					m_iAvailableSpawnCount;
public:

	int Init();
	BOOL HandleSpawnClassRequest(BYTE bytePlayerID,int iSpawnType);
	
	CGameModeGeneric() { 
		m_iLastSpawnIssued = 0;
		m_iAvailableSpawnCount = 0;
	};

	~CGameModeGeneric() {};
};

#endif