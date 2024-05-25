#include <stdio.h>
#include "fakeranks.h"
#include "utils/module.h"
#include "schemasystem/schemasystem.h"
#include <funchook.h>
#include "cs2_sdk/entity/cbaseplayerpawn.h"
#include "cs2_sdk/entity/ccsplayercontroller.h"
#include "cs2_sdk/entity/cbaseplayercontroller.h"
#include <networksystem/inetworkserializer.h>
#include <networksystem/inetworkmessages.h>
#include <inetchannel.h>
#include "protobuf/generated/cstrike15_usermessages.pb.h"

class GameSessionConfiguration_t { };

SH_DECL_HOOK3_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);

/*
#ifdef _WIN32
#define ROOTBIN "/bin/win64/"
#define GAMEBIN "/csgo/bin/win64/"
#else
#define ROOTBIN "/bin/linuxsteamrt64/"
#define GAMEBIN "/csgo/bin/linuxsteamrt64/"
#endif
*/

FakeRank_RevealAll g_FakeRanks;
PLUGIN_EXPOSE(FakeRank_RevealAll, g_FakeRanks);

IServerGameDLL* server = NULL;
CSchemaSystem* g_pSchemaSystem2 = nullptr;
CGlobalVars *g_pGlobals = nullptr;
CGameEntitySystem* g_pEntitySystem = nullptr;
IGameEventSystem *g_pGameEventSystem = nullptr;

uint64_t iOldButtons[65];

CGlobalVars* GetGameGlobals()
{
	INetworkGameServer* srv = g_pNetworkServerService->GetIGameServer();

	if (!srv)
		return nullptr;

	return g_pNetworkServerService->GetIGameServer()->GetGlobals();
}

CGameEntitySystem* GameEntitySystem()
{
#ifdef WIN32
	static int offset = 88;
#else
	static int offset = 80;
#endif
	return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(g_pGameResourceServiceServer)+offset);
}

bool FakeRank_RevealAll::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	GET_V_IFACE_ANY(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pSchemaSystem2, CSchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkMessages, INetworkMessages, NETWORKMESSAGES_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameEventSystem, IGameEventSystem, GAMEEVENTSYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameResourceServiceServer, IGameResourceService, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);

	g_SMAPI->AddListener( this, this );

	SH_ADD_HOOK(IServerGameDLL, GameFrame, server, SH_MEMBER(this, &FakeRank_RevealAll::Hook_GameFrame), true);

	ConVar_Register( FCVAR_RELEASE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL );

	if(late)
	{
		g_pEntitySystem = GameEntitySystem();
		g_pGlobals = GetGameGlobals();
	}

	return true;
}

bool FakeRank_RevealAll::Unload(char *error, size_t maxlen)
{
	SH_REMOVE_HOOK(IServerGameDLL, GameFrame, server, SH_MEMBER(this, &FakeRank_RevealAll::Hook_GameFrame), true);

	return true;
}

void FakeRank_RevealAll::Hook_StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*)
{
	g_pEntitySystem = GameEntitySystem();
	g_pGlobals = GetGameGlobals();
}

void FakeRank_RevealAll::Hook_GameFrame(bool simulating, bool bFirstTick, bool bLastTick)
{
	if(!g_pEntitySystem || !g_pGlobals)
		return;
	
	if(g_pGlobals->tickcount % 12 != 0)
		return;
	
	int maxClients = g_pGlobals->maxClients > 64 ? 64 : g_pGlobals->maxClients;
	
	for(int i = 0; i < maxClients; i++)
	{
		CCSPlayerController* pPlayerController =  (CCSPlayerController *)g_pEntitySystem->GetEntityInstance((CEntityIndex)(i + 1));

		if(!pPlayerController) continue;
		if(!pPlayerController->IsConnected() || !pPlayerController->m_hPawn() || !pPlayerController->m_hPawn()->m_pMovementServices()) continue;

		uint64_t iButtons = pPlayerController->m_hPawn()->m_pMovementServices()->m_nButtons().m_pButtonStates()[0];
		if(std::to_string(iButtons).find("858993") != std::string::npos && !(std::to_string(iOldButtons[i]).find("858993") != std::string::npos))
		{
			CRecipientFilter filter;
			CPlayerSlot PlayerSlot = CPlayerSlot(i);
			filter.AddRecipient(PlayerSlot);
			static INetworkSerializable* message_type = g_pNetworkMessages->FindNetworkMessagePartial("CCSUsrMsg_ServerRankRevealAll");
			CCSUsrMsg_ServerRankRevealAll message;
			g_pGameEventSystem->PostEventAbstract(0, false, &filter, message_type, &message, 0);
		}
		iOldButtons[i] = iButtons;
	}
}

void FakeRank_RevealAll::AllPluginsLoaded()
{
}

void FakeRank_RevealAll::OnLevelInit(char const* pMapName,
	char const* pMapEntities,
	char const* pOldLevel,
	char const* pLandmarkName,
	bool loadGame,
	bool background)
{
}

void FakeRank_RevealAll::OnLevelShutdown()
{
}

bool FakeRank_RevealAll::Pause(char *error, size_t maxlen)
{
	return true;
}

bool FakeRank_RevealAll::Unpause(char *error, size_t maxlen)
{
	return true;
}

const char *FakeRank_RevealAll::GetLicense()
{
	return "GPLv3";
}

const char *FakeRank_RevealAll::GetVersion()
{
	return "1.0.4";
}

const char *FakeRank_RevealAll::GetDate()
{
	return __DATE__;
}

const char *FakeRank_RevealAll::GetLogTag()
{
	return "FakeRanks";
}

const char *FakeRank_RevealAll::GetAuthor()
{
	return "Cruze";
}

const char *FakeRank_RevealAll::GetDescription()
{
	return "Reveals all fake ranks";
}

const char *FakeRank_RevealAll::GetName()
{
	return "FakeRanks - Reveal All";
}

const char *FakeRank_RevealAll::GetURL()
{
	return "https://github.com/cruze03";
}
