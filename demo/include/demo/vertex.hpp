#pragma once
#include <GVKRender/pipeline.hpp>
#include <glm/glm.hpp>

namespace GVK {


struct BasicVertex {
  glm::vec2 pos;
  glm::vec3 color;

  static VertexDescription getVertexDescription();
};

struct TexVertex {
  glm::vec2 pos;
  glm::vec2 texCoords;

  static VertexDescription getVertexDescription();
};

struct Vertex3D {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoords;
   
  static VertexDescription getVertexDescription();
};


} // namespace GVK
