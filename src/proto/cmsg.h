#ifndef _PROTO_CMSG_H
#define _PROTO_CMSG_H

#include <string>
#include <nlohmann/json.hpp>

#include "msg.h"

namespace proto {
  class cmsg : public msg {
  private:
    typedef msg super;
  public:
    cmsg();
    std::string target;
    std::string source;
    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif