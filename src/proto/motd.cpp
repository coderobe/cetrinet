#include "motd.h"

using json = nlohmann::json;

namespace proto {
  motd::motd(){
    populate();
  }

  void motd::populate(){
    super::populate();

    type = "motd";
  }
  
  void motd::load_json(json payload){
    super::load_json(payload);
  }
}