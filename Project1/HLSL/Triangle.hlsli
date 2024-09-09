struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Att;
    float Pad;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Reflect;
};
Texture2D g_Tex : register(t0);
Texture2D g_Tex2 : register(t1);
SamplerState g_SamLinear : register(s0);


cbuffer VSConstantBuffer : register(b0)
{
    matrix g_World;
    matrix g_View;
    matrix g_Proj;
    matrix g_WorldInvTranspose;
}

cbuffer PSConstantBuffer : register(b1)
{
    PointLight g_PointLight;
    Material g_Material;
}

cbuffer EyePosition:register(b2)
{
    float3 g_EyePosW;
    float g_pad;
}
struct VertexPosNormalTex
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
};

struct VertexPosTex
{
    float3 PosL : POSITION;
    float2 Tex : TEXCOORD;
};

struct VertexPosHWNormalTex
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL; 
    float2 Tex : TEXCOORD;
};

struct VertexPosHTex
{
    float4 PosH : SV_POSITION;
    float2 Tex : TEXCOORD;
};

void ComputePointLight(Material mat, PointLight L, float3 pos, float3 normal, float3 toEye,
    out float4 ambient, out float4 diffuse, out float4 spec, out float DF)
{
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
   
    float3 lightVec = L.Position - pos;

    float d = length(lightVec);

    if (d > L.Range)
        return;

    lightVec /= d;

    ambient = mat.Ambient * L.Ambient;

    float diffuseFactor = dot(lightVec, normal);

    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }

    float att = 1.0f / dot(L.Att, float3(1.0f, d, d * d));

    spec *= att;
    DF = diffuseFactor;
}