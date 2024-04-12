#pragma once
#include "CCSPlayerController_InGameMoneyServices.h"
#include "CBasePlayerController.h"
#include "CCSPlayerPawn.h"
#include "ehandle.h"
#include "schemasystem.h"

class CCSPlayerController : public CBasePlayerController
{
public:
	SCHEMA_FIELD(CCSPlayerController_InGameMoneyServices*, CCSPlayerController, m_pInGameMoneyServices);
	SCHEMA_FIELD(CUtlSymbolLarge, CCSPlayerController, m_szClan);
	SCHEMA_FIELD(char[32], CCSPlayerController, m_szClanName);
	SCHEMA_FIELD(CHandle<CCSPlayerPawn>, CCSPlayerController, m_hPlayerPawn);
	SCHEMA_FIELD(int32_t, CCSPlayerController, m_iCompetitiveRanking);
	SCHEMA_FIELD(int8_t, CCSPlayerController, m_iCompetitiveRankType);
};