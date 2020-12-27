#pragma once
#include "R8_inetmessage.h"
#include "R8_inetchannelinfo.h"
#include "R8_inetmsghandler.h"

class CNetMessage : public INetMessage
{
public:
	CNetMessage()
	{
		m_bReliable = true;
		m_NetChannel = 0;
	}

	virtual ~CNetMessage() {};

	virtual int GetGroup() const
	{
		return INetChannelInfo::GENERIC;
	}
	INetChannel *GetNetChannel() const
	{
		return m_NetChannel;
	}

	virtual void SetReliable(bool state)
	{
		m_bReliable = state;
	};
	virtual bool IsReliable() const
	{
		return m_bReliable;
	};
	virtual void SetNetChannel(INetChannel *netchan)
	{
		m_NetChannel = netchan;
	}
	virtual bool Process()
	{
		return false;
	}; // no handler set

protected:
	bool m_bReliable;          // true if message should be send reliable
	INetChannel *m_NetChannel; // netchannel this message is from/for
};

// Thanks to Cathook developers!

#define net_StringCmd 4   // a string command

#define DECLARE_BASE_MESSAGE(msgtype)                                          \
public:                                                                        \
    bool ReadFromBuffer(bf_read &buffer);                                      \
    bool WriteToBuffer(bf_write &buffer);                                      \
    const char *ToString() const;                                              \
    int GetType() const                                                        \
    {                                                                          \
        return msgtype;                                                        \
    }                                                                          \
    const char *GetName() const                                                \
    {                                                                          \
        return #msgtype;                                                       \
    }

#define DECLARE_NET_MESSAGE(name)                                              \
    DECLARE_BASE_MESSAGE(net_##name);                                          \
    INetMessageHandler *m_pMessageHandler;                                     \
    bool Process()                                                             \
    {                                                                          \
        return false;                                                          \
    }

class NET_StringCmd : public CNetMessage
{
	DECLARE_NET_MESSAGE(StringCmd);

	int GetGroup() const
	{
		return INetChannelInfo::STRINGCMD;
	}

	NET_StringCmd()
	{
		m_szCommand = nullptr;
	}
	NET_StringCmd(const char *cmd)
	{
		m_szCommand = cmd;
	}

public:
	const char *m_szCommand; // execute this command

private:
	char m_szCommandBuffer[1024]; // buffer for received messages
};