#ifndef _PROTO_GSTOP_H
#define _PROTO_GSTOP_H

#include <string>
#include <nlohmann/json.hpp>

#include "msg.h"

namespace proto {
  class gstop : public msg {
  private:
    typedef msg super;
  public:
    gstop();
    std::string target;

    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif