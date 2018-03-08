#include "error.h"

using json = nlohmann::json;

namespace proto {
  error::error(){
    populate();
  }

  void error::populate(){
    super::populate();

    type = "error";
    code = "";
  }
  
  void error::load_json(json payload){
    super::load_json(payload);

    code = payload["d"].value("c", "");
  }

  json error::encode(){
    json payload = super::encode();

    payload["d"]["c"] = code;

    return payload;
  }
}