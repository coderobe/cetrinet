#ifndef _UTIL_H
#define _UTIL_H

#include <fstream>
#include <atomic>
#include <iostream>
#include <cctype>
#include <cstdlib>
#include <string>

#include "globals.h"
#include "net.h"
#include "proto/message.h"

namespace util{
  void stdout_silence();
  void stdout_unsilence();
  void thread_start_net();
  void thread_start_ui();
  void add_message(std::string to, std::string from, std::string content);
  void add_message(std::string to, std::string from, std::string content, unsigned char color[3]);
  void send_message(std::string to, std::string content);
  void join_channel(std::string channel);
  bool is_not_digit(char c);
};

#endif