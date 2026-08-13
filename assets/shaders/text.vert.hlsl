struct Input
{
    float2 position : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

struct Output
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

cbuffer Projection : register(b0, space1)
{
    float4x4 projection;
};

Output main(Input input)
{
    Output output;
    output.position = mul(projection, float4(input.position, 0.0, 1.0));
    output.uv = input.uv;
    return output;
}
