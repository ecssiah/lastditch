struct Input
{
    float3 position : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

struct Output
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

cbuffer Camera : register(b0, space1)
{
    float4x4 projection; float4x4 view;
};

cbuffer Object : register(b1, space1)
{
    float4x4 model;
};

Output main(Input input)
{
    Output output;
    output.position = mul(projection, mul(view, mul(model, float4(input.position, 1.0))));
    output.uv = input.uv;
    return output;
}
