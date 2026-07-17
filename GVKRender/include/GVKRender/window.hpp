#pragma once
#include <cstdint>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace GVK {


struct Window {
  GLFWwindow* handle;
  bool framebufferResized = false;

  Window(uint32_t width, uint32_t height, const char *title,
               bool resizable = true);

  ~Window();

};


} // namespace GVK
