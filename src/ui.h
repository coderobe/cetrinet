#ifndef _UI_H
#define _UI_H

#include <string>
#include <atomic>
#include <thread>
#include <iostream>
#include <cctype>
#include <cwchar>

#include <fontconfig/fontconfig.h>
#include <TGUI/TGUI.hpp>

#include "globals.h"
#include "util.h"
#include "proto/cmsg.h"

// Fuck windows, honestly
#define WIN32_LEAN_AND_MEAN
#undef MessageBox

void ui_worker();
void ui_update_chats();
void ui_update_channels();
bool ui_running();

#endif