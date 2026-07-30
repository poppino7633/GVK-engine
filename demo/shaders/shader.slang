struct VSInput {
  float3 inPos;
  float3 inNormal;
  float4 inTangent;
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
[[vk::binding(1, 1)]]
Sampler2D albedo;
[[vk::binding(2, 1)]]
Sampler2D normalMap;

struct PushConstants {
  float4x4 model;
  float4x4 normal;
};
[[vk::push_constant]]
PushConstants pc;

struct VSOutput {
  float4 pos : SV_Position;
  float4 worldPos;
  float2 texCoords;
  float3 tg;
  float3 n;
  float3x3 TBN;
};

[shader("vertex")]
VSOutput vertMain(VSInput input) {
  VSOutput output;
  output.pos =
      mul(ubo.proj, mul(ubo.view, mul(pc.model, float4(input.inPos, 1.0))));

  output.texCoords = input.inTexCoords;
  output.worldPos = mul(pc.model, float4(input.inPos, 1.0f));

  float3 T = normalize(mul(pc.normal, float4(input.inTangent.xyz, 0)).xyz);
  float3 N = normalize(mul(pc.normal, float4(input.inNormal, 0)).xyz);
  T = normalize(T - dot(T, N) * N);
  float3 B = cross(N, T) * input.inTangent.w;
  output.TBN = float3x3(T, B, N);
  output.n = mul(output.TBN, input.inNormal);
  output.tg = normalize(input.inTangent.xyz);
  output.tg = T;

  return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {

  float3 baseColor =
      material.baseColor.rgb * albedo.Sample(vertIn.texCoords).rgb;

  float3 tangentNormal = normalMap.Sample(vertIn.texCoords).rgb;
  tangentNormal = tangentNormal * 2.0f - 1.0f;

  float3 worldNormal = normalize(mul(tangentNormal, vertIn.TBN));

  float3 lightPos = float3(0.0, 2.0, 3.0);
  float3 lightDir = normalize(lightPos - vertIn.worldPos.xyz);

  float NdotL = max(dot(worldNormal, lightDir), 0.0);

  float ambientStrength = 0.1;
  float3 ambient = ambientStrength * baseColor;

  float3 diffuse = baseColor * NdotL;
  float3 color = ambient + diffuse;

  // return float4((tangentNormal - float3(0, 0, 1)) * 0.5 + 0.5, 1);
  // return float4(normalize(mul(vertIn.TBN, float3(0,0,1))) * 0.5 + 0.5, 1);
  // return float4(worldNormal * 0.5f + 0.5f, 1);
  return float4(color, 1.0);
}
