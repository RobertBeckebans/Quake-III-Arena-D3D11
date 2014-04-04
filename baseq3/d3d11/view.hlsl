
cbuffer ViewData : register(b0)
{
    float4x4 Projection;
    float4x4 View;
    // x: DepthRangeMin, y: DepthRange
    float2 DepthRange; 
};

float4 DepthRangeHack(float4 vpos)
{
    // glDepthRange hack
    vpos.z = vpos.z / vpos.w;
    vpos.z = DepthRange.x + vpos.z * DepthRange.y;
    vpos.z *= vpos.w;

    return vpos;
}