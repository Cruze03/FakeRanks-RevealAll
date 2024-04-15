#include <entitysystem.h>
#include <convar.h>
#include <networksystem/inetworkserializer.h>
#include <networksystem/inetworkmessages.h>
#include <inetchannel.h>
#include "sdk/schemasystem.h"
#include "protobuf/generated/cstrike15_usermessages.pb.h"
#include "fakeranks.h"

FakeRank_RevealAll g_LR_FakeRank;
PLUGIN_EXPOSE(FakeRank_RevealAll, g_LR_FakeRank);

// /IVEngineServer2* engine = nullptr;
CSchemaSystem* g_pCSchemaSystem = nullptr;
CGameEntitySystem* g_pGameEntitySystem = nullptr;
CEntitySystem* g_pEntitySystem = nullptr;
IGameEventSystem *g_pGameEventSystem = nullptr;
IGameResourceServiceServer* g_pGameResourceService = nullptr;

class GameSessionConfiguration_t { };
SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t&, ISource2WorldSession*, const char*);
SH_DECL_HOOK3_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);

uint64_t iOldButtons[64];

bool FakeRank_RevealAll::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();
	//GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
	//GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION)
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pCSchemaSystem, CSchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkMessages, INetworkMessages, NETWORKMESSAGES_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetEngineFactory, g_pGameEventSystem, IGameEventSystem, GAMEEVENTSYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetServerFactory, g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameResourceService, IGameResourceServiceServer, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
	//GET_V_IFACE_CURRENT(GetFileSystemFactory, g_pFullFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);

	SH_ADD_HOOK(IServerGameDLL, GameFrame, g_pSource2Server, SH_MEMBER(this, &FakeRank_RevealAll::GameFrame), true);
	SH_ADD_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_MEMBER(this, &FakeRank_RevealAll::StartupServer), true);

	ConVar_Register(FCVAR_RELEASE | FCVAR_SERVER_CAN_EXECUTE | FCVAR_GAMEDLL);
	return true;
}

bool FakeRank_RevealAll::Unload(char *error, size_t maxlen)
{
	SH_REMOVE_HOOK(IServerGameDLL, GameFrame, g_pSource2Server, SH_MEMBER(this, &FakeRank_RevealAll::GameFrame), true);
	SH_REMOVE_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_MEMBER(this, &FakeRank_RevealAll::StartupServer), true);
	return true;
}

void FakeRank_RevealAll::GameFrame(bool simulating, bool bFirstTick, bool bLastTick)
{
	if(!g_pEntitySystem)
		return;
	
	for (int i = 0; i < 64; i++)
	{
		CCSPlayerController* pPlayerController =  (CCSPlayerController *)g_pEntitySystem->GetBaseEntity((CEntityIndex)(i + 1));

		if(!pPlayerController) continue;
		if(pPlayerController->m_iConnected() != PlayerConnectedState::PlayerConnected || !pPlayerController->m_hPawn() || !pPlayerController->m_hPawn()->m_pMovementServices()) continue;

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

CGameEntitySystem* GameEntitySystem()
{
	g_pGameEntitySystem = *reinterpret_cast<CGameEntitySystem**>(reinterpret_cast<uintptr_t>(g_pGameResourceService) + 0x50);
	return g_pGameEntitySystem;
}

void FakeRank_RevealAll::StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*)
{
	g_pEntitySystem = GameEntitySystem();
}

///////////////////////////////////////
const char* FakeRank_RevealAll::GetLicense()
{
	return "GPL";
}

const char* FakeRank_RevealAll::GetVersion()
{
	return "1.0.2";
}

const char* FakeRank_RevealAll::GetDate()
{
	return __DATE__;
}

const char* FakeRank_RevealAll::GetLogTag()
{
	return "FakeRanks";
}

const char* FakeRank_RevealAll::GetAuthor()
{
	return "Cruze";
}

const char* FakeRank_RevealAll::GetDescription()
{
	return "Reveals all fake ranks";
}

const char* FakeRank_RevealAll::GetName()
{
	return "FakeRanks - Reveal All";
}

const char* FakeRank_RevealAll::GetURL()
{
	return "https://github.com/cruze03";
}

