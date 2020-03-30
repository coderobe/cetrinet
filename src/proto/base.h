#ifndef _PROTO_BASE_H
#define _PROTO_BASE_H

#include <string>
#include <nlohmann/json.hpp>

namespace proto {
  class base {
  public:
    base();
    virtual ~base();
    size_t version;
    std::string type;

    void populate();
    void load_json(nlohmann::json payload);
    nlohmann::json encode();
  };
}

#endif