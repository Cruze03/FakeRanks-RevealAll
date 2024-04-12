#pragma once
#include "CCSPlayerPawnBase.h"
#include "schemasystem.h"

struct GameTime_t
{
public:
	float m_Value; // 0x0	
};

class CCSPlayerPawn : public CCSPlayerPawnBase
{
public:
    SCHEMA_FIELD(GameTime_t, CCSPlayerPawn, m_flHealthShotBoostExpirationTime);
};