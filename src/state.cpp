#include "state.h"

using namespace std;
using json = nlohmann::json;

void state_update(json payload){
  if(payload["t"] == "motd"){
    proto::motd event = proto::motd();
    event.load_json(payload);

    cout << "motd: " << event.message << endl;

    util::add_message("server", "MOTD", event.message, (unsigned char[3]){0, 0, 100});
  }else if(payload["t"] == "error"){
    proto::error event = proto::error();
    event.load_json(payload);

    cout << "server reports error " << event.code << ": " << event.message << endl;
    util::add_message("server", "Error", event.message, (unsigned char[3]){100, 0, 0});
  }else if(payload["t"] == "join"){
    proto::join event = proto::join();
    event.load_json(payload);

    for(proto::channel* chan : channels){
      if(chan->name == event.target){
        if(event.user == username){
          chan->joined = true;
        }else{
          proto::user* nu = new proto::user;
          nu->name = event.user;
          chan->userdata.push_back(nu);
        }
        ui_update_channels();
      }
    }
    cout << "user '" << event.user << "' joined channel " << event.target << endl;
    util::add_message(event.target, "Join", event.user+" joined the channel", (unsigned char[3]){0, 0, 100});
    ui_update_users();
  }else if(payload["t"] == "part"){
    proto::part event = proto::part();
    event.load_json(payload);

    for(proto::channel* chan : channels){
      if(chan->name == event.target){
        if(event.user == username){
          chan->joined = false;
        }else{
          for(auto user : chan->userdata){
            if(user->name == event.user){
              chan->userdata.erase(remove(chan->userdata.begin(), chan->userdata.end(), user), chan->userdata.end());
            }
          }
        }
        ui_update_channels();
      }
    }
    cout << "user '" << event.user << "' parted from " << event.target << endl;
    util::add_message(event.target, "Part", event.user+" left the channel", (unsigned char[3]){100, 0, 0});
    ui_update_users();
  }else if(payload["t"] == "smsg"){
    proto::smsg event = proto::smsg();
    event.load_json(payload);

    cout << "smsg: " << event.message << endl;
    util::add_message("server", "Server", event.message, (unsigned char[3]){0, 0, 100});
  }else if(payload["t"] == "cmsg"){
    proto::cmsg event = proto::cmsg();
    event.load_json(payload);

    cout << "cmsg from '" << event.source << "': " << event.message << endl;
    util::add_message(event.target, event.source, event.message);
  }else if(payload["t"] == "channellist"){
    proto::channellist event = proto::channellist();
    event.load_json(payload);

    for(proto::channel* chan : event.channels){
      cout << "found channel " << chan->name << " with " << chan->users << " users" << endl;

      proto::channel* channel = new proto::channel;
      channel->name = chan->name;
      channel->users = chan->users;
      channel->joined = chan->joined;
      channels.push_back(channel);
    }
    ui_update_channels();
  }else if(payload["t"] == "userlist"){
    proto::userlist event = proto::userlist();
    event.load_json(payload);

    cout << "userlist for channel " << event.channel << " reports " << event.users.size() << " users" << endl;
    for(proto::channel* chan : channels){
      if(chan->name == event.channel){
        vector<proto::user*> new_users;
        for(proto::user* eu : event.users){
          proto::user* nu = new proto::user;
          nu->name = eu->name;
          new_users.push_back(nu);
        }
        //TODO: clean up new_users to prevent leaking memory?
        chan->userdata.swap(new_users);
      }
    }
    ui_update_users();
  }else if(payload["t"] == "gtick"){
    proto::gtick event = proto::gtick();
    event.load_json(payload);

    cout << "server tick" << endl;

    //TODO: hook up to game logic
  }
}