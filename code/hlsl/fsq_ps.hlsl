#include "pscommon.h"

cbuffer CBuf : register(b1)
{
    float3 Color;
};

Texture2D Diffuse;

SamplerState Sampler : register(s0);

struct VS_PS_Data
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

float4 main(VS_PS_Data input) : SV_TARGET
{
    return FinalColor(float4(Color, 1) * Diffuse.Sample(Sampler, input.TexCoord));
}
