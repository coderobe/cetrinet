#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <vector>
#include <thread>

#include "proto/channel.h"

#include <TGUI/TGUI.hpp>
#include <simple-websocket-server/client_ws.hpp>

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;

extern WsClient* net_client;
extern std::vector<std::thread*> threads;
extern std::string server;
extern std::string port;
extern std::string username;
extern std::vector<proto::channel*> channels;
extern sf::RenderWindow window;

#endif