#include "Triangle.hlsli"

VertexPosHTex VS(VertexPosNormalTex vIn)
{
    VertexPosHTex vOut;
    matrix viewProj = mul(g_View, g_Proj);
    float4 posW = mul(float4(vIn.PosL, 1.0f), g_World);
    vOut.PosH = mul(posW, viewProj);
    vOut.Tex = vIn.Tex;
    return vOut;
}
