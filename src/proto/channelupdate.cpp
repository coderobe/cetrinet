#include "channelupdate.h"

using json = nlohmann::json;

namespace proto {
  channelupdate::channelupdate(){
    populate();
  }

  void channelupdate::populate(){
    super::populate();
  }
  
  void channelupdate::load_json(json payload){
    super::load_json(payload);

    target = payload["d"].value("t", "<error>");
    users = payload["d"].value("u", 0);
  }

  json channelupdate::encode(){
    json payload = super::encode();

    payload["d"]["t"] = target;
    payload["d"]["u"] = users;

    return payload;
  }
}