struct Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

Texture2DArray actor_texture : register(t0, space2);
SamplerState actor_sampler : register(s0, space2);
cbuffer Layer : register(b0, space3)
{
    int texture_layer;
    float3 padding;
};

float4 main(Input input) : SV_Target0
{
    return actor_texture.Sample(actor_sampler, float3(input.uv, texture_layer));
}
