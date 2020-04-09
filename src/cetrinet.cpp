#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include "ui.h"
#include "util.h"
#include "state.h"
#include "proto/internal/channel.h"
#include "proto/internal/message.h"

using namespace std;

// globals
std::shared_ptr<WsClient> net_client = nullptr;
std::vector<std::shared_ptr<std::thread>> threads;
std::string server;
std::string port;
std::string username;
std::mutex channels_lock;
std::vector<std::shared_ptr<proto::internal::channel>> channels;
std::mutex server_messages_lock;
std::vector<std::shared_ptr<proto::internal::message>> server_messages;
sf::RenderWindow window(sf::VideoMode(948, 720), "cetrinet");
tgui::Gui gui(window);
sf::Color color_white = sf::Color::White;
sf::Color color_black = sf::Color::Black;
sf::Color color_grey = sf::Color(200, 200, 200, 255);
sf::Color color_red = sf::Color(255, 0, 0, 255);
sf::Color color_green = sf::Color(0, 255, 0, 255);
sf::Color color_blue = sf::Color(0, 0, 255, 255);

void clean_up(){

  {
    scoped_lock lock(channels_lock);
    while(!channels.empty()){
      channels.erase(channels.begin());
    }
  }

  {
    scoped_lock lock(server_messages_lock);
    server_messages.erase(
      std::remove_if(
        server_messages.begin(),
        server_messages.end(),
        [](const shared_ptr<proto::internal::message> msg) {
          return (msg->to != "server") && (msg->to != "raw");
        }
      ),
      server_messages.end()
    );
  }
}

int main(){
  window.setFramerateLimit(60);
  window.setVerticalSyncEnabled(true);
  window.setActive(false);

  cout << "Hello, cetrinet!" << endl;

  state_init();
  util::thread_start_ui();

  while(!threads.empty()){
    shared_ptr<thread> t = threads.front();
    threads.erase(threads.begin());
    threads.shrink_to_fit();
    if(t->joinable()){
      t->join();
    }
  }

  clean_up();

  cout << "Done" << endl;
  return 0;
}
