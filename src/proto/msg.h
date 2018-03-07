#ifndef _PROTO_MSG_H
#define _PROTO_MSG_H

#include <string>
#include <nlohmann/json.hpp>

#include "base.h"

namespace proto {
  class msg : public base {
  private:
    typedef base super;
  public:
    msg();
    std::string message;

    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif