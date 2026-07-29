struct VSInput {
  float3 inPos;
  float3 inColor;
  float2 inTexCoords;
};

struct UniformBuffer {
  float4x4 view;
  float4x4 proj;
};
struct MaterialConstants {
    float4 baseColor;
    float metallic;
    float roughness;
};

[[vk::binding(0, 0)]]
ConstantBuffer<UniformBuffer> ubo;
[[vk::binding(0, 1)]]
ConstantBuffer<MaterialConstants> material;

struct PushConstants
{
    float4x4 model;
};
[[vk::push_constant]]
PushConstants pc;

[[vk::binding(1, 0)]]
Sampler2D texture;

struct VSOutput {
  float4 pos : SV_Position;
  float3 color;
  float2 texCoords;
};

[shader("vertex")]
VSOutput vertMain(VSInput input) {
  VSOutput output;
  output.pos = mul(
      ubo.proj, mul(ubo.view, mul(pc.model, float4(input.inPos, 1.0))));
  output.color = input.inColor; 
  output.texCoords = input.inTexCoords;
  return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
  return material.baseColor * texture.Sample(vertIn.texCoords);
}
