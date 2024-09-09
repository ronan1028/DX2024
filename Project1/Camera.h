#pragma once
#include "Object.h"
using namespace DirectX;
class Camera :public Object {
public:
	Camera(GameApp* mainGame);
	float m_NearZ = 0.1f;
	float m_FarZ = 10000.0f;
	float m_Aspect = 0.0f;
	float m_FovY = XM_PIDIV4;
	DirectX::XMMATRIX GetViewProjXM() const;
	DirectX::XMMATRIX GetViewXM() const;
	DirectX::XMMATRIX GetProjXM()const;
	virtual void update(float deltaTime);
};

class TPCamera :public Camera {
public:
	TPCamera(GameApp* mainGame, Object* followed);
	Object* followedObject;
	float distance;
	float minDis = 1.0f;
	float maxDis = 1000.0f;
	void Pitch(float rad);
	void RotateY(float rad);
	void Move(float dis);
	virtual void update(float deltaTime);
};

class FPCamera :public Camera {
public:
	FPCamera(GameApp* mainGame, Object* followed);
	Object* followedObject;

	void Pitch(float rad);
	void RotateY(float rad);
	virtual void update(float deltaTime);
};