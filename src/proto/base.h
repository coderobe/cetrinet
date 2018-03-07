#ifndef _PROTO_BASE_H
#define _PROTO_BASE_H

#include <string>
#include <nlohmann/json.hpp>

namespace proto {
  class base {
  public:
    base();
    size_t version;
    std::string type;

    void populate();
    void load_json(nlohmann::json payload);
    std::vector<unsigned char> encode(nlohmann::json patch = {});
  };
}

#endif