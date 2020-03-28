#include "util.h"

using namespace std;
using json = nlohmann::json;

namespace util {
  atomic<bool> stdout_silenced = false;

  void stdout_silence(){
    if(!util::stdout_silenced){
      stdout_silenced = true;

      #ifdef WINDOWS
        cerr << "util::stdout_silence() not implemented on this platform" << endl;
      #else
        freopen("/dev/null", "w", stdout);
      #endif
    }
  }

  bool is_not_digit(char c){
    return !isdigit(c);
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
    ui_update_chats();
  }

  void send_message(string to, string content){
    proto::cmsg* msg = new proto::cmsg();
    msg->target = to;
    msg->message = content;
    net_send(json::to_msgpack(msg->encode()));
    delete msg;
  }

  void join_channel(string channel){
    proto::join* msg = new proto::join();
    msg->target = channel;
    net_send(json::to_msgpack(msg->encode()));
    delete msg;
  }

  void set_ready(string channel, bool ready){
    proto::greadystate* msg = new proto::greadystate();
    msg->target = channel;
    msg->ready = ready;
    net_send(json::to_msgpack(msg->encode()));
    delete msg;
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
    proto::gstart* msg = new proto::gstart();
    msg->target = channel;
    net_send(json::to_msgpack(msg->encode()));
    delete msg;
  }

  void send_gstop(string channel){
    proto::gstop* msg = new proto::gstop();
    msg->target = channel;
    net_send(json::to_msgpack(msg->encode()));
    delete msg;
  }

  void stdout_unsilence(){
    if(util::stdout_silenced){
      stdout_silenced = false;

      #ifdef WINDOWS
        cerr << "util::stdout_unsilence() not implemented on this platform" << endl;
      #else
        freopen("/dev/tty", "w", stdout);
      #endif
    }
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