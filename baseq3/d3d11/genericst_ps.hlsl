#include "pscommon.hlsl"

Texture2D Diffuse;

SamplerState Sampler : register(s0);

struct VS_PS_Data
{
	float4 Position : SV_POSITION;
	float2 AlbedoTC : TEXCOORD0;
    float4 Color : COLOR;
};

float4 Main(VS_PS_Data input) : SV_TARGET
{
    return 
        input.Color *
        Diffuse.Sample(Sampler, input.AlbedoTC);
}
