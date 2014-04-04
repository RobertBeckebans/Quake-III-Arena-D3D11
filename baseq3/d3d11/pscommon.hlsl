
cbuffer ViewDataPS : register(b0)
{
    float4 ClipPlane;
};

void ClipToPlane(float4 vpos)
{
    clip(dot(ClipPlane, vpos));
}
