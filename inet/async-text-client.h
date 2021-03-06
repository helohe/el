/*######   Copyright (c) 2013-2015 Ufasoft  http://ufasoft.com  mailto:support@ufasoft.com,  Sergey Pavlov  mailto:dev@ufasoft.com ####
#                                                                                                                                     #
# 		See LICENSE for licensing information                                                                                         #
#####################################################################################################################################*/

#pragma once

#include <el/libext/ext-net.h>
#include <el/inet/proxy-client.h>


namespace Ext { namespace Inet {
using namespace Ext;

class AsyncTextClient : public SocketThread {
	typedef SocketThread base;
public:
	ProxyClient Tcp;
	IPEndPoint EpServer;
	StreamWriter W;
	StreamReader R;
	CBool ConnectionEstablished;

	AsyncTextClient()
		:	base(0)
		,	W(Tcp.Stream)
		,	R(Tcp.Stream)
		,	FirstByte(-1)
	{}

	void Send(RCString cmd);
protected:
	mutex MtxSend;
	String DataToSend;
	int FirstByte;
	
	void SendPendingData();
	void Execute() override;
	virtual void OnLine(RCString line) {}
	virtual void OnLastLine(RCString line) { OnLine(line); }
};



}} // Ext::Inet::

