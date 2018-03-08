#ifndef _UI_H
#define _UI_H

#include <string>
#include <atomic>
#include <thread>
#include <iostream>
#include <cctype>
#include <cwchar>

#include <LCUI_Build.h>
#include <LCUI/LCUI.h>
#include <LCUI/display.h>
#include <LCUI/gui/css_parser.h>
#include <LCUI/gui/builder.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/widget_event.h>
#include <LCUI/gui/widget/textedit.h>
#include <LCUI/gui/widget/textview.h>
#include <LCUI/gui/widget/scrollbar.h>
extern "C" {
  #include <LCUIEx.h>
}

#undef max
#undef min

#include "globals.h"
#include "util.h"
#include "proto/cmsg.h"

void ui_chat_message_add_raw(std::string message, std::string type="light");
void ui_populate_fields();
void ui_add_tile(LCUI_Widget parent);
void ui_chat_scroller_worker();
void ui_show_game(bool show);
void ui_worker();

#endif