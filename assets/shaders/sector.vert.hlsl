struct Input
{
    uint vertex : TEXCOORD0;
    uint face : TEXCOORD1;
};

struct Output
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    nointerpolation uint block_type : TEXCOORD1;
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

float3 uv_axis(uint direction, bool second)
{
    if (direction == 0) return second ? float3(0,0,1) : float3(0,1,0);
    if (direction == 1) return second ? float3(0,0,1) : float3(0,-1,0);
    if (direction == 2) return second ? float3(0,0,1) : float3(-1,0,0);
    if (direction == 3) return second ? float3(0,0,1) : float3(1,0,0);
    if (direction == 4) return second ? float3(0,1,0) : float3(1,0,0);
    return second ? float3(0,-1,0) : float3(1,0,0);
}

Output main(Input input)
{
    const float3 coordinate = float3(
        (input.vertex >> 0u) & 63u,
        (input.vertex >> 6u) & 63u,
        (input.vertex >> 12u) & 255u
    );

    const uint direction = (input.face >> 8u) & 7u;

    Output output;
    output.position = mul(projection, mul(view, mul(model, float4(coordinate, 1.0))));
    output.uv = float2(dot(coordinate, uv_axis(direction, false)), dot(coordinate, uv_axis(direction, true)));
    output.block_type = (input.face & 255u) - 1u;
    return output;
}
