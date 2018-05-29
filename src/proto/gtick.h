#ifndef _PROTO_GTICK_H
#define _PROTO_GTICK_H

#include <string>
#include <nlohmann/json.hpp>

#include "msg.h"

namespace proto {
  class gtick : public msg {
  private:
    typedef msg super;
  public:
    gtick();
    std::string target;
    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif