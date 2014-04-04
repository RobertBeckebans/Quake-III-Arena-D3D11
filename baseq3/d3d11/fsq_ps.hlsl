
struct VS_PS_Data
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};

float4 Main(VS_PS_Data input) : SV_TARGET
{
	return float4(0, 1, 1, 1);
}
