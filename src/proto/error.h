#ifndef _PROTO_ERROR_H
#define _PROTO_ERROR_H

#include <string>
#include <nlohmann/json.hpp>

#include "msg.h"

namespace proto {
  class error : public msg {
  private:
    typedef msg super;
  public:
    error();
    std::string code;
    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif