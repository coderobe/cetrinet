#include "util.h"

using namespace std;
using json = nlohmann::json;

namespace util {
  void add_notify_message(string content){
    add_message("server", "cetrinet", content, (unsigned char[3]){0, 100, 0});
  }
  void add_error_message(string content){
    add_message("server", "cetrinet", content, (unsigned char[3]){100, 0, 0});
  }
  void add_info_message(string content){
    add_message("server", "cetrinet", content, (unsigned char[3]){0, 0, 100});
  }

  void add_message(string to, string from, string content){
    unsigned char color[3] = {0, 0, 0};
    add_message(to, from, content, color);
  }

  void add_message(string to, string from, string content, unsigned char color[3]){
    proto::message* lmsg = new proto::message;
    lmsg->to = to;
    lmsg->from = from;
    lmsg->content = content;
    lmsg->rgb[0] = color[0];
    lmsg->rgb[1] = color[1];
    lmsg->rgb[2] = color[2];
    server_messages.push_back(lmsg);
    cout << "[" << to << "] " << from << ": '" << content << "'" << endl;
    ui_update_chats();
  }

  void send_message(string to, string content){
    shared_ptr<proto::cmsg> msg = make_shared<proto::cmsg>();
    msg->target = to;
    msg->message = content;
    net_send(json::to_msgpack(msg->encode()));
  }

  void join_channel(string channel){
    shared_ptr<proto::join> msg = make_shared<proto::join>();
    msg->target = channel;
    net_send(json::to_msgpack(msg->encode()));
  }

  void set_ready(string channel, bool ready){
    shared_ptr<proto::greadystate> msg = make_shared<proto::greadystate>();
    msg->target = channel;
    msg->ready = ready;
    net_send(json::to_msgpack(msg->encode()));
  }

  void toggle_ready(string channel){
    for(auto chan : channels){
      if(chan->name == channel){
        for(auto user : chan->userdata){
          if(user->name == username){
            set_ready(chan->name, !user->ready);
            return;
          }
        }
      }
    }
  }

  void send_gstart(string channel){
    shared_ptr<proto::gstart> msg = make_shared<proto::gstart>();
    msg->target = channel;
    net_send(json::to_msgpack(msg->encode()));
  }

  void send_gstop(string channel){
    shared_ptr<proto::gstop> msg = make_shared<proto::gstop>();
    msg->target = channel;
    net_send(json::to_msgpack(msg->encode()));
  }

  void thread_start_net(){
    net_disconnect();
    cout << "Starting net worker" << endl;
    threads.push_back(new thread(net_worker));
  }

  void thread_start_ui(){
    cout << "Starting UI worker" << endl;
    //https://github.com/SFML/SFML/blob/master/src/SFML/Window/OSX/SFWindowController.mm#L563-L580
    //threads.push_back(new thread(ui_worker));
    ui_worker();
  }
}