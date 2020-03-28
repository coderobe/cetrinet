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

    channel = payload["d"].value("c", "<error>");

    json users_json = payload["d"].value("u", json::array());
    for(auto user_json : users_json){
      std::shared_ptr<user> u = std::make_shared<user>();
      u->name = user_json;
      users.push_back(u);
    }

  }

  json userlist::encode(){
    json payload = super::encode();

    payload["d"]["c"] = channel;

    payload["d"]["u"] = json::array();
    for(std::shared_ptr<user> u : users){
      payload["d"]["u"] += {u->name};
    }

    return payload;
  }
}