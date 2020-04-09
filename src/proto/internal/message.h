#ifndef _PROTO_INTERNAL_MESSAGE_H
#define _PROTO_INTERNAL_MESSAGE_H

#include <string>
#include <ctime>

namespace proto::internal {
  typedef struct {
    std::string to;
    std::string from;
    std::string content;
    std::time_t time;
    unsigned char rgb[3];
  } message;
}

#endif
