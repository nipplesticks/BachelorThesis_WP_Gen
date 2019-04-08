SamplerState defaultSampler : register(s1);
Texture2D albedoTexture : register(t0);

cbuffer CAMERA_DATA : register(b1)
{
    float4x4 ViewProjectionMatrix;
    float4 WorldPosition;
}

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : WORLDPOS;
    float4 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_Target
{
    const float3 LIGHT_DIR = normalize(float3(-1.0f, -1.f, -1.0f));
    const float3 LIGHT_COLOR = float3(99, 37, 33) / 256;

    float4 albedo = albedoTexture.Sample(defaultSampler, input.uv) * input.color * (float4(LIGHT_COLOR, 1.0f) + float4(1,1,1,0));

    float4 ambient = 0.3f * albedo;

    float diffuseFactor = abs(min(dot(LIGHT_DIR, input.normal.xyz), 0.0f));

    float3 viewer = normalize(WorldPosition.xyz - input.worldPosition.xyz);
    float3 objectToLight = LIGHT_DIR * -1;
    float3 halfDir = normalize(objectToLight + viewer);
    float specular = pow(max(dot(input.normal.xyz, halfDir), 0.0f), 512.0f);
    
    float4 diffuse = diffuseFactor * albedo;
    float4 specColor = specular * (float4(LIGHT_COLOR, 1.0f) + float4(0.5, 0.5, 0.5, 0));

    //return saturate(float4(specColor.rgb, 1.0f));
    return saturate(ambient + diffuse + specColor);
}