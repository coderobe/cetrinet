#include "channellist.h"

using json = nlohmann::json;

namespace proto {
  channellist::channellist(){
    populate();
  }

  void channellist::populate(){
    super::populate();
  }
  
  void channellist::load_json(json payload){
    super::load_json(payload);

    json channels_json = payload["d"].value("c", json::object());
    for(auto chan_json : channels_json){
      std::shared_ptr<channel> chan = std::make_shared<channel>();
      chan->name = chan_json.value("t", "<error>");
      chan->users = chan_json.value("u", 0);
      chan->joined = false;
      channels.push_back(chan);
    }

  }

  json channellist::encode(){
    json payload = super::encode();

    payload["d"]["c"] = json::object();
    for(std::shared_ptr<channel> chan : channels){
      payload["d"]["c"] += {
        {"t", chan->name},
        {"u", chan->users}
      };
    }

    return payload;
  }
}