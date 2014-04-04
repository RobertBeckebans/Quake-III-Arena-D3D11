#include "view.hlsl"

struct VS_Data
{
	float3 Position : POSITION;
	float2 AlbedoTC : TEXCOORD0;
    float4 Color : COLOR;
};

struct VS_PS_Data
{
	float4 Position : SV_POSITION;
	float2 AlbedoTC : TEXCOORD0;
    float4 Color : COLOR;
};

VS_PS_Data Main(VS_Data input)
{
	VS_PS_Data output;

	float4 viewPos = mul(View, float4(input.Position, 1));
    float4 sPos = mul(Projection, viewPos);
	output.Position = sPos;
    output.AlbedoTC = input.AlbedoTC;
	output.Color = input.Color;

	return output;
}
