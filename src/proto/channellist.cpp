#include "channellist.h"

using json = nlohmann::json;

namespace proto {
  channellist::channellist(){
    populate();
  }
  channellist::~channellist(){
    for(channel* c : channels){
      delete c;
    }
  }

  void channellist::populate(){
    super::populate();
  }
  
  void channellist::load_json(json payload){
    super::load_json(payload);

    json channels_json = payload["d"].value("c", json::object());
    for(auto chan_json : channels_json){
      channel* chan = new channel;
      chan->name = chan_json.value("n", "<error>");
      chan->users = chan_json.value("u", 0);
      chan->joined = false;
      channels.push_back(chan);
    }

  }

  json channellist::encode(){
    json payload = super::encode();

    payload["d"]["c"] = json::object();
    for(channel* chan : channels){
      payload["d"]["c"] += {
        {"name", chan->name},
        {"users", chan->users}
      };
    }

    return payload;
  }
}