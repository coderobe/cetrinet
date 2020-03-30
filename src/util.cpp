#include "util.h"

using namespace std;
using json = nlohmann::json;

namespace util {
  void add_muted_message_for(string channel, string content){
    add_message(channel, "", content, (unsigned char[3]){75, 75, 75});
  }
  void add_muted_message(string content){
    add_muted_message_for("server", content);
  }

  void add_notify_message_for(string channel, string content){
    add_message(channel, "", content, (unsigned char[3]){0, 100, 0});
  }
  void add_notify_message(string content){
    add_notify_message_for("server", content);
  }

  void add_error_message_for(string channel, string content){
    add_message(channel, "", content, (unsigned char[3]){100, 0, 0});
  }
  void add_error_message(string content){
    add_error_message_for("server", content);
  }

  void add_info_message_for(string channel, string content){
    add_message(channel, "", content, (unsigned char[3]){0, 0, 100});
  }
  void add_info_message(string content){
    add_info_message_for("server", content);
  }

  void add_message_divider(){
    shared_ptr<proto::message> lmsg = make_shared<proto::message>();
    lmsg->to = "raw";
    lmsg->content = "---";
    lmsg->rgb[0] = 0;
    lmsg->rgb[1] = 0;
    lmsg->rgb[2] = 0;
    server_messages.push_back(lmsg);
    ui_update_chats();
  }

  void add_message(string to, string from, string content){
    unsigned char color[3] = {0, 0, 0};
    add_message(to, from, content, color);
  }

  void add_message(string to, string from, string content, unsigned char color[3]){
    from = (from.length() == 0 ? "" : from+": ");
    shared_ptr<proto::message> lmsg = make_shared<proto::message>();
    lmsg->to = to;
    lmsg->from = from;
    lmsg->content = content;
    lmsg->rgb[0] = color[0];
    lmsg->rgb[1] = color[1];
    lmsg->rgb[2] = color[2];
    lmsg->time = chrono::system_clock::to_time_t(chrono::system_clock::now());
    server_messages.push_back(lmsg);
    string time = ctime(&(lmsg->time));
    time.pop_back(); // remove trailing newline
    string timepadding(time.length(), ' '); // padding for multiline strings
    string frompadding(from.length(), ' ');

    bool first_line = true;
    istringstream lines(content);
    for(string line; getline(lines, line, '\n'); [](string to, string from, string line, bool& first_line, string time, string timepadding, string frompadding){
      cout << (first_line ? time : timepadding) << " | [" << to << "] " << (first_line ? from : frompadding) << line << endl;
      first_line = false;
    }(to, from, line, first_line, time, timepadding, frompadding));
    ui_update_chats();
  }

  void authenticate_as(string new_username){
    proto::auth auth = proto::auth();
    auth.name = new_username;
    username = new_username.substr(0, new_username.find_last_of("#"));
    util::add_muted_message("Authenticating as '"+username+"'"+(username != new_username ? " with tripcode" : "")+"...");
    net_send(auth.encode());
  }

  void send_message(string to, string content){
    shared_ptr<proto::cmsg> msg = make_shared<proto::cmsg>();
    msg->target = to;
    msg->message = content;
    net_send(msg->encode());
  }

  void join_channel(string channel){
    shared_ptr<proto::join> msg = make_shared<proto::join>();
    msg->target = channel;
    net_send(msg->encode());
  }

  void leave_channel(string channel){
    shared_ptr<proto::part> msg = make_shared<proto::part>();
    msg->target = channel;
    net_send(msg->encode());
  }

  void set_ready(string channel, bool ready){
    shared_ptr<proto::greadystate> msg = make_shared<proto::greadystate>();
    msg->target = channel;
    msg->ready = ready;
    net_send(msg->encode());
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
    net_send(msg->encode());
  }

  void send_gstop(string channel){
    shared_ptr<proto::gstop> msg = make_shared<proto::gstop>();
    msg->target = channel;
    net_send(msg->encode());
  }

  void thread_start_net(){
    net_disconnect();
    cout << "Starting net worker" << endl;
    threads.push_back(std::make_shared<thread>(net_worker));
  }

  void thread_start_ui(){
    cout << "Starting UI worker" << endl;
    //https://github.com/SFML/SFML/blob/master/src/SFML/Window/OSX/SFWindowController.mm#L563-L580
    //threads.push_back(new thread(ui_worker));
    ui_worker();
  }
}