#include "smsg.h"

using json = nlohmann::json;

namespace proto {
  smsg::smsg(){
    populate();
  }

  void smsg::populate(){
    super::populate();

    type = "smsg";
    target = "";
  }
  
  void smsg::load_json(json payload){
    super::load_json(payload);

    target = payload["d"].value("t", "");
  }

  json smsg::encode(){
    json payload = super::encode();

    payload["d"]["t"] = target;

    return payload;
  }
}