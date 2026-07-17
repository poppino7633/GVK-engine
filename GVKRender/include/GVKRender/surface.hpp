#pragma once
#include <GVKRender/instance.hpp>
#include <GVKRender/window.hpp>

namespace GVK {
  vk::raii::SurfaceKHR createSurface(const vk::raii::Instance& instance, GLFWwindow* window);

}
