#pragma once
#include "CPlayerPawnComponent.h"
#include "ehandle.h"
#include "schemasystem.h"

class CPlayer_WeaponServices
{
public:
	SCHEMA_FIELD(CUtlVector<CHandle<int>>, CPlayer_WeaponServices, m_hMyWeapons);
	SCHEMA_FIELD(uint16_t, CPlayer_WeaponServices, m_iAmmo);
};