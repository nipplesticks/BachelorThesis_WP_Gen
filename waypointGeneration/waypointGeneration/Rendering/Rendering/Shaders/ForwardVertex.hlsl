cbuffer OBJECT_DATA : register(b0)
{
    float4x4    WorldMatrix;
    float4      ObjectColor;
}

cbuffer CAMERA_DATA : register(b1)
{
    float4x4 ViewProjectionMatrix;
    float4 WorldPosition;
}

struct VS_INPUT
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = mul(WorldMatrix, input.position);
    output.position = mul(ViewProjectionMatrix, output.position);
    output.normal = float4(normalize(mul(WorldMatrix, input.normal).xyz),0.0f);
    output.color = ObjectColor;
    output.uv = input.uv;
    return output;
}