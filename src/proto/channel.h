#ifndef _PROTO_CHANNEL_H
#define _PROTO_CHANNEL_H

#include <string>

namespace proto {
  typedef struct {
    std::string name;
    size_t users;
    bool joined;
  } channel;
}

#endif
