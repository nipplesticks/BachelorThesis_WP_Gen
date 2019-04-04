struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
};

float4 main(VS_OUTPUT input) : SV_Target
{
    input.color = float4(1, 1, 1, 1);
    //const float3 LIGHT_DIR = normalize(float3(1.0f, -1.0f, 1.0f));
    const float3 LIGHT_DIR = normalize(float3(0.0, 0.0, 1.0f));
    float4 ambient = input.color * 0.1f;
    float diffuseFactor = abs(min(dot(LIGHT_DIR, input.normal.xyz), 0.0f));
    float4 diffuse = input.color * diffuseFactor;
    return saturate(float4(ambient.rgb + diffuse.rgb, 1.0f));
}