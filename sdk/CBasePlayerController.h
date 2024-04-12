#pragma once
#include "CBaseEntity.h"
#include "CBasePlayerPawn.h"
#include "ehandle.h"
#include "schemasystem.h"

enum class PlayerConnectedState : uint32_t
{
	PlayerNeverConnected = 0xFFFFFFFF,
	PlayerConnected = 0x0,
	PlayerConnecting = 0x1,
	PlayerReconnecting = 0x2,
	PlayerDisconnecting = 0x3,
	PlayerDisconnected = 0x4,
	PlayerReserved = 0x5,
};

class CBasePlayerController : public SC_CBaseEntity
{
public:
	SCHEMA_FIELD(CHandle<CBasePlayerPawn>, CBasePlayerController, m_hPawn);
	SCHEMA_FIELD(char[128], CBasePlayerController, m_iszPlayerName);
	SCHEMA_FIELD(uint64_t, CBasePlayerController, m_steamID);
	SCHEMA_FIELD(PlayerConnectedState, CBasePlayerController, m_iConnected);
};