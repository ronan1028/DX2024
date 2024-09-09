#include "Camera.h"
#include "GameApp.h"
#include "Mouse.h"
Camera::Camera(GameApp* mainGame):Object(mainGame)
{	
}

DirectX::XMMATRIX Camera::GetViewProjXM() const
{
	XMMATRIX v = transform.GetWorldToLocalMatrix();
	XMMATRIX p=XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ);
	return v * p;
}

DirectX::XMMATRIX Camera::GetViewXM() const
{
	return transform.GetWorldToLocalMatrix();
}

DirectX::XMMATRIX Camera::GetProjXM() const
{
	return XMMatrixPerspectiveFovLH(m_FovY,myGameApp->AspectRatio(), m_NearZ, m_FarZ);
}

void Camera::update(float deltaTime)
{
}



void FPCamera::update(float deltaTime)
{
	transform.position = followedObject->transform.position;
	Mouse::State lastMouseState = myGameApp->m_MouseTracker.GetLastState();
	Mouse::State mouseState = myGameApp->m_pMouse->GetState();
	if (mouseState.positionMode == Mouse::MODE_RELATIVE) {
		Pitch(mouseState.y * deltaTime * 2.5f);
		RotateY(mouseState.x * deltaTime * 2.5f);
	}
}

FPCamera::FPCamera(GameApp* mainGame,Object* followed):Camera(mainGame)
{
	followedObject = followed;
}

void FPCamera::Pitch(float rad)
{
	transform.rotation.x += rad;
	if (transform.rotation.x > XM_PI * 8 / 18)
		transform.rotation.x = XM_PI * 8 / 18;
	else if (transform.rotation.x < -XM_PI * 8 / 18)
		transform.rotation.x = -XM_PI * 8 / 18;

}

void FPCamera::RotateY(float rad)
{
	transform.rotation.y = XMScalarModAngle(transform.rotation.y + rad);
}

TPCamera::TPCamera(GameApp* mainGame, Object* followed) :Camera(mainGame)
{
	followedObject = followed;
}

void TPCamera::Pitch(float rad)
{

	transform.rotation.x += rad;
	if (transform.rotation.x < -XM_PI*9/18)
		transform.rotation.x = 0.0f;
	else if (transform.rotation.x > XM_PI *9/ 18)
		transform.rotation.x = XM_PI *9/ 18;

	transform.position = followedObject->transform.position;
	XMFLOAT3 forward = transform.GetForwardAxis();
	transform.position.x += forward.x * -distance;
	transform.position.y += forward .y* -distance;
	transform.position.z += forward.z * -distance;

}

void TPCamera::RotateY(float rad)
{
	transform.rotation.y = XMScalarModAngle(transform.rotation.y + rad);

	transform.position = followedObject->transform.position;
	XMFLOAT3 forward = transform.GetForwardAxis();
	transform.position.x += forward.x * -distance;
	transform.position.y += forward.y * -distance;
	transform.position.z += forward.z * -distance;
}

void TPCamera::Move(float dis)
{
	distance += dis;
	if (distance < minDis)
		distance = minDis;
	else if (distance > maxDis)
		distance = maxDis;
	transform.position = followedObject->transform.position;
	XMFLOAT3 forward = transform.GetForwardAxis();
	transform.position.x += forward.x * -distance;
	transform.position.y += forward.y * -distance;
	transform.position.z += forward.z * -distance;
}

void TPCamera::update(float deltaTime)
{
	Mouse::State mouseState = myGameApp->m_pMouse->GetState();
	Pitch(mouseState.y * deltaTime * 2.5f);
	RotateY(mouseState.x * deltaTime * 2.5f);
	Move(-mouseState.scrollWheelValue / 120 * 1.0f);
	myGameApp->m_pMouse->ResetScrollWheelValue();

}
