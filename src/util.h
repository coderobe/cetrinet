#ifndef _UTIL_H
#define _UTIL_H

#include <fstream>
#include <atomic>
#include <iostream>
#include <cctype>
#include <cstdlib>
#include <string>
#include <chrono>

#include "globals.h"
#include "net.h"
#include "proto/message.h"

namespace util{
  void thread_start_net();
  void thread_start_ui();
  void add_notify_message(std::string content);
  void add_error_message(std::string content);
  void add_info_message(std::string content);
  void add_message_divider();
  void add_message(std::string to, std::string from, std::string content);
  void add_message(std::string to, std::string from, std::string content, unsigned char color[3]);
  void send_message(std::string to, std::string content);
  void join_channel(std::string channel);
  void leave_channel(std::string channel);
  void set_ready(std::string channel, bool ready);
  void toggle_ready(std::string channel);
  void send_gstart(std::string channel);
  void send_gstop(std::string channel);
};

#endif