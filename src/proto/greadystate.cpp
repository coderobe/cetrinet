#include "greadystate.h"

using json = nlohmann::json;

namespace proto {
  greadystate::greadystate(){
    populate();
  }

  void greadystate::populate(){
    super::populate();

    target = "";
    source = "";
    ready = false;
  }

  void greadystate::load_json(json payload){
    super::load_json(payload);

    target = payload["d"].value("t", "<error>");
    source = payload["d"].value("s", "<error>");
    ready = payload["d"].value("r", false);
  }

  json greadystate::encode(){
    json payload = super::encode();

    payload["d"]["t"] = target;
    payload["d"]["s"] = source;
    payload["d"]["r"] = ready;

    return payload;
  }
}