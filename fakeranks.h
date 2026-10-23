#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

#include <ISmmPlugin.h>
#include "engine2/ivengineserver2.h"
#include "engine2/igameeventsystem.h"
#include "engine2/isource2server.h"
#include "engine2/inetworkserverservice.h"
#include "engine2/gamesessionconfiguration.h"
#include "engine2/networkgameserverbase.h"
#include "igameevents.h"
#include "irecipientfilter.h"
#include "khook.hpp"
#include "edict.h"
#include "playerslot.h"
#include <bit>

#ifdef AMBUILD
#include "version_gen.h"
#else
#include "version_gen_placeholder.h"
#endif

class CRecipientFilter : public IRecipientFilter
{
  public:
    CRecipientFilter(NetChannelBufType_t nBufType = BUF_RELIABLE, bool bInitMessage = false) : m_nBufType(nBufType), m_bInitMessage(bInitMessage) {}

    CRecipientFilter(IRecipientFilter* source, CPlayerSlot exceptSlot = -1)
    {
        m_Recipients = source->GetRecipients();
        m_nBufType = source->GetNetworkBufType();
        m_bInitMessage = source->IsInitMessage();

        if (exceptSlot != -1) m_Recipients.Clear(exceptSlot.Get());
    }

    ~CRecipientFilter() override {}

    NetChannelBufType_t GetNetworkBufType(void) const override { return m_nBufType; }
    bool IsInitMessage(void) const override { return m_bInitMessage; }
    const CPlayerBitVec& GetRecipients(void) const override { return m_Recipients; }
    CPlayerSlot GetPredictedPlayerSlot(void) const override { return m_Recipients.Get(0); }

    void AddRecipient(CPlayerSlot slot)
    {
        if (slot.Get() >= 0 && slot.Get() < ABSOLUTE_PLAYER_LIMIT) m_Recipients.Set(slot.Get());
    }

    int GetRecipientCount()
    {
        const uint64 bits = *reinterpret_cast<const uint64*>(&GetRecipients());

        return std::popcount(bits);
    }

  protected:
    NetChannelBufType_t m_nBufType;
    bool m_bInitMessage;
    CPlayerBitVec m_Recipients;
};

KHook::Return<void> Hook_GameFrame_Post(ISource2Server* pThis, bool simulating, bool bFirstTick, bool bLastTick);
KHook::Return<void> Hook_StartupServer_Post(INetworkServerService* pThis, const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*);

class FakeRank_RevealAll : public ISmmPlugin, public IMetamodListener
{
  public:
    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late);
    bool Unload(char* error, size_t maxlen);
    void UpdatePlayers();
    bool Pause(char* error, size_t maxlen);
    bool Unpause(char* error, size_t maxlen);
    void AllPluginsLoaded();

  public: // hooks
    void OnLevelInit(char const* pMapName, char const* pMapEntities, char const* pOldLevel, char const* pLandmarkName, bool loadGame, bool background);
    void OnLevelShutdown();

  public:
    const char* GetAuthor() override { return PLUGIN_AUTHOR; }
    const char* GetName() override { return PLUGIN_DISPLAY_NAME; }
    const char* GetDescription() override { return PLUGIN_DESCRIPTION; }
    const char* GetURL() override { return PLUGIN_URL; }
    const char* GetLicense() override { return PLUGIN_LICENSE; }
    const char* GetVersion() override { return PLUGIN_FULL_VERSION; }
    const char* GetDate() override { return __DATE__; }
    const char* GetLogTag() override { return PLUGIN_LOGTAG; }
};

extern FakeRank_RevealAll g_FakeRanks;

PLUGIN_GLOBALVARS();

#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
