
#include <windows.h>

#include "main.h"
#include "game/game.h"
#include "game/util.h"
#include "game/keystuff.h"
#include "game/aimstuff.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern char szSpecialActorModels[][32];

CPed::CPed(int iModel, float fX, float fY, float fZ, float fRotation)
{
	DWORD dwPedID=0;
	m_pPed=0;
	m_dwGTAId=0;
	//if(iModel < 107) return;
	if(!pGame->IsModelLoaded(iModel)) {
		pGame->RequestModel(iModel);
		pGame->LoadRequestedModels();
		while(!pGame->IsModelLoaded(iModel)) { Sleep(1); }
	}
	ScriptCommand(&create_actor, 21, iModel, fX, fY, fZ, &dwPedID);
	m_dwGTAId = dwPedID;
	m_pPed = GamePool_Ped_GetAt(m_dwGTAId);
	ScriptCommand(&set_actor_immunities,m_dwGTAId,1,1,1,1,1);
	SetZAngle(fRotation);
}

void CPed::Destroy()
{
	DWORD dwPedPtr = (DWORD)m_pPed;
	_asm mov ecx, dwPedPtr
	_asm mov ebx, [ecx] ; vtable
	_asm push 1
	_asm call [ebx+8] ; destroy
	m_pPed = NULL;
}

void CPed::ShowMarker(int iMarkerColor)
{	
	DWORD hndMarker;
	DWORD dwPedID;
	float f=0.0f;

	dwPedID = GamePool_Ped_GetIndex(m_pPed);

	_asm push 2
	_asm push 4
	_asm mov eax, dwPedID
	_asm push eax
	_asm push 2
	_asm mov edx, ADDR_TIE_MARKER_TO_ACTOR
	_asm call edx
	_asm mov hndMarker, eax
	_asm pop ecx
	_asm pop ecx
	_asm pop ecx
	_asm pop ecx

	_asm push iMarkerColor
	_asm push hndMarker
	_asm mov edx, ADDR_SET_MARKER_COLOR
	_asm call edx
	_asm pop ecx
	_asm pop ecx

	_asm push 2
	_asm push hndMarker
	_asm mov edx, ADDR_SHOW_ON_RADAR1
	_asm call edx
	_asm pop ecx
	_asm pop ecx

}

BOOL CPed::IsOnScreen()
{
	if(m_pPed) {
		return GameIsEntityOnScreen((DWORD *)m_pPed);
	}
	return FALSE;
}

void CPed::Say(UINT uiNum)
{
	if(m_pPed)
	{
		DWORD dwPedPtr = (DWORD)m_pPed;
		_asm mov ecx, dwPedPtr
		_asm push uiNum
		_asm mov edx, ADDR_CPED_SAY
		_asm call edx
	}
}

void CPed::SetZAngle(float z)
{
	if(m_pPed) {
		m_pPed->fRotation1 = z;
		m_pPed->fRotation2 = z;
	}
}

float CPed::GetHealth()
{	
	return m_pPed->fHealth;
}

void CPed::SetHealth(float fHealth)
{	
	m_pPed->fHealth = fHealth;
}	

void CPed::SetDead()
{
	if(m_pPed) {
		m_pPed->fHealth = 0.0f;
		DWORD dwPed = (DWORD)m_pPed;
		_asm mov ecx, dwPed
		_asm mov edx, 0x4F6430
		_asm call edx
	}
}

int CPed::GetModel()
{
	return m_pPed->entity.nModelIndex;	
}

void CPed::SetModel(int iModel)
{
	PCHAR			szModelName=0;
	DWORD dwPedPtr = (DWORD)m_pPed;

	if(iModel == 8) return; // invalid skin
	
	if(iModel > 106)
	{
		iModel -= 106;
		if(iModel < 54)
		{
			szModelName = szSpecialActorModels[iModel];

			_asm mov ecx, dwPedPtr
			_asm push szModelName
			_asm mov edx, ADDR_SET_SKIN_MODELNAME
			_asm call edx

			_asm push 0
			_asm mov edx, ADDR_LOAD_REQUESTED_MODELS2
			_asm call edx
			_asm pop ecx

			_asm mov ecx, dwPedPtr
			_asm mov edx, ADDR_REFRESH_ACTOR_SKIN
			_asm call edx
		}
	}
	else // default.ide number
	{
		if(!pGame->IsModelLoaded(iModel)) {
			pGame->RequestModel(iModel);
			pGame->LoadRequestedModels();
			while(!pGame->IsModelLoaded(iModel)) Sleep(1);
		}

		_asm mov ecx, dwPedPtr
		_asm push iModel
		_asm mov edx, ADDR_SET_ACTOR_SKIN
		_asm call edx
	}
}

void CPed::ForceIntoPassengerSeat(UINT uiVehicleID, UINT uiSeat)
{
	PED_TYPE * pPed = m_pPed;
	VEHICLE_TYPE * pVehicle = GamePool_Vehicle_GetAt(uiVehicleID);
	UINT uiPassengerOffset;

	if(!pPed || !pVehicle) return;
	if(uiSeat > pVehicle->byteMaxPassengers) return;

	pPed->byteAction = 50;

	pVehicle->pPassengers[uiSeat-1] = pPed;
	uiPassengerOffset = 424 + (uiSeat * 4);

	_asm mov     eax, pVehicle
	_asm add	 eax, uiPassengerOffset
	_asm push    eax
	_asm mov     eax, pVehicle
	_asm mov     ecx, pPed
	_asm mov	 edx, ADDR_ACTOR_PUT_IN_VEHICLE
	_asm call    edx

	_asm mov	 ebx, pPed
	_asm mov     eax, pVehicle
	_asm mov     [ebx+936], eax
	_asm lea     eax, [ebx+936]
	_asm mov     ecx, [ebx+936]
	_asm push    eax
	_asm mov	 edx, ADDR_ACTOR_PUT_IN_VEHICLE
	_asm call    edx

	pPed->byteIsInVehicle = 1;
	pPed->entity.nControlFlags2 &= 0xFE;

	_asm push 0
	_asm push pVehicle
	_asm mov ecx, pPed
	_asm mov edx, ADDR_VEHICLE_SET_DRIVER
	_asm call edx
}
