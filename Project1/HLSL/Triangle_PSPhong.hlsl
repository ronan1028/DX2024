#include "Triangle.hlsli"



// 像素着色器(3D)
float4 PS(VertexPosHWNormalTex pIn) : SV_Target
{
    // 标准化法向量
    pIn.NormalW = normalize(pIn.NormalW);

// 顶点指向眼睛的向量
float3 toEyeW =normalize(g_EyePosW - pIn.PosW);

// 初始化为0 
float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
float4 A = float4(0.0f, 0.0f, 0.0f, 0.0f);
float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
float factor = 0.0f;



    ComputePointLight(g_Material, g_PointLight, pIn.PosW, pIn.NormalW, toEyeW, A, D, S, factor);
    ambient += A;
    diffuse += D;
    spec += S;




float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
float4 litColor = texColor * (ambient + diffuse) + spec;
litColor.a = texColor.a * g_Material.Diffuse.a;

return litColor;
}