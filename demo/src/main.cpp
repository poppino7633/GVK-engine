#include <GVKAsset/image.hpp>
#include <GVKAsset/model.hpp>
#include <GVKAsset/shapes.hpp>
#include <GVKRender/render.hpp>
#include <GVKRender/state.hpp>
#include <demo/vertex.hpp>

#include <chrono>
#include <cstdlib>
#include <fstream>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stdexcept>

constexpr uint32_t WIDTH = 600;
constexpr uint32_t HEIGHT = 600;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

struct Matrices {
  glm::mat4 view;
  glm::mat4 proj;

  static vk::DescriptorSetLayoutBinding getBinding(uint32_t index) {
    return {.binding = index,
            .descriptorType = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = 1,
            .stageFlags = vk::ShaderStageFlagBits::eVertex};
  }
  static vk::DescriptorPoolSize getPoolSize(uint32_t count) {
    return {.type = vk::DescriptorType::eUniformBuffer,
            .descriptorCount = count};
  }
};

std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  std::vector<char> buffer(file.tellg());

  file.seekg(0, std::ios::beg);
  file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
  file.close();
  return buffer;
}

void updateMatricesUBO(GVK::BufferMapped &matricesBuffer,
                       vk::Extent2D swapChainExtent) {
  Matrices ubo{};
  ubo.view =
      glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj = glm::perspective(glm::radians(90.0f),
                              static_cast<float>(swapChainExtent.width) /
                                  static_cast<float>(swapChainExtent.height),
                              0.1f, 10.0f);
  ubo.proj[1][1] *= -1;
  memcpy(matricesBuffer.ptr, &ubo, sizeof(ubo));
}

void mainLoop(GVK::State &state, std::vector<GVK::FrameState> &frameStates,
              GVK::Window &window, const GVK::PipelineFamily &pipelineFamily,
              const GVK::Mesh &mesh, const GVK::Material &material) {
  std::cout << "Starting main loop" << std::endl;
  uint32_t frameIndex = 0;
  while (!glfwWindowShouldClose(window.handle)) {
    glfwPollEvents();
    GVK::FrameState &frameState = frameStates[frameIndex];
    updateMatricesUBO(frameState.ubo, state.swapChain.extent);
    GVK::beginFrame(state, frameState, window);
    frameState.commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, pipelineFamily.pipelineLayout, 0,
        *frameState.descriptorSet, nullptr);

    GVK::PipelineHandle pipelineHandle =
        GVK::getPipelineHandle(pipelineFamily, 0);

    GVK::bindPipeline(frameState, pipelineHandle);

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
                     currentTime - startTime)
                     .count();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, time * glm::radians(90.0f),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

    GVK::bindMaterial(frameState, pipelineHandle, state.materialSystem,
                      material);
    GVK::drawMesh(frameState, mesh, pipelineHandle, {model});

    GVK::endFrame(state, frameState, window);

    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
  }
  state.device.waitIdle();
}

int main() {
  try {
    GVK::Window window(WIDTH, HEIGHT, "GVK", true);
#ifdef NDEBUG
    const std::vector<char const *> validationLayers = {};
#else
    const std::vector<char const *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};
#endif
    
      std::vector<vk::DescriptorPoolSize> poolSizes = {
          Matrices::getPoolSize(MAX_FRAMES_IN_FLIGHT),
          GVK::Texture::getPoolSize(MAX_FRAMES_IN_FLIGHT)};

      GVK::State state(window.handle, validationLayers,
                       {vk::KHRSwapchainExtensionName}, poolSizes, 64, 100);

      std::vector<vk::DescriptorSetLayoutBinding> globalBindings = {
          Matrices::getBinding(0), GVK::Texture::getBinding(1)};

      vk::raii::DescriptorSetLayout globalLayout =
          GVK::createDescriptorSetLayout(state.device, globalBindings);

      GVK::PipelineFamily pipelineFamily = GVK::createPipelineFamily(
          state.device,
          {*globalLayout, *state.materialSystem.descriptorSetLayout});

      GVK::addGraphicsPipeline(
          state.device, pipelineFamily,
          GVK::createShaderModule(state.device, readFile("shaders/slang.spv")),
          GVK::getVertexDescription<GVK::Vertex>(), state.swapChain);

      GVK::MeshData meshData = GVK::shapes::generateUVSphere(2.0f, 32, 64);

      GVK::Mesh mesh =
          GVK::createMesh(state.device, state.physicalDevice, state.commandPool,
                          state.queue, meshData);
      GVK::Material material =
          GVK::createMaterial(state.device, state.materialSystem,
                              {.baseColor = {1.0f, 0.0f, 0.0f, 1.0f},
                               .metallic = 0.0f,
                               .roughness = 1.0f});

      GVK::Texture texture = GVK::createTexture(
          state.device, state.physicalDevice, state.queue, state.commandPool,
          GVK::loadImage("assets/textures/albedo.jpeg"));

      std::vector<GVK::FrameState> frameStates = GVK::createFrameStates(
          state, globalLayout,
          GVK::createUniformBuffers<Matrices>(
              state.device, state.physicalDevice, MAX_FRAMES_IN_FLIGHT),
          texture);

      mainLoop(state, frameStates, window, pipelineFamily, mesh, material);
    
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
