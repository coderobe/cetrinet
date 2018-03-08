#include "join.h"

using json = nlohmann::json;

namespace proto {
  join::join(){
    populate();
  }

  void join::populate(){
    super::populate();

    type = "join";
    user = "";
    target = "";
  }
  
  void join::load_json(json payload){
    super::load_json(payload);

    user = payload["d"].value("u", "");
    target = payload["d"].value("t", "");
  }

  json join::encode(){
    json payload = super::encode();

    payload["d"]["u"] = user;
    payload["d"]["t"] = target;

    return payload;
  }
}