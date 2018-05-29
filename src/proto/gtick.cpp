#include "gtick.h"

using json = nlohmann::json;

namespace proto {
  gtick::gtick(){
    populate();
  }

  void gtick::populate(){
    super::populate();

    type = "gtick";
    target = "";
  }
  
  void gtick::load_json(json payload){
    super::load_json(payload);

    target = payload["d"].value("t", "");
  }

  json gtick::encode(){
    json payload = super::encode();

    payload["d"]["t"] = target;

    return payload;
  }
}