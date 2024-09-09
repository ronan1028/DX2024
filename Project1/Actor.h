#pragma once
#include "Object.h"
#include "Mesh.h"
class Actor;
struct SelfRotating
{
	float angel = 0.0f;
	float speed = 0.0f;
	DirectX::XMFLOAT3 axis;
};
struct Rotating {
	float angel = 0.0f;
	float speed = 0.0f;
	float r;
	DirectX::XMVECTOR FVector;
	DirectX::XMFLOAT3 axis;
	Actor* target;
};
struct Track {
	float speedr;//变轨的速度
	float speedw;//变轨的速度
	float time;//变轨花的时间
	float usedTime = 0;//已花费的时间
	float neededR;//目标轨道的信息
	float neededW;//目标轨道的信息
	float oldR;//记录原轨道的信息
	float oldW;//记录原轨道的信息
	Actor* rounded;
	Actor* nextTarget;
	Actor* roundingStar;
	DirectX::XMFLOAT3 axis;//记录原轨道的公转轴
};

class Actor :public Object {

public:
	enum class State
	{
		rotate,
		transfer,
		landTo,
		landing,
		raising,
	};
	Mesh mesh;
	SelfRotating selfRotating;
	Rotating rotating;
	Track track;
	State mState = State::rotate;
	Actor(GameApp* myGameApp);
	void selfRotate(float deltatime);
	void Rotating(float deltatime);
	void GetVectorF();
	void CalculatingTrack(Actor* target, float distance, float wSpeed);
	virtual void update(float deltaTime);
	void StartLand(float distance);
	void StartRaise();
};

