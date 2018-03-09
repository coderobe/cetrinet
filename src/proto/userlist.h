#ifndef _PROTO_USERLIST_H
#define _PROTO_USERLIST_H

#include <string>
#include <nlohmann/json.hpp>

#include "base.h"
#include "user.h"

namespace proto {
  class userlist : public base {
  private:
    typedef base super;
  public:
    userlist();
    ~userlist();
    std::string channel;
    std::vector<user *> users;

    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif