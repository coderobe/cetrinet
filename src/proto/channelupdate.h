#ifndef _PROTO_CHANNELUPDATE_H
#define _PROTO_CHANNELUPDATE_H

#include <string>
#include <nlohmann/json.hpp>

#include "base.h"

namespace proto {
  class channelupdate : public base {
  private:
    typedef base super;
  public:
    channelupdate();
    std::string target;
    size_t users;

    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif