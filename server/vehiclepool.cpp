// File Author: kyeman

#include "netgame.h"
#include "CarCols.h"

extern BOOL bDebugMessages;
extern CCarColManager *pCarColManager;

//----------------------------------------------------

CVehiclePool::CVehiclePool()
{
	BYTE byteVehicleID = 0;
	while(byteVehicleID != MAX_VEHICLES) {
		m_bVehicleSlotState[byteVehicleID] = FALSE;
		m_pVehicles[byteVehicleID] = NULL;
		byteVehicleID++;
	}
}

//----------------------------------------------------

BOOL CVehiclePool::InitialiseFromConfig(char* szFile)
{
	BYTE d=1;
	VECTOR vecPos;
	TiXmlDocument* m_xmlDoc = NULL;
	m_xmlDoc = new TiXmlDocument(szFile);
	if (!m_xmlDoc->LoadFile())
	{
		logprintf("The vehicles file %s failed to load!",szFile);
	}
	TiXmlElement* xmlRoot = m_xmlDoc->FirstChildElement("Vehicle");
	while (xmlRoot) 
	{
		BYTE bModel = static_cast< BYTE >(atoi(xmlRoot->Attribute("model")));
		float fX = (float)atof(xmlRoot->Attribute("x"));
		float fY = (float)atof(xmlRoot->Attribute("y"));
		float fZ = (float)atof(xmlRoot->Attribute("z"));
		vecPos.X = fX;
		vecPos.Y = fY;
		vecPos.Z = fZ;
		float fAngle = (float)atof(xmlRoot->Attribute("angle"));
		char cCol1 = (char)atoi( xmlRoot->Attribute("col1") );
		char cCol2 = (char)atoi( xmlRoot->Attribute("col2") );
		if (bDebugMessages==TRUE)
			CConsole::Print("CVehiclePool::New(%u,%.2f,%.2f,%.2f,%.2f,%d,%d)\n",COLOUR_RED,bModel,vecPos.X,vecPos.Y,vecPos.Z,fAngle,cCol1,cCol2);
		CCarCols* pCarCols = pCarColManager->Find( bModel );
		if(pCarCols)
		{
			pCarCols->GenerateRandCol();
			if ( cCol1 == -1 ) 
			{
				cCol1 = pCarCols->RandomColour1();
			}
			if ( cCol2 == -1 ) 
			{
				cCol2 = pCarCols->RandomColour2();
			}
		}
		else
		{
			if ( cCol1 == -1 ) cCol1 = 0;
			if ( cCol2 == -1 ) cCol2 = 0;
		}
		New(d,bModel,&vecPos,fAngle,cCol1,cCol2);
		d++;
		xmlRoot = xmlRoot->NextSiblingElement("Vehicle");
	}
	return TRUE;
}


//----------------------------------------------------

CVehiclePool::~CVehiclePool()
{	
	BYTE byteVehicleID = 0;
	while(byteVehicleID != MAX_VEHICLES) {
		Delete(byteVehicleID);
		byteVehicleID++;
	}
}

//----------------------------------------------------

BOOL CVehiclePool::New(BYTE byteVehicleID, BYTE byteVehicleType,
					   VECTOR * vecPos, float fRotation,
					   int iColor1, int iColor2)
{
	m_pVehicles[byteVehicleID] = new CVehicle(byteVehicleType,vecPos,fRotation,iColor1,iColor2);

	/*
	logprintf("CVehiclePool::New(%u,%.2f,%.2f,%.2f,%.2f,%d,%d)",
		byteVehicleType,vecPos->X,vecPos->Y,vecPos->Z,fRotation,iColor1,iColor2);*/

	if(m_pVehicles[byteVehicleID])
	{
		m_pVehicles[byteVehicleID]->SetID(byteVehicleID);
		m_bVehicleSlotState[byteVehicleID] = TRUE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//----------------------------------------------------

BOOL CVehiclePool::Delete(BYTE byteVehicleID)
{
	if(!GetSlotState(byteVehicleID) || !m_pVehicles[byteVehicleID])
	{
		return FALSE;
	}

	m_bVehicleSlotState[byteVehicleID] = FALSE;
	delete m_pVehicles[byteVehicleID];
	m_pVehicles[byteVehicleID] = NULL;

	return TRUE;
}

//----------------------------------------------------

void CVehiclePool::Process()
{
	
}

//----------------------------------------------------