#include <GVKAsset/model.hpp>
#include <stdexcept>
#include <iostream>
#include <tinyobjloader/tiny_obj_loader.h>

namespace GVK {
MeshData loadModel(std::string path) {
  MeshData mesh;
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str())) {
    throw std::runtime_error(err);
  }

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex{};

      vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                         attrib.vertices[3 * index.vertex_index + 1],
                         attrib.vertices[3 * index.vertex_index + 2]};

      vertex.uv = {attrib.texcoords[2 * index.texcoord_index + 0],
                   1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

      auto [it, inserted] = uniqueVertices.insert(
          {vertex, static_cast<uint32_t>(mesh.vertices.size())});
      if (inserted) {
        mesh.vertices.push_back(vertex);
      }
      mesh.indices.push_back(it->second);
    }
  }
  std::cout << mesh.vertices.size() << " " << mesh.indices.size() << std::endl;
  return mesh;
}
}; // namespace GVK
