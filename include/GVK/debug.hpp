#pragma once
#include <GVK/instance.hpp>

namespace GVK {
  vk::raii::DebugUtilsMessengerEXT createDebugMessenger(const vk::raii::Instance &instance, void *userData);
}
