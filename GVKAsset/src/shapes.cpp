#include <GVKAsset/shapes.hpp>

namespace GVK::shapes {
MeshData generateUVSphere(float radius, uint32_t stacks, uint32_t slices) {
  assert(radius > 0.0f);
  assert(stacks >= 2);
  assert(slices >= 3);
  MeshData data;
  for (uint32_t stack = 0; stack <= stacks; ++stack) {
    float v = static_cast<float>(stack) / stacks;
    float phi = v * glm::pi<float>();

    for (uint32_t slice = 0; slice <= slices; ++slice) {
      float u = static_cast<float>(slice) / slices;
      float theta = u * glm::two_pi<float>();

      float x = radius * std::sin(phi) * std::cos(theta);
      float y = radius * std::cos(phi);
      float z = radius * std::sin(phi) * std::sin(theta);

      Vertex vertex;
      vertex.position = {x, y, z};
      vertex.normal = glm::normalize(vertex.position);
      vertex.uv = {u, v};
      vertex.tangent = glm::vec4(
          glm::normalize(glm::vec3(-std::sin(theta), 0.0f, std::cos(theta))),
          1.0f);
      vertex.tangent = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
      data.vertices.push_back(vertex);
    }
  }
  for (uint32_t stack = 0; stack < stacks; ++stack) {
    for (uint32_t slice = 0; slice < slices; ++slice) {

      uint32_t a = stack * (slices + 1) + slice;
      uint32_t b = a + 1;
      uint32_t c = a + slices + 1;
      uint32_t d = c + 1;

      data.indices.push_back(a);
      data.indices.push_back(c);
      data.indices.push_back(b);

      data.indices.push_back(b);
      data.indices.push_back(c);
      data.indices.push_back(d);
    }
  }
  return data;
}
}; // namespace GVK::shapes
