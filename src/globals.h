#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <vector>
#include <thread>

#include "proto/internal/channel.h"
#include "proto/internal/message.h"

#include <TGUI/TGUI.hpp>
#include <simple-websocket-server/client_ws.hpp>

using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;

extern std::shared_ptr<WsClient> net_client;
extern std::vector<std::shared_ptr<std::thread>> threads;
extern std::string server;
extern std::string port;
extern std::string username;
extern std::mutex channels_lock;
extern std::vector<std::shared_ptr<proto::internal::channel>> channels;
extern std::mutex server_messages_lock;
extern std::vector<std::shared_ptr<proto::internal::message>> server_messages;
extern sf::RenderWindow window;
extern tgui::Gui gui;
extern sf::Color color_white;
extern sf::Color color_black;
extern sf::Color color_grey;
extern sf::Color color_red;
extern sf::Color color_green;
extern sf::Color color_blue;

extern void clean_up();

#endif
