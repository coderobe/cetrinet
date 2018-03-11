#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include "ui.h"
#include "util.h"
#include "proto/channel.h"

using namespace std;

// globals
WsClient* net_client = nullptr;
std::vector<std::thread*> threads;
std::string server;
std::string port;
std::string username;
std::vector<proto::channel*> channels;
sf::RenderWindow window(sf::VideoMode(800, 700), "cetrinet");

int main(){
  window.setActive(false);
  cout << "Hello, cetrinet!" << endl;

  util::thread_start_ui();

  while(!threads.empty()){
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

  while(!channels.empty()){
    delete channels.front();
    channels.erase(channels.begin());
  }

  cout << "Done" << endl;
  return 0;
}
