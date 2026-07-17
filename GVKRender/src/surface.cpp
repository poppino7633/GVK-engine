#include <GVKRender/surface.hpp>

namespace GVK {
  vk::raii::SurfaceKHR createSurface(const vk::raii::Instance& instance, GLFWwindow* window) {
  VkSurfaceKHR _surface;
  if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
    throw std::runtime_error("Failed to create window surface!");
  }
  return vk::raii::SurfaceKHR(instance, _surface);
}
} // namespace GVK
