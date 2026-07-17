#include <GVKRender/window.hpp>

namespace GVK {

static void framebufferResizeCallback(GLFWwindow *handle, int width,
                                      int height) {
  auto window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(handle));
  window->framebufferResized = true;
}

Window::Window(uint32_t width, uint32_t height, const char *title,
               bool resizable) {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

  handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
  glfwSetWindowUserPointer(handle, this);
  glfwSetFramebufferSizeCallback(handle, framebufferResizeCallback);
}

Window::~Window() {
  glfwDestroyWindow(handle);
  glfwTerminate();
}


} // namespace GVK
