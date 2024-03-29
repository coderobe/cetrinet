#ifndef _NET_H
#define _NET_H

#include <chrono>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>

#include "globals.h"
#include "ui.h"
#include "state.h"

extern std::atomic<bool> net_connected;
void net_worker();
void net_disconnect();
void net_send(std::vector<unsigned char> data);
void net_send(nlohmann::json payload);

#endif