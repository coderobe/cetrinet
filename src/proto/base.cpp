#include "base.h"

using json = nlohmann::json;

namespace proto {
  base::base(){
    populate();
  }
  void base::populate(){
    version = 1;
    type = "";
  }
  
  void base::load_json(json payload){
    type = payload.value("t", "");
  }
}