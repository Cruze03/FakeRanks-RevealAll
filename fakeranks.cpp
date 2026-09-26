#include <stdio.h>
#include "fakeranks.h"
#include "utils/module.h"
#include "schemasystem/schemasystem.h"
#include "khook_helpers.h"
#include "cs2_sdk/entity/cbaseplayerpawn.h"
#include "cs2_sdk/entity/ccsplayercontroller.h"
#include "cs2_sdk/entity/cbaseplayercontroller.h"
#include <networksystem/inetworkserializer.h>
#include <networksystem/inetworkmessages.h>
#include <inetchannel.h>
#include "protobuf/generated/cstrike15_usermessages.pb.h"

class GameSessionConfiguration_t
{
};

KHOOK_VIRTUAL(GameFrame, &IServerGameDLL::GameFrame, g_pSource2Server, nullptr, Hook_GameFrame_Post);
KHOOK_VIRTUAL(StartupServer, &INetworkServerService::StartupServer, g_pNetworkServerService, nullptr, Hook_StartupServer_Post);

FakeRank_RevealAll g_FakeRanks;
PLUGIN_EXPOSE(FakeRank_RevealAll, g_FakeRanks);

CGlobalVars* g_pGlobals = nullptr;
CGameEntitySystem* g_pEntitySystem = nullptr;
IGameEventSystem* g_pGameEventSystem = nullptr;

uint64_t g_iOldButtons[65];
bool g_bRequiredInitLoaded = true;

CGlobalVars* GetGameGlobals()
{
    INetworkGameServer* srv = g_pNetworkServerService->GetIGameServer();

    if (!srv) return nullptr;

    return g_pNetworkServerService->GetIGameServer()->GetGlobals();
}

CGameEntitySystem* GameEntitySystem()
{
#ifdef WIN32
    static int offset = 88;
#else
    static int offset = 80;
#endif
    return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(g_pGameResourceServiceServer) + offset);
}

std::vector<CKHookBase*>& GetKHookList()
{
    static std::vector<CKHookBase*> s_vecSigHooks;
    return s_vecSigHooks;
}

void InitKHooks()
{
    for (auto hook : GetKHookList())
        hook->Configure();
}

bool FakeRank_RevealAll::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();

    GET_V_IFACE_ANY(GetServerFactory, g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pSchemaSystem, ISchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkMessages, INetworkMessages, NETWORKMESSAGES_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameEventSystem, IGameEventSystem, GAMEEVENTSYSTEM_INTERFACE_VERSION);
    GET_V_IFACE_ANY(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
    GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameResourceServiceServer, IGameResourceService, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);

    g_SMAPI->AddListener(this, this);

    InitKHooks();

    if (!g_bRequiredInitLoaded)
    {
        snprintf(error, maxlen, "Failed to hook one or more virtual functions, please refer to startup logs for more information");
        return false;
    }

    ConVar_Register(FCVAR_RELEASE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);

    if (late)
    {
        g_pEntitySystem = GameEntitySystem();
        g_pGlobals = GetGameGlobals();
    }

    return true;
}

bool FakeRank_RevealAll::Unload(char* error, size_t maxlen) { return true; }

KHook::Return<void> Hook_StartupServer_Post(INetworkServerService* pThis, const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*)
{
    g_pEntitySystem = GameEntitySystem();
    g_pGlobals = GetGameGlobals();
    return { KHook::Action::Ignore };
}

KHook::Return<void> Hook_GameFrame_Post(IServerGameDLL* pThis, bool simulating, bool bFirstTick, bool bLastTick)
{
    if (!g_pEntitySystem || !g_pGlobals) return { KHook::Action::Ignore };

    if (g_pGlobals->tickcount % 12 != 0) return { KHook::Action::Ignore };

    int maxClients = g_pGlobals->maxClients > 65 ? 65 : g_pGlobals->maxClients;
    CRecipientFilter filter;

    for (int i = 0; i < maxClients; i++)
    {
        CCSPlayerController* pPlayerController = (CCSPlayerController*)g_pEntitySystem->GetEntityInstance((CEntityIndex)(i + 1));

        if (!pPlayerController) continue;

        if (!pPlayerController->IsConnected() || !pPlayerController->m_hPawn() || !pPlayerController->m_hPawn()->m_pMovementServices()) continue;

        uint64_t iButtons = pPlayerController->m_hPawn()->m_pMovementServices()->m_nButtons().m_pButtonStates()[0];
        if ((iButtons & PlayerButtons_t::Scoreboard) && !(g_iOldButtons[i] & PlayerButtons_t::Scoreboard))
        {
            filter.AddRecipient(CPlayerSlot(i));
        }
        g_iOldButtons[i] = iButtons;
    }

    if (filter.GetRecipientCount() > 0)
    {
        INetworkMessageInternal* netmsg = g_pNetworkMessages->FindNetworkMessagePartial("CCSUsrMsg_ServerRankRevealAll");
        CNetMessage* msg = netmsg->AllocateMessage();
        g_pGameEventSystem->PostEventAbstract(0, false, &filter, netmsg, msg, 0);
        delete msg;
    }
    return { KHook::Action::Ignore };
}

void FakeRank_RevealAll::AllPluginsLoaded() {}

void FakeRank_RevealAll::OnLevelInit(
    char const* pMapName, char const* pMapEntities, char const* pOldLevel, char const* pLandmarkName, bool loadGame, bool background)
{
}

void FakeRank_RevealAll::OnLevelShutdown() {}

bool FakeRank_RevealAll::Pause(char* error, size_t maxlen) { return true; }

bool FakeRank_RevealAll::Unpause(char* error, size_t maxlen) { return true; }

const char* FakeRank_RevealAll::GetLicense() { return "GPLv3"; }

const char* FakeRank_RevealAll::GetVersion() { return "1.1.3-fix"; }

const char* FakeRank_RevealAll::GetDate() { return __DATE__; }

const char* FakeRank_RevealAll::GetLogTag() { return "FakeRanks"; }

const char* FakeRank_RevealAll::GetAuthor() { return "Cruze"; }

const char* FakeRank_RevealAll::GetDescription() { return "Reveals all fake ranks"; }

const char* FakeRank_RevealAll::GetName() { return "FakeRanks - Reveal All"; }

const char* FakeRank_RevealAll::GetURL() { return "https://github.com/cruze03"; }
