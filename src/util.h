#ifndef _UTIL_H
#define _UTIL_H

#include <fstream>
#include <atomic>
#include <iostream>

#include "globals.h"
#include "net.h"

namespace util{
  void stdout_silence();
  void stdout_unsilence();
  void thread_start_net(wchar_t* server, wchar_t* username);
  void thread_start_ui();
};

#endif