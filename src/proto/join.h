#ifndef _PROTO_JOIN_H
#define _PROTO_JOIN_H

#include <string>
#include <nlohmann/json.hpp>

#include "base.h"

namespace proto {
  class join : public base {
  private:
    typedef base super;
  public:
    join();
    std::string user;
    std::string target;
    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif