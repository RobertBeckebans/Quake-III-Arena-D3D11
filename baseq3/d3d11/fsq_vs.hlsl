
struct VS_Data
{
	float2 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct VS_PS_Data
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};

VS_PS_Data Main(VS_Data input)
{
	VS_PS_Data output;

	output.Position = float4(input.Position, 0, 1);
	output.TexCoord = input.TexCoord;

	return output;
}
