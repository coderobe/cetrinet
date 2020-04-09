#ifndef _PROTO_CHANNELLIST_H
#define _PROTO_CHANNELLIST_H

#include <string>
#include <nlohmann/json.hpp>

#include "base.h"
#include "internal/channel.h"

namespace proto {
  class channellist : public base {
  private:
    typedef base super;
  public:
    channellist();
    std::vector<std::shared_ptr<internal::channel>> channels;

    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif
