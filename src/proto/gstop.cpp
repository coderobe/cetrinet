#include "gstop.h"

using json = nlohmann::json;

namespace proto {
  gstop::gstop(){
    populate();
  }

  void gstop::populate(){
    super::populate();

    type = "gstop";
    target = "";
  }
  
  void gstop::load_json(json payload){
    super::load_json(payload);

    target = payload["d"].value("t", "");
  }

  json gstop::encode(){
    json payload = super::encode();

    payload["d"]["t"] = target;

    return payload;
  }
}