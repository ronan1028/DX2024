#pragma once
#include <DirectXMath.h>
struct PointLight
{

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	DirectX::XMFLOAT3 position;
	float range;

	DirectX::XMFLOAT3 att;
	float pad;
};

struct Material
{

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	DirectX::XMFLOAT4 reflect;
};

struct PSConstantBuffer
{
	PointLight pointLight;
	Material material;
};