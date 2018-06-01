#ifndef _PROTO_GREADYSTATE_H
#define _PROTO_GREADYSTATE_H

#include <string>
#include <nlohmann/json.hpp>

#include "base.h"

namespace proto {
  class greadystate : public base {
  private:
    typedef base super;
  public:
    greadystate();
    std::string target;
    std::string source;
    bool ready;

    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif