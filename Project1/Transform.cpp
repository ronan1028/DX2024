#include "Transform.h"
using namespace DirectX;
Transform::Transform()
{
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}
XMMATRIX Transform::GetLocalToWorldMatrix() const
{
	XMVECTOR scaleVec = XMLoadFloat3(&scale);
	XMVECTOR rotationVec = XMLoadFloat3(&rotation);
	XMVECTOR positionVec = XMLoadFloat3(&position);
	XMMATRIX matrix = XMMatrixScalingFromVector(scaleVec) * XMMatrixRotationRollPitchYawFromVector(rotationVec) * XMMatrixTranslationFromVector(positionVec);
	return matrix;
}
XMMATRIX Transform::GetWorldToLocalMatrix() const
{
	XMMATRIX InvWorld = XMMatrixInverse(nullptr, GetLocalToWorldMatrix());
	return InvWorld;
}
DirectX::XMFLOAT3 Transform::GetForwardAxis() const
{
	XMMATRIX R = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, R.r[2]);
	return forward;
}
