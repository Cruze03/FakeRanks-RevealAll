#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

class CGameEntitySystem;

#include <ISmmPlugin.h>
#include <igameevents.h>
#include <igameeventsystem.h>
#include <irecipientfilter.h>
#include <sh_vector.h>
#include "utlvector.h"
#include "ehandle.h"
#include <iserver.h>
#include <entity2/entitysystem.h>
#include "entitysystem.h"
#include "sdk/schemasystem.h"
#include "sdk/CBaseEntity.h"
#include "sdk/CGameRulesProxy.h"
#include "sdk/CBasePlayerPawn.h"
#include "sdk/CCSPlayerController.h"
#include "sdk/CCSPlayer_ItemServices.h"
#include "sdk/CSmokeGrenadeProjectile.h"
#include "sdk/module.h"

class CRecipientFilter : public IRecipientFilter
{
public:
	CRecipientFilter()
	{
		m_bReliable = true;
		m_bInitMessage = false;
	}

	CRecipientFilter(IRecipientFilter *source, int iExcept = -1)
	{
		m_bReliable = source->IsReliable();
		m_bInitMessage = source->IsInitMessage();
		m_Recipients.RemoveAll();

		for (int i = 0; i < source->GetRecipientCount(); i++)
		{
			if (source->GetRecipientIndex(i).Get() != iExcept)
				m_Recipients.AddToTail(source->GetRecipientIndex(i));
		}
	}

	~CRecipientFilter() override {}

	bool IsReliable(void) const override { return m_bReliable; }
	bool IsInitMessage(void) const override { return m_bInitMessage; }
	int GetRecipientCount(void) const override { return m_Recipients.Count(); }

	CPlayerSlot GetRecipientIndex(int slot) const override
	{
		if (slot < 0 || slot >= GetRecipientCount())
			return CPlayerSlot(-1);

		return m_Recipients[slot];
	}

	void AddRecipient(CPlayerSlot slot)
	{
		// Don't add if it already exists
		if (m_Recipients.Find(slot) != m_Recipients.InvalidIndex())
			return;

		m_Recipients.AddToTail(slot);
	}

private:
	bool m_bReliable;
	bool m_bInitMessage;
	CUtlVectorFixed<CPlayerSlot, 64> m_Recipients;
};

class FakeRank_RevealAll final : public ISmmPlugin, public IMetamodListener
{
public:
	bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late);
	bool Unload(char* error, size_t maxlen);
	void AllPluginsLoaded();
private:
	const char* GetAuthor();
	const char* GetName();
	const char* GetDescription();
	const char* GetURL();
	const char* GetLicense();
	const char* GetVersion();
	const char* GetDate();
	const char* GetLogTag();
	void StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*);
	void GameFrame(bool simulating, bool bFirstTick, bool bLastTick);
};

#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
