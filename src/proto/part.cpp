#include "part.h"

using json = nlohmann::json;

namespace proto {
  part::part(){
    populate();
  }

  void part::populate(){
    super::populate();

    type = "part";
    user = "";
    target = "";
  }
  
  void part::load_json(json payload){
    super::load_json(payload);

    user = payload["d"].value("u", "");
    target = payload["d"].value("t", "");
  }

  json part::encode(){
    json payload = super::encode();

    payload["d"]["u"] = user;
    payload["d"]["t"] = target;

    return payload;
  }
}