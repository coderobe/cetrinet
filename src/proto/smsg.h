#ifndef _PROTO_SMSG_H
#define _PROTO_SMSG_H

#include <string>
#include <nlohmann/json.hpp>

#include "msg.h"

namespace proto {
  class smsg : public msg {
  private:
    typedef msg super;
  public:
    smsg();
    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif