#ifndef _PROTO_AUTH_H
#define _PROTO_AUTH_H

#include <string>
#include <nlohmann/json.hpp>

#include "base.h"

namespace proto {
  class auth : public base {
  private:
    typedef base super;
  public:
    auth();
    std::string name;
    std::string client;

    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif