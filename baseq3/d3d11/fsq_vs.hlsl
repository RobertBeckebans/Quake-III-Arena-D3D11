#include "vscommon.hlsl"

struct VS_Data
{
	float2 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct VS_PS_Data
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VS_PS_Data Main(VS_Data input)
{
	VS_PS_Data output;

	float4 viewPos = mul(View, float4(input.Position, 0, 1));
    float4 sPos = mul(Projection, viewPos);
	output.Position = float4(sPos.xy, 0, 1);
    //output.Position = float4(input.Position, 0, 1);
	output.TexCoord = input.TexCoord;

	return output;
}
