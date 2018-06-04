#ifndef _PROTO_GSTART_H
#define _PROTO_GSTART_H

#include <string>
#include <nlohmann/json.hpp>

#include "msg.h"

namespace proto {
  class gstart : public msg {
  private:
    typedef msg super;
  public:
    gstart();
    std::string target;
    std::vector<std::string> users;

    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif