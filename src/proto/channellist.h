#ifndef _PROTO_CHANNELLIST_H
#define _PROTO_CHANNELLIST_H

#include <string>
#include <nlohmann/json.hpp>

#include "base.h"

namespace proto {
  typedef struct {
    std::string name;
    size_t users;
    bool joined;
  } channel;

  class channellist : public base {
  private:
    typedef base super;
  public:
    channellist();
    ~channellist();
    std::vector<channel *> channels;

    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif