static const float PI = 3.14159274;
struct VSInput {
  float3 inPos;
  float3 inNormal;
  float4 inTangent;
  float2 inTexCoords;
  float3 inColor;
};

struct UniformBuffer {
  float4x4 view;
  float4x4 proj;
  float4 viewPos;
  float4 lightPos;
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
[[vk::binding(3, 1)]]
Sampler2D metallicRoughnessMap;

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
  float3 color;
  float3x3 TBN;
};

float diffuseBRDF() { return 1.0 / PI; }

float X(float x) {
  if (x >= 0)
    return 1;
  else
    return 0;
}

float specularBRDF(float3 N, float3 H, float3 L, float3 V, float alpha) {
  float NdotL = dot(N, L);
  float NdotV = dot(N, V);
  float NdotH = dot(N, H);
  float term1 =
      X(dot(H, L)) /
      (abs(NdotL) + sqrt(alpha * alpha + (1 - alpha * alpha) * NdotL * NdotL));

  float term2 =
      X(dot(H, V)) /
      (abs(NdotV) + sqrt(alpha * alpha + (1 - alpha * alpha) * NdotV * NdotV));

  float visibility = term1 * term2;

  float denom = (NdotH * NdotH * (alpha * alpha - 1) + 1);

  float D = (alpha * alpha * X(NdotH)) / (PI * denom * denom);
  return visibility * D;
}

float fresnel(float f0, float3 V, float3 H) {
  return f0 + (1 - f0) * pow(1 - abs(dot(V, H)), 5);
}
float3 fresnel(float3 f0, float3 V, float3 H) {
  return f0 + (1 - f0) * pow(1 - abs(dot(V, H)), 5);
}

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
  output.color = input.inColor;

  return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {

  float3 baseColor = material.baseColor.rgb *
                     albedo.Sample(vertIn.texCoords).rgb * vertIn.color;

  float roughness =
      material.roughness * metallicRoughnessMap.Sample(vertIn.texCoords).g;
  float metallic =
      material.metallic * metallicRoughnessMap.Sample(vertIn.texCoords).b;

  float3 tangentNormal = normalMap.Sample(vertIn.texCoords).rgb;
  tangentNormal = tangentNormal * 2.0f - 1.0f;

  float3 N = normalize(mul(tangentNormal, vertIn.TBN));
  float3 L = normalize(ubo.lightPos.xyz - vertIn.worldPos.xyz);
  float3 V = normalize(ubo.viewPos.xyz - vertIn.worldPos.xyz);
  float3 H = normalize(V + L);

  float lightIntensity = 10.0f;

  float3 metalBRDF = specularBRDF(N, H, L, V, roughness*roughness) * fresnel(baseColor, V, H);
  float3 dielectricBRDF = lerp(diffuseBRDF() * baseColor, specularBRDF(N, H, L, V, roughness * roughness), fresnel(0.04, V, H));
  float3 materialColor = lerp(dielectricBRDF, metalBRDF, metallic) * max(dot(N, L), 0.0f) * 5.0f;

  float3 ambientColor = float3(0.0f, 0.1f, 0.1f);
  float ambientFactor = 0.0f;
  float3 color = materialColor + (ambientColor * ambientFactor);

  // return float4((tangentNormal - float3(0, 0, 1)) * 0.5 + 0.5, 1);
  // return float4(normalize(mul(vertIn.TBN, float3(0,0,1))) * 0.5 + 0.5, 1);
  // return float4(worldNormal * 0.5f + 0.5f, 1);
  return float4(color, albedo.Sample(vertIn.texCoords).a);
}
