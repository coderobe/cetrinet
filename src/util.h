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

namespace util{
  void stdout_silence();
  void stdout_unsilence();
  void thread_start_net();
  void thread_start_ui();
  bool is_not_digit(char c);
};

#endif