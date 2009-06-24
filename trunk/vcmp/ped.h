// File Author: JackPowell

#pragma once

#include "game.h"

class CPed
{
public:
	CPed::CPed(int iModel, float fX, float fY, float fZ, float fRotation);
	CPed::~CPed(){};

	void  Destroy();
	void  ShowMarker(int iMarkerColor);
	BOOL  IsOnScreen();
	void  Say(UINT uiNum);
	void  SetZAngle(float z);
	float GetHealth();
	void  SetHealth(float fHealth);
	void  SetDead();
	int   GetModel();
	void  SetModel(int iModel);
	void  ForceIntoPassengerSeat(UINT uiVehicleID, UINT uiSeat);

	PED_TYPE   *m_pPed;
	DWORD		m_dwGTAId;
};
