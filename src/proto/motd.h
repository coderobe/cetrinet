#ifndef _PROTO_MOTD_H
#define _PROTO_MOTD_H

#include <string>
#include <nlohmann/json.hpp>

#include "msg.h"

namespace proto {
  class motd : public msg {
  private:
    typedef msg super;
  public:
    motd();
    void populate();
    void load_json(nlohmann::json payload);
  };
}

#endif