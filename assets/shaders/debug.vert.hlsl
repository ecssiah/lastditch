struct Input
{
    float3 position : TEXCOORD0;
    float3 color : TEXCOORD1;
};

struct Output
{
    float4 position : SV_Position;
    float3 color : TEXCOORD0;
};

cbuffer Camera : register(b0, space1)
{
    float4x4 projection;
    float4x4 view;
};

cbuffer Object : register(b1, space1)
{
    float4x4 model;
};

Output main(Input input)
{
    Output output;
    output.position = mul(projection, mul(view, mul(model, float4(input.position, 1.0))));
    output.color = input.color;
    return output;
}
