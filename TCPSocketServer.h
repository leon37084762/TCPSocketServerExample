#pragma once
#ifndef _TCPSOCKETSERVER_H
#define _TCPSOCKETSERVER_H
#include <string>
#include<list>
#include <queue>
#include <functional>
#include <algorithm>
#include <string>
#include <mutex>
#include <Winsock2.h>
#include "iphlpapi.h"   
#pragma comment ( lib, "ws2_32.lib" )   
#pragma comment ( lib, "Iphlpapi.lib" )   

#define SOCKET_OK					     0
#define SOCKET_GET_HOST_NAME_ERR		-1
#define SOCKET_BAD_HOST_LOOKUP			-2
#define SOCKET_REQUEST_SOCKET_VER_ERR	-3
#define SOCKET_INVALID_SOCKET			-4
#define SOCKET_OPEN_SERVER_ERR			-5
using namespace std;

class TCPSocketServer
{
public:
	~TCPSocketServer();
public:
	void run(uint16_t port);
	void stop();
public:
	static std::shared_ptr<TCPSocketServer> get_instance();
private:
	char ServerSocket_Init();
private:
	SOCKET m_sock;
	UINT m_port;
	bool m_runing;
	std::shared_ptr<std::thread> m_ptr_thread;
private:
	TCPSocketServer();
	static std::shared_ptr<TCPSocketServer> m_p_instance;
};

#endif