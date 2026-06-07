#pragma once
#include <GVK/instance.hpp>
#include <GVK/window.hpp>

namespace GVK {
  vk::raii::SurfaceKHR createSurface(const vk::raii::Instance& instance, GLFWwindow* window);

}
