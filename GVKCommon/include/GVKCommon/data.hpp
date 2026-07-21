#pragma once
#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>


namespace GVK {

enum class PixelFormat { RGBA };

struct PixelData {
  std::vector<unsigned char> data;
  uint32_t width;
  uint32_t height;

  PixelFormat format;
};

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  bool operator==(const Vertex &other) const {
    return position == other.position && normal == other.normal &&
           uv == other.uv;
  }
};


struct MeshData {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
};

}; // namespace GVK

namespace std {
template <> struct hash<GVK::Vertex> {
  size_t operator()(GVK::Vertex const &vertex) const {
    return ((hash<glm::vec3>()(vertex.position) ^
             (hash<glm::vec3>()(vertex.normal) << 1)) >>
            1) ^
           (hash<glm::vec2>()(vertex.uv) << 1);
  }
};
} // namespace std

