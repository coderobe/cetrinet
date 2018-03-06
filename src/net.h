#ifndef _NET_H
#define _NET_H

#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>

#include "globals.h"
#include "ui.h"

void net_worker(wchar_t* server, wchar_t* username);

#endif