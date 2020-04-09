#include "userlist.h"

using json = nlohmann::json;

namespace proto {
  userlist::userlist(){
    populate();
  }

  void userlist::populate(){
    super::populate();
  }

  void userlist::load_json(json payload){
    super::load_json(payload);

    target = payload["d"].value("t", "<error>");

    json users_json = payload["d"].value("u", json::array());
    for(auto user_json : users_json){
      std::shared_ptr<internal::user> u = std::make_shared<internal::user>();
      u->name = user_json;
      users.push_back(u);
    }

  }

  json userlist::encode(){
    json payload = super::encode();

    payload["d"]["t"] = target;

    payload["d"]["u"] = json::array();
    for(std::shared_ptr<internal::user> u : users){
      payload["d"]["u"] += {u->name};
    }

    return payload;
  }
}
