#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

#include <ISmmPlugin.h>
#include <igameevents.h>
#include "engine/igameeventsystem.h"
#include "irecipientfilter.h"
#include <sh_vector.h>
#include "iserver.h"
#include <bit>

class CRecipientFilter : public IRecipientFilter
{
public:
	CRecipientFilter(NetChannelBufType_t nBufType = BUF_RELIABLE, bool bInitMessage = false) :
		m_nBufType(nBufType), m_bInitMessage(bInitMessage) {}

	CRecipientFilter(IRecipientFilter* source, CPlayerSlot exceptSlot = -1)
	{
		m_Recipients = GetRecipients();
		m_nBufType = source->GetNetworkBufType();
		m_bInitMessage = source->IsInitMessage();

		if (exceptSlot != -1)
			m_Recipients.Clear(exceptSlot.Get());
	}

	~CRecipientFilter() override {}

	NetChannelBufType_t GetNetworkBufType(void) const override { return m_nBufType; }
	bool IsInitMessage(void) const override { return m_bInitMessage; }
	const CPlayerBitVec& GetRecipients(void) const { return m_Recipients; }

	void AddRecipient(CPlayerSlot slot)
	{
		if (slot.Get() >= 0 && slot.Get() < ABSOLUTE_PLAYER_LIMIT)
			m_Recipients.Set(slot.Get());
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

class FakeRank_RevealAll : public ISmmPlugin, public IMetamodListener
{
public:
	bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);
	bool Unload(char *error, size_t maxlen);
	void UpdatePlayers();
	bool Pause(char *error, size_t maxlen);
	bool Unpause(char *error, size_t maxlen);
	void AllPluginsLoaded();
	void Hook_StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*);
public: //hooks
	void OnLevelInit( char const *pMapName,
				 char const *pMapEntities,
				 char const *pOldLevel,
				 char const *pLandmarkName,
				 bool loadGame,
				 bool background );
	void OnLevelShutdown();
	void Hook_GameFrame( bool simulating, bool bFirstTick, bool bLastTick );
public:
	const char *GetAuthor();
	const char *GetName();
	const char *GetDescription();
	const char *GetURL();
	const char *GetLicense();
	const char *GetVersion();
	const char *GetDate();
	const char *GetLogTag();
};

extern FakeRank_RevealAll g_FakeRanks;

PLUGIN_GLOBALVARS();

#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
