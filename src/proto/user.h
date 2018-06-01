#ifndef _PROTO_USER_H
#define _PROTO_USER_H

#include <string>

namespace proto {
  typedef struct {
    std::string name;
    bool ready;
  } user;
}

#endif
