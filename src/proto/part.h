#ifndef _PROTO_PART_H
#define _PROTO_PART_H

#include <string>
#include <nlohmann/json.hpp>

#include "base.h"

namespace proto {
  class part : public base {
  private:
    typedef base super;
  public:
    part();
    std::string user;
    std::string target;
    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif