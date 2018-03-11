#ifndef _PROTO_MESSAGE_H
#define _PROTO_MESSAGE_H

#include <string>

namespace proto {
  typedef struct {
    std::string to;
    std::string from;
    std::string content;
    unsigned char rgb[3];
  } message;
}

#endif
