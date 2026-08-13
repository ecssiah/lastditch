struct Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    nointerpolation uint block_type : TEXCOORD1;
};

Texture2DArray block_texture : register(t0, space2);
SamplerState block_sampler : register(s0, space2);

float4 main(Input input) : SV_Target0
{
    return block_texture.Sample(block_sampler, float3(input.uv, input.block_type));
}
