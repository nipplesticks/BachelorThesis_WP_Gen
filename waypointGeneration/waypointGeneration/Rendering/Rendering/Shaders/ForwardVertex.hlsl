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
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float4 color : COLOR;
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    //output.position = mul(WorldMatrix, input.position);
    //output.position = mul(ViewProjectionMatrix, output.position);
    output.position = input.position;
    output.normal = input.normal;
    //output.normal = mul(WorldMatrix, input.normal);


    output.color = ObjectColor;

    return output;
}