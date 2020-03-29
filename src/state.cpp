#include "state.h"

using namespace std;
using json = nlohmann::json;

void state_update(json payload){
  if(payload["t"] == "motd"){
    proto::motd event = proto::motd();
    event.load_json(payload);

    util::add_message("server", "MOTD", event.message, (unsigned char[3]){0, 0, 100});
  }else if(payload["t"] == "error"){
    proto::error event = proto::error();
    event.load_json(payload);

    util::add_message("server", "Error", event.message + " (" + event.code + ")", (unsigned char[3]){100, 0, 0});
  }else if(payload["t"] == "join"){
    proto::join event = proto::join();
    event.load_json(payload);

    bool found = false;
    for(shared_ptr<proto::channel> chan : channels){
      if(chan->name == event.target){
        found = true;
        if(event.user == username){
          util::add_muted_message_for(event.target, "You joined the channel");
          chan->joined = true;
        }else{
          shared_ptr<proto::user> nu = make_shared<proto::user>();
          nu->name = event.user;
          chan->userdata.push_back(nu);
        }
        ui_update_channels();
      }
    }
    if(!found){
      util::add_muted_message("Server sent a join event for an unknown channel ("+event.target+")");

      vector<shared_ptr<proto::user>> new_users;
      shared_ptr<proto::user> nu = make_shared<proto::user>();
      nu->name = event.user;
      new_users.push_back(nu);
      shared_ptr<proto::channel> channel = make_shared<proto::channel>();
      channel->name = event.target;
      channel->userdata = new_users;
      channel->joined = event.user == username;
      channels.push_back(channel);
      util::add_notify_message_for(event.target, "Discovered channel via join event");
      ui_update_channels();
    }
    util::add_muted_message_for(event.target, event.user+" joined the channel");
    ui_update_users();
  }else if(payload["t"] == "part"){
    proto::part event = proto::part();
    event.load_json(payload);

    for(shared_ptr<proto::channel> chan : channels){
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
    util::add_muted_message_for(event.target, event.user+" left the channel");
    ui_update_users();
  }else if(payload["t"] == "smsg"){
    proto::smsg event = proto::smsg();
    event.load_json(payload);

    if(event.target.size() == 0){
      event.target = "server";
    }
    util::add_message(event.target, "Server", event.message, (unsigned char[3]){100, 70, 0});
  }else if(payload["t"] == "cmsg"){
    proto::cmsg event = proto::cmsg();
    event.load_json(payload);

    util::add_message(event.target, event.source, event.message);
  }else if(payload["t"] == "channellist"){
    proto::channellist event = proto::channellist();
    event.load_json(payload);

    for(shared_ptr<proto::channel> chan : event.channels){
      shared_ptr<proto::channel> channel = make_shared<proto::channel>();
      channel->name = chan->name;
      channel->users = chan->users;
      channel->joined = chan->joined;
      channels.push_back(channel);
      util::add_info_message_for(chan->name, "Found channel with "+to_string(chan->users)+" users");
    }
    ui_update_channels();
  }else if(payload["t"] == "userlist"){
    proto::userlist event = proto::userlist();
    event.load_json(payload);

    bool found = false;
    for(shared_ptr<proto::channel> chan : channels){
      if(chan->name == event.channel){
        found = true;

        vector<shared_ptr<proto::user>> new_users;
        for(shared_ptr<proto::user> eu : event.users){
          shared_ptr<proto::user> nu = make_shared<proto::user>();
          nu->name = eu->name;
          nu->ready = false;
          new_users.push_back(nu);
        }
        chan->userdata.swap(new_users);
      }
    }
    if(!found){
      vector<shared_ptr<proto::user>> new_users;
      for(shared_ptr<proto::user> eu : event.users){
        shared_ptr<proto::user> nu = make_shared<proto::user>();
        nu->name = eu->name;
        nu->ready = false;
        new_users.push_back(nu);
      }

      shared_ptr<proto::channel> channel = make_shared<proto::channel>();
      channel->name = event.channel;
      channel->userdata = new_users;
      channel->joined = false;
      channels.push_back(channel);
      util::add_notify_message_for(event.channel, "Discovered channel via userlist");
    }
    util::add_info_message_for(event.channel, "Userlist reported "+to_string(event.users.size())+" users in this channel");
    ui_update_users();
  }else if(payload["t"] == "gtick"){
    proto::gtick event = proto::gtick();
    event.load_json(payload);

    cout << "server tick" << endl;

    //TODO: hook up to game logic
  }else if(payload["t"] == "greadystate"){
    proto::greadystate event = proto::greadystate();
    event.load_json(payload);

    util::add_notify_message_for(event.target, event.source+" is "+(event.ready ? "" : "not")+" ready");

    for(shared_ptr<proto::channel> chan : channels){
      if(chan->name == event.target){
        for(auto user : chan->userdata){
          if(user->name == event.source){
            user->ready = event.ready;
          }
        }
      }
    }
    ui_update_users();
  }else if(payload["t"] == "gstart"){
    proto::gstart event = proto::gstart();
    event.load_json(payload);

    util::add_notify_message_for(event.target, "The game is starting, "+to_string(event.users.size())+" users are playing");

    // TODO: hook up to game logic
  }else if(payload["t"] == "gstop"){
    proto::gstop event = proto::gstop();
    event.load_json(payload);

    util::add_notify_message_for(event.target, "The game ended");

    // TODO: hook up to game logic
  }
}