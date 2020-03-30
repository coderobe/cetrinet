#include "base.h"

using json = nlohmann::json;

namespace proto {
  base::base(){
    populate();
  }
  base::~base(){
  }
  void base::populate(){
    version = 1;
    type = "";
  }
  
  void base::load_json(json payload){
    type = payload.value("t", "");
  }

  json base::encode(){
    json payload;
    payload["v"] = version;
    payload["t"] = type;

    return payload;
  }
}