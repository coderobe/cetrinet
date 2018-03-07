#include "auth.h"

using json = nlohmann::json;

namespace proto {
  auth::auth(){
    populate();
  }

  void auth::populate(){
    super::populate();

    type = "auth";
    name = "";
    client = "libcetrinet/0.3.0";
  }
  
  void auth::load_json(json payload){
    super::load_json(payload);

    name = payload["d"].value("n", "");
    client = payload["d"].value("c", "");
  }

  json auth::encode(){
    json payload = super::encode();

    payload["d"]["n"] = name;
    payload["d"]["c"] = client;

    return payload;
  }
}