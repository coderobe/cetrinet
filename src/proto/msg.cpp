#include "msg.h"

using json = nlohmann::json;

namespace proto {
  msg::msg(){
    populate();
  }

  void msg::populate(){
    super::populate();

    message = "";
  }
  
  void msg::load_json(json payload){
    super::load_json(payload);

    message = payload["d"].value("m", "<error>");
  }
}