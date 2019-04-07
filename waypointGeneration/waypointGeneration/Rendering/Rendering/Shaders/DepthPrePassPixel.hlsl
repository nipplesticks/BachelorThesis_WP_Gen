struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : WORLDPOS;
    float4 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 main(VS_OUTPUT input) : SV_Target0
{
    return input.worldPosition;
}