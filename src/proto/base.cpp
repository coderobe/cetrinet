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

  std::vector<unsigned char> base::encode(json patch){
    json payload;
    payload["v"] = version;
    payload["t"] = type;
    payload.merge_patch(patch);

    return json::to_msgpack(payload);
  }
}