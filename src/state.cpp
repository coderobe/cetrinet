#include "state.h"

using namespace std;
using json = nlohmann::json;

#define COMPATIBLE_PTR_ASSIGN(from, as, of, c) if(shared_ptr<of> as = dynamic_pointer_cast<of>(from)) c
#define EVENT_AS(t, c) COMPATIBLE_PTR_ASSIGN(base_event, event, t, {event->load_json(payload); c})
template<typename T> shared_ptr<proto::base> create_event(){
  return make_shared<T>();
}
map<string, function<shared_ptr<proto::base>()>> payload_type;
atomic<bool> payload_type_init = false;

void state_init(){
  if(!payload_type_init){
    payload_type_init = true;
    payload_type["auth"] = &create_event<proto::auth>;
    payload_type["channellist"] = &create_event<proto::channellist>;
    payload_type["channelupdate"] = &create_event<proto::channelupdate>;
    payload_type["cmsg"] = &create_event<proto::cmsg>;
    payload_type["error"] = &create_event<proto::error>;
    payload_type["greadystate"] = &create_event<proto::greadystate>;
    payload_type["gstart"] = &create_event<proto::gstart>;
    payload_type["gstop"] = &create_event<proto::gstop>;
    payload_type["gtick"] = &create_event<proto::gtick>;
    payload_type["join"] = &create_event<proto::join>;
    payload_type["motd"] = &create_event<proto::motd>;
    payload_type["part"] = &create_event<proto::part>;
    payload_type["smsg"] = &create_event<proto::smsg>;
    payload_type["userlist"] = &create_event<proto::userlist>;
  }
}

shared_ptr<proto::base> event_from_type(string type){
  return payload_type[type]();
}
shared_ptr<proto::base> event_from_payload(json payload){
  return event_from_type(payload["t"]);
}

void state_update(json payload){
  shared_ptr<proto::base> base_event;

  try {
    base_event = event_from_payload(payload);
  } catch(bad_function_call) {
    util::add_error_message("Received incompatible packet of type '"+string(payload["t"])+"'");
    return;
  }

  EVENT_AS(proto::motd, {
    util::add_message("server", "MOTD", event->message, (unsigned char[3]){0, 0, 100});
    return;
  });

  EVENT_AS(proto::error, {
    util::add_message("server", "Error", event->message + " (" + event->code + ")", (unsigned char[3]){100, 0, 0});
    if(event->code == "name_unavailable"){
      ui_show_reauth();
    }
    return;
  });

  EVENT_AS(proto::join, {
    bool found = false;
    for(shared_ptr<proto::channel> chan : channels){
      if(chan->name == event->target){
        found = true;
        if(event->user == username){
          chan->joined = true;
        }else{
          shared_ptr<proto::user> nu = make_shared<proto::user>();
          nu->name = event->user;
          chan->userdata.push_back(nu);
        }
        ui_update_channels();
      }
    }
    if(!found){
      //util::add_muted_message("Server sent a join event for an unknown channel ("+event->target+")");

      vector<shared_ptr<proto::user>> new_users;
      shared_ptr<proto::user> nu = make_shared<proto::user>();
      nu->name = event->user;
      new_users.push_back(nu);
      shared_ptr<proto::channel> channel = make_shared<proto::channel>();
      channel->name = event->target;
      channel->userdata = new_users;
      channel->joined = event->user == username;
      channels.push_back(channel);
      //util::add_notify_message_for(event->target, "Discovered channel via join event");
      ui_update_channels();
    }
    util::add_muted_message_for(event->target, event->user+" joined the channel");
    ui_update_users();
    return;
  });


  EVENT_AS(proto::part, {
    for(shared_ptr<proto::channel> chan : channels){
      if(chan->name == event->target){
        if(event->user == username){
          chan->joined = false;
        }else{
          for(auto user : chan->userdata){
            if(user->name == event->user){
              chan->userdata.erase(remove(chan->userdata.begin(), chan->userdata.end(), user), chan->userdata.end());
            }
          }
        }
        ui_update_channels();
      }
    }
    util::add_muted_message_for(event->target, event->user+" left the channel");
    ui_update_users();
    return;
  });

  EVENT_AS(proto::smsg, {
    if(event->target.size() == 0){
      event->target = "server";
    }
    util::add_message(event->target, "Server", event->message, (unsigned char[3]){100, 70, 0});
    return;
  });

  EVENT_AS(proto::cmsg, {
    util::add_message(event->target, event->source, event->message);
    return;
  });

  EVENT_AS(proto::channellist, {
    for(shared_ptr<proto::channel> chan : event->channels){
      shared_ptr<proto::channel> channel = make_shared<proto::channel>();
      channel->name = chan->name;
      channel->users = chan->users;
      channel->joined = chan->joined;
      channels.push_back(channel);
      //util::add_info_message_for(chan->name, "Found channel with "+to_string(chan->users)+" users");
    }
    ui_update_channels();
    return;
  });

  EVENT_AS(proto::channelupdate, {
    for(shared_ptr<proto::channel> chan : channels){
      if(chan->name == event->target){
        chan->users = event->users;
        if(chan->users == 0){
          channels.erase(std::remove(channels.begin(), channels.end(), chan), channels.end());
        }
        ui_update_channels();
        return;
      }
    }

    vector<shared_ptr<proto::user>> new_users;
    shared_ptr<proto::channel> channel = make_shared<proto::channel>();
    channel->name = event->target;
    channel->userdata = new_users;
    channel->users = event->users;
    channel->joined = false;
    channels.push_back(channel);

    ui_update_channels();
    return;
  });

  EVENT_AS(proto::userlist, {
    for(shared_ptr<proto::channel> chan : channels){
      if(chan->name == event->target){
        vector<shared_ptr<proto::user>> new_users;
        for(shared_ptr<proto::user> eu : event->users){
          shared_ptr<proto::user> nu = make_shared<proto::user>();
          nu->name = eu->name;
          nu->ready = false;
          new_users.push_back(nu);
        }
        chan->userdata.swap(new_users);
        ui_update_users();
        return;
      }
    }

    vector<shared_ptr<proto::user>> new_users;
    for(shared_ptr<proto::user> eu : event->users){
      shared_ptr<proto::user> nu = make_shared<proto::user>();
      nu->name = eu->name;
      nu->ready = false;
      new_users.push_back(nu);
    }

    shared_ptr<proto::channel> channel = make_shared<proto::channel>();
    channel->name = event->target;
    channel->userdata = new_users;
    channel->joined = false;
    channels.push_back(channel);
    //util::add_notify_message_for(event->target, "Discovered channel via userlist");

    //util::add_info_message_for(event->target, "Userlist reported "+to_string(event->users.size())+" users in this channel");
    ui_update_users();
    return;
  });

  EVENT_AS(proto::gtick, {
    cout << "server tick" << endl;

    //TODO: hook up to game logic
    return;
  });

  EVENT_AS(proto::greadystate, {
    util::add_notify_message_for(event->target, event->source+" is "+(event->ready ? "" : "not")+" ready");

    for(shared_ptr<proto::channel> chan : channels){
      if(chan->name == event->target){
        for(auto user : chan->userdata){
          if(user->name == event->source){
            user->ready = event->ready;
          }
        }
      }
    }
    ui_update_users();
    return;
  });

  EVENT_AS(proto::gstart, {
    util::add_notify_message_for(event->target, "The game is starting, "+to_string(event->users.size())+" users are playing");

    // TODO: hook up to game logic
    return;
  });

  EVENT_AS(proto::gstop, {
    util::add_notify_message_for(event->target, "The game ended");

    // TODO: hook up to game logic
    return;
  });
}