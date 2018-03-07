#ifndef _NET_H
#define _NET_H

#include <chrono>
#include <iostream>
#include <vector>
#include <nlohmann/json.hpp>

#include "globals.h"
#include "ui.h"

#include "proto/motd.h"

void net_worker(wchar_t* server, wchar_t* port, wchar_t* username);
void net_send(std::vector<unsigned char> data);

#endif