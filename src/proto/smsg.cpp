#include "smsg.h"

using json = nlohmann::json;

namespace proto {
  smsg::smsg(){
    populate();
  }

  void smsg::populate(){
    super::populate();

    type = "smsg";
  }
  
  void smsg::load_json(json payload){
    super::load_json(payload);
  }

  json smsg::encode(){
    json payload = super::encode();

    return payload;
  }
}