SamplerState defaultSampler : register(s1);
Texture2D albedoTexture : register(t0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_Target
{
    const float3 LIGHT_DIR = normalize(float3(1.0f, -1.0f, 1.0f));
    float4 albedo = albedoTexture.Sample(defaultSampler, input.uv) * input.color;
    float4 ambient = 0.3f * albedo;
    float diffuseFactor = abs(min(dot(LIGHT_DIR, input.normal.xyz), 0.0f));
    float4 diffuse = diffuseFactor * albedo;
    return saturate(ambient + diffuse);
}