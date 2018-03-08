#ifndef _UTIL_H
#define _UTIL_H

#include <fstream>
#include <atomic>
#include <iostream>
#include <cctype>
#include <cstdlib>
#include <string>

#include <LCUI_Build.h>
#include <LCUI/LCUI.h>
#include <LCUI/gui/widget.h>
#undef max
#undef min

#include "globals.h"
#include "net.h"

namespace util{
  void stdout_silence();
  void stdout_unsilence();
  void thread_start_net();
  void thread_start_ui();
  void get_char_from_textinput_event(LCUI_WidgetEvent event, char* dest);
  bool is_not_digit(char c);
  std::string wctos(wchar_t* w, size_t len);
  std::string wctos(wchar_t* w);
  LCUI_Widget get_widget(std::string id);
  LCUI_Widget get_widget(char* id);
};

#endif