#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <vector>
#include <thread>

#include <simple-websocket-server/client_ws.hpp>

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;

extern WsClient* net_client;
extern std::vector<std::thread*> threads;
extern wchar_t server[256];
extern wchar_t username[256];

#endif