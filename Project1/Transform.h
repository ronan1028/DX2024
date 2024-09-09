#pragma once
#include <DirectXMath.h>
class Transform {
public:
	Transform();
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	DirectX::XMMATRIX GetLocalToWorldMatrix() const;
	DirectX::XMFLOAT3 GetForwardAxis() const;
	DirectX::XMMATRIX GetWorldToLocalMatrix() const;
};