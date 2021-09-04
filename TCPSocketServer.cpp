#include "stdafx.h"
#include "TCPSocketServer.h"

std::shared_ptr<TCPSocketServer> TCPSocketServer::m_p_instance;
std::shared_ptr<TCPSocketServer> TCPSocketServer::get_instance()
{
	if (m_p_instance == nullptr)m_p_instance.reset(new TCPSocketServer());
	return m_p_instance;
}
TCPSocketServer::~TCPSocketServer()
{
}
TCPSocketServer::TCPSocketServer()
{
	m_runing = false;
}

void TCPSocketServer::run(uint16_t port)
{
	if (m_runing)return;
	m_port = port;
	if (ServerSocket_Init() != SOCKET_OK) {
		::printf("open socket error!");
		return;
	}
	::printf("server run on port:%d\n", m_port);
	m_runing = true;
	auto func = [&]()->void{
		while (m_runing) {
			struct fd_set fds;
			struct timeval timeout = { 0, 1000 };

			SOCKET sock_client;
			SOCKADDR_IN addr_client;
			int len = sizeof(SOCKADDR);

			FD_ZERO(&fds);
			FD_SET(m_sock, &fds);

			if (select(NULL, &fds, NULL, NULL, &timeout) > 0) 
			{
				sock_client = accept(m_sock, (SOCKADDR *)&addr_client, &len);
				if (INVALID_SOCKET == sock_client) { continue; }
				string remote_ip = ::inet_ntoa(addr_client.sin_addr);
				int port = addr_client.sin_port;
				::printf("remote ip:%s,port:%d connect\n",remote_ip.c_str(),port);

			}

			Sleep(10);
		}

		::printf("socket close!program terminal\n");
		closesocket(m_sock);
		WSACleanup();
	};
	if (m_ptr_thread != nullptr) {
		if (m_ptr_thread->joinable())m_ptr_thread->join();
	}
	m_ptr_thread.reset(new std::thread(func));
}

void TCPSocketServer::stop() {
	m_runing = false;
	if (m_ptr_thread != nullptr) {
		if (m_ptr_thread->joinable())m_ptr_thread->join();
	}
}

char TCPSocketServer::ServerSocket_Init()
{
	WSADATA wsaData;
	BOOL bReuseaddr = TRUE;		//设置调用closesocket()后,仍可继续重用该socket
	BOOL bDontLinger = FALSE;	//处于连接状态的soket在调用closesocket()后强制关闭, 不经历TIME_WAIT的过程


	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
		return SOCKET_REQUEST_SOCKET_VER_ERR;

	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET)
		return SOCKET_INVALID_SOCKET;

	setsockopt(m_sock, SOL_SOCKET, SO_DONTLINGER, (const char*)&bDontLinger, sizeof(BOOL));
	setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseaddr, sizeof(BOOL));

	ULONG ul = 1;     // 使socket工作在异步模式
	ioctlsocket(m_sock, FIONBIO, &ul);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = ADDR_ANY;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(m_port);


	if (::bind(m_sock, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR) 
	{
		WSAGetLastError();
		WSACleanup();
		return SOCKET_OPEN_SERVER_ERR;
	}

	//	if (listen(m_sock, 5) == SOCKET_ERROR) {
	if (listen(m_sock, 20) == SOCKET_ERROR) {
		WSAGetLastError();
		WSACleanup();
		return SOCKET_OPEN_SERVER_ERR;
	}


	return SOCKET_OK;
}