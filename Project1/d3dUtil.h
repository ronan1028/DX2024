#pragma once
#include <d3d11_1.h>			// 已包含Windows.h
#include <DirectXCollision.h>	// 已包含DirectXMath.h
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <vector>
#include <string>



HRESULT CreateShaderFromFile(
	const WCHAR* csoFileNameInOut,
	const WCHAR* hlslFileName,
	LPCSTR entryPoint,
	LPCSTR shaderModel,
	ID3DBlob** ppBlobOut);
inline DirectX::XMMATRIX XM_CALLCONV InverseTranspose(DirectX::FXMMATRIX M)
{
	using namespace DirectX;

	XMMATRIX A = M;
	A.r[3] = g_XMIdentityR3;
	return XMMatrixTranspose(XMMatrixInverse(nullptr, A));
}