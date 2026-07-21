#pragma once
#include <GVKCommon/data.hpp>
#include <GVKRender/pipeline.hpp>
#include <glm/glm.hpp>

namespace GVK {



template <typename T> VertexDescription getVertexDescription();
template<>
VertexDescription getVertexDescription<Vertex>();


} // namespace GVK
