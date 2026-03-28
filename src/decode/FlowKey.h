#pragma once

#include <boost/functional/hash.hpp>
#include <cstdint>
#include <string>

namespace sniffles::decode {

struct FlowKey {
  std::string src;
  std::string dst;
  std::uint16_t src_port = 0;
  std::uint16_t dst_port = 0;

  bool operator==(const FlowKey &other) const {
    return src == other.src && dst == other.dst &&
           src_port == other.src_port && dst_port == other.dst_port;
  }

  bool operator!=(const FlowKey &other) const { return !(*this == other); }
};

} // namespace sniffles::decode

namespace std {

template <>
struct hash<sniffles::decode::FlowKey> {
  std::size_t operator()(const sniffles::decode::FlowKey &key) const {
    std::size_t seed = 0;
    boost::hash_combine(seed, key.src);
    boost::hash_combine(seed, key.dst);
    boost::hash_combine(seed, key.src_port);
    boost::hash_combine(seed, key.dst_port);
    return seed;
  }
};

} // namespace std
