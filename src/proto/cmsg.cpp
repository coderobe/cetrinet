#include "cmsg.h"

using json = nlohmann::json;

namespace proto {
  cmsg::cmsg(){
    populate();
  }

  void cmsg::populate(){
    super::populate();

    type = "cmsg";
    target = "";
    source = "";
  }
  
  void cmsg::load_json(json payload){
    super::load_json(payload);

    target = payload["d"].value("t", "");
    source = payload["d"].value("s", "");
  }

  json cmsg::encode(){
    json payload = super::encode();

    payload["d"]["t"] = target;
    payload["d"]["s"] = source;

    return payload;
  }
}