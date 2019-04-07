struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUTPUT main(uint id : SV_VertexID)
{
    VS_OUTPUT output;
    if (id == 0) // Bottom left
    {
        output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 1.0f);
    }
    else if (id == 1) // Top left
    {
        output.position = float4(-1.0f, 1.0f, 0.0f, 1.0f);
        output.uv = float2(0.0f, 0.0f);
    }
    else if (id == 2) // Bottom right
    {
        output.position = float4(1.0f, -1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 1.0f);
    }
    else // Top right
    {
        output.position = float4(1.0f, 1.0f, 0.0f, 1.0f);
        output.uv = float2(1.0f, 0.0f);
    }
    return output;
}