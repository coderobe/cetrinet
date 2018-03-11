#ifndef _PROTO_CHANNEL_H
#define _PROTO_CHANNEL_H

#include <string>

#include "user.h"
#include "message.h"

namespace proto {
  typedef struct {
    std::string name;
    size_t users;
    std::vector<user*> userdata;
    std::vector<message*> messages;
    bool joined;
  } channel;
}

#endif
