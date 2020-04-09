#ifndef _PROTO_INTERNAL_USER_H
#define _PROTO_INTERNAL_USER_H

#include <string>

namespace proto::internal {
  typedef struct {
    std::string name;
    bool ready;
  } user;
}

#endif
