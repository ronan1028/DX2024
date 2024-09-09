#include "Triangle.hlsli"



// 像素着色器(3D)
float4 PS(VertexPosHTex pIn) : SV_Target
{
    // 标准化法向量
return g_Tex.Sample(g_SamLinear, pIn.Tex);
}