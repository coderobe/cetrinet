#ifndef _PROTO_MESSAGE_H
#define _PROTO_MESSAGE_H

#include <string>
#include <ctime>

namespace proto {
  typedef struct {
    std::string to;
    std::string from;
    std::string content;
    std::time_t time;
    unsigned char rgb[3];
  } message;
}

#endif
