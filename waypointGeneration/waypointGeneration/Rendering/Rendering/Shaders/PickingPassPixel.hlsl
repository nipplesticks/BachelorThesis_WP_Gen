SamplerState defaultSampler : register(s1);
Texture2D WorldTexture : register(t0);
RWTexture2D<float3> OutputPosition : register(u1);


cbuffer MOUSE_SCREEN_POS : register(b0)
{
    uint4 MousePosition;
}

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

void main(VS_OUTPUT input)
{
    uint2 pixel = uint2(input.position.xy);
    if (pixel.x == MousePosition.x && pixel.y == MousePosition.y)
    {
        float3 worldPosition = WorldTexture.Sample(defaultSampler, input.uv).rgb;
        OutputPosition[uint2(0, 0)] = worldPosition;
    }  
}