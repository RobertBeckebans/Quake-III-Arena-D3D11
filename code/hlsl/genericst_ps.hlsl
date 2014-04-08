#include "pscommon.h"

Texture2D Diffuse;

SamplerState Sampler : register(s0);

struct VS_PS_Data
{
	float4 Position : SV_POSITION;
	float2 AlbedoTC : TEXCOORD0;
    float4 Color : COLOR;
    float4 ViewPos : TEXCOORD2;
};

float4 main(VS_PS_Data input) : SV_TARGET
{
    ClipToPlane(input.ViewPos);

    return FinalColor(
        input.Color *
        Diffuse.Sample(Sampler, input.AlbedoTC));
}
