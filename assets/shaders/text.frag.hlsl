struct Input
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

Texture2D font_texture : register(t0, space2);
SamplerState font_sampler : register(s0, space2);

float4 main(Input input) : SV_Target0
{
    return font_texture.Sample(font_sampler, input.uv);
}
