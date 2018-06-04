#include "gstart.h"

using json = nlohmann::json;

namespace proto {
  gstart::gstart(){
    populate();
  }

  void gstart::populate(){
    super::populate();

    type = "gstart";
    target = "";
  }
  
  void gstart::load_json(json payload){
    super::load_json(payload);

    target = payload["d"].value("t", "");
    json users_json = payload["d"].value("u", json::array());
    for(auto user : users_json){
      users.push_back(user);
    }
  }

  json gstart::encode(){
    json payload = super::encode();

    payload["d"]["t"] = target;
    payload["d"]["u"] = json::array();
    for(auto user : users){
      payload["d"]["u"] += user;
    }

    return payload;
  }
}