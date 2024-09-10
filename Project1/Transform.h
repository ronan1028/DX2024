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
	void LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up = { 0.0f, 1.0f, 0.0f })
	{
		using namespace DirectX;
		XMMATRIX View = XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&target), XMLoadFloat3(&up));
		XMMATRIX InvView = XMMatrixInverse(nullptr, View);
		DirectX::XMFLOAT4 quat;
		XMStoreFloat4(&quat, XMQuaternionRotationMatrix(InvView));
        CalculateRotationByQuat(quat, rotation);
	}

    void CalculateRotationByQuat(DirectX::XMFLOAT4 quat, DirectX::XMFLOAT3 rotation) {
        float sinX = 2 * (quat.w * quat.x - quat.y * quat.z);
        float sinY_cosX = 2 * (quat.w * quat.y + quat.x * quat.z);
        float cosY_cosX = 1 - 2 * (quat.x * quat.x + quat.y * quat.y);
        float sinZ_cosX = 2 * (quat.w * quat.z + quat.x * quat.y);
        float cosZ_cosX = 1 - 2 * (quat.x * quat.x + quat.z * quat.z);
        if (fabs(fabs(sinX) - 1.0f) < 1e-5f)
        {
            rotation.x = copysignf(DirectX::XM_PI / 2, sinX);
            rotation.y = 2.0f * atan2f(quat.y, quat.w);
            rotation.z = 0.0f;
        }
        else
        {
            rotation.x = asinf(sinX);
            rotation.y = atan2f(sinY_cosX, cosY_cosX);
            rotation.z = atan2f(sinZ_cosX, cosZ_cosX);
        }
	}
};