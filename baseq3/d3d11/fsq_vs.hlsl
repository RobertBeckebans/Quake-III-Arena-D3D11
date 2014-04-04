
cbuffer ViewData : register(b0)
{
    float4x4 Projection;
    float4x4 View;
};

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

	output.Position = mul(Projection, viewPos);
	output.TexCoord = input.TexCoord;

	return output;
}
