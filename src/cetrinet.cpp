#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include "ui.h"
#include "util.h"
#include "proto/channel.h"
#include "proto/message.h"

using namespace std;

// globals
WsClient* net_client = nullptr;
std::vector<std::thread*> threads;
std::string server;
std::string port;
std::string username;
std::vector<proto::channel*> channels;
std::vector<proto::message*> server_messages;
sf::RenderWindow window(sf::VideoMode(948, 720), "cetrinet");
tgui::Gui gui(window);
sf::Color color_white = sf::Color(255, 255, 255, 255);
sf::Color color_black = sf::Color(0, 0, 0, 255);
sf::Color color_grey = sf::Color(200, 200, 200, 255);
sf::Color color_red = sf::Color(255, 0, 0, 255);
sf::Color color_green = sf::Color(0, 255, 0, 255);
sf::Color color_blue = sf::Color(0, 0, 255, 255);

void clean_up(){
  while(!channels.empty()){
    delete channels.front();
    channels.erase(channels.begin());
  }

  while(!server_messages.empty()){
    delete server_messages.front();
    server_messages.erase(server_messages.begin());
  }
}

int main(){
  window.setFramerateLimit(60);
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

  clean_up();

  cout << "Done" << endl;
  return 0;
}
