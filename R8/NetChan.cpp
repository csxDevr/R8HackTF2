#include "NetChan.h"
#include "R8_inetchannel.h"
#include "Aim.h"
#include "VMTHooks.h"
#include "Misc.h"
#include "Entity.h"

void AddLatencyToNetchan(CNetChan *netchan, float Latency);

int __fastcall Hooked_SendDatagram(CNetChan* netchan, void*, bf_write* datagram)
{
	VMTManager& hook = VMTManager::GetHook(netchan); //Get a pointer to the instance of your VMTManager with the function GetHook.
	auto CallOriginal = hook.GetMethod<int(__thiscall*)(CNetChan*, bf_write*)>((int)e_offset::SendDatagram); //Call the original.

	static float flLatency = netchan->GetLatency(MAX_FLOWS);
	float flDesired = gAim.backtrack.value * TICK_INTERVAL;

	try
	{
		bool bSkip = false;
		if (gAim.smarttrack.value != 1 || flDesired <= 0.2f || datagram)
			bSkip = true;

		CPlayer* you;
		if (!GetBaseEntity(me)->ToPlayer(you) || you->Lifestate() != LIFE_ALIVE)
			bSkip = true;

		if (bSkip)
		{
			flLatency = netchan->GetLatency(MAX_FLOWS);
			return CallOriginal(netchan, datagram);
		}

		if (flLatency > flDesired)
			flLatency = flDesired;
		else if (flLatency < flDesired && (flLatency += (TICK_INTERVAL / 10)) > flDesired)
			flLatency = flDesired;

		int instate = netchan->m_nInReliableState;
		int insequencenr = netchan->m_nInSequenceNr;

		AddLatencyToNetchan(netchan, flLatency);

		int ret = CallOriginal(netchan, datagram);

		netchan->m_nInReliableState = instate;
		netchan->m_nInSequenceNr = insequencenr;

		return ret;
	}
	catch (...)
	{
		CATCH_WARN();
		return CallOriginal(netchan, datagram);
	}
}

class CIncomingSequence
{
public:
	int		m_nInReliableState;
	int		m_nOutReliableState;
	int		m_nInSequenceNr;
	float	curtime;

	CIncomingSequence(int InReliableState, int OutReliableState, int InSequenceNr, float Time)
	{
		m_nInReliableState = InReliableState, m_nOutReliableState = OutReliableState, m_nInSequenceNr = InSequenceNr, curtime = Time;
	}
};

#include <list>
list<CIncomingSequence> sequences;

#include "Engine.h"
void AddLatencyToNetchan(CNetChan *netchan, float Latency)
{
	for (auto& seq : sequences)
	{
		if (gBase.Globals->curtime - seq.curtime >= Latency)
		{
			netchan->m_nInReliableState = seq.m_nInReliableState;
			netchan->m_nInSequenceNr = seq.m_nInSequenceNr;
			break;
		}
	}
}

int last_insequence;
void UpdateIncomingSequences()
{
	CNetChan *netchan = (CNetChan*)gBase.Engine->GetNetChannelInfo();
	if (!netchan)
		return;

	if (netchan->m_nInSequenceNr > last_insequence)
	{
		last_insequence = netchan->m_nInSequenceNr;
		sequences.push_front(CIncomingSequence(netchan->m_nInReliableState, netchan->m_nOutReliableState, netchan->m_nInSequenceNr, gBase.Globals->curtime));
	}

	if (sequences.size() > 2048)
		sequences.pop_back();
}

void ResetIncomingSequences()
{
	last_insequence = 0;
	sequences.clear();
}