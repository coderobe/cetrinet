#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include "ui.h"
#include "util.h"

#define inline_raw(...) ""#__VA_ARGS__""

using namespace std;

// globals
WsClient* net_client = nullptr;
std::vector<std::thread*> threads;
wchar_t server[256];
wchar_t port[256];
wchar_t username[256];

int main(){
  cout << "Hello, cetrinet!" << endl;

  util::thread_start_ui();

  while(!threads.empty()){
    cout << "Waiting for threads..." << endl;
    thread* t = threads.front();
    threads.erase(threads.begin());
    threads.shrink_to_fit();
    if(t->joinable()){
      if(net_client != nullptr)
        net_client->stop();
      t->join();
    }
    delete t;
  }

  cout << "Done" << endl;
  return 0;
}
