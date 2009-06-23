// File Author: kyeman

#pragma once

#ifndef _NETGAME_H_
#define _NETGAME_H_

#define IS_FIRING(x) (x & 0x200) // for checking the keystate firing bit

#include "main.h"
#include "player.h"
#include "playerpool.h"
#include "vehicle.h"
#include "vehiclepool.h"
#include "../raknet/PacketEnumerations.h"
#include "netrpc.h"
#include "gamemode/generic.h"

#define UPDATE_TYPE_NONE	0
#define UPDATE_TYPE_FULL	1
#define UPDATE_TYPE_MINIMAL 2

#define GAMESTATE_STOPPED	0
#define GAMESTATE_RUNNING	1

#define INVALID_ID			0xFF

class CNetGame
{
private:

	CPlayerPool					*m_pPlayerPool;
	CVehiclePool				*m_pVehiclePool;
	RakServerInterface			*m_pRak;
	CGameModeGeneric			*m_pGameLogic;
	int							m_iGameState;
		
	void UpdateNetwork();

	// This is from RakNet sources.
	inline BYTE GetPacketID(Packet *p) {
		if (p==0) return 255;

		if ((unsigned char)p->data[0] == ID_TIMESTAMP) {
			assert(p->length > sizeof(unsigned char) + sizeof(unsigned long));
			return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
		}
		else
			return (unsigned char) p->data[0];
	};

	void SetupInitPositions();

public:

	CNetGame(int iMaxPlayers,int iPort,int iGameType,
		char *szPassword,char *szGameFile,
		BYTE byteFriendlyFire,BYTE byteShowOnRadar);

	~CNetGame();

	int GetGameState() { return m_iGameState; };
	CPlayerPool * GetPlayerPool() { return m_pPlayerPool; };
	CVehiclePool * GetVehiclePool() { return m_pVehiclePool; };
	RakServerInterface * GetRakServer() { return m_pRak; };
	CGameModeGeneric * GetGameLogic() { return m_pGameLogic; };
	
	void Process();
	void BroadcastData( RakNet::BitStream *bitStream, PacketPriority priority,
						PacketReliability reliability,
						char orderingStream,
						BYTE byteExcludedPlayer );

	void PlayerSync(Packet *p);
	void VehicleSync(Packet *p);
	void PassengerSync(Packet *p);
	void KickPlayer(BYTE byteKickPlayer);
	void AddBan(char * ip_mask);
	void LoadBanList();

	VECTOR		m_vecInitPlayerPos;
	VECTOR		m_vecInitCameraPos;
	VECTOR		m_vecInitCameraLook;
	float		m_WorldBounds[4]; // pos_x neg_x pos_y neg_y
	int			m_iSpawnsAvailable;
	BYTE		m_byteFriendlyFire;
	BYTE		m_byteShowOnRadar;
};

#endif