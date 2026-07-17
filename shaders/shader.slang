struct VSInput {
  float3 inPos;
  float3 inColor;
  float2 inTexCoords;
};

struct UniformBuffer {
  float4x4 model;
  float4x4 view;
  float4x4 proj;
};
ConstantBuffer<UniformBuffer> ubo;

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
      ubo.proj, mul(ubo.view, mul(ubo.model, float4(input.inPos, 1.0))));
  output.color = input.inColor; 
  output.texCoords = input.inTexCoords;
  return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
  return texture.Sample(vertIn.texCoords);
}
