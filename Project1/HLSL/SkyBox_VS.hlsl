#include "Triangle.hlsli"

VertexPosHTex VS(VertexPosNormalTex vIn)
{
    VertexPosHTex vOut;
    matrix viewProj = mul(g_View, g_Proj);
    float4 posH = mul(float4(vIn.PosL, 1.0f), viewProj);
    vOut.PosH = posH.xyww;
    vOut.Tex = vIn.Tex;
    return vOut;
}
