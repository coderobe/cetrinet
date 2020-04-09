#ifndef _PROTO_INTERNAL_CHANNEL_H
#define _PROTO_INTERNAL_CHANNEL_H

#include <string>

#include "user.h"
#include "message.h"

namespace proto::internal {
  typedef struct {
    std::string name;
    size_t users;
    std::vector<std::shared_ptr<user>> userdata;
    std::vector<std::shared_ptr<message>> messages;
    bool joined;
  } channel;
}

#endif
