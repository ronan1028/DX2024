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
	float speedr;//�����ٶ�
	float speedw;//�����ٶ�
	float time;//��컨��ʱ��
	float usedTime = 0;//�ѻ��ѵ�ʱ��
	float neededR;//Ŀ��������Ϣ
	float neededW;//Ŀ��������Ϣ
	float oldR;//��¼ԭ�������Ϣ
	float oldW;//��¼ԭ�������Ϣ
	Actor* rounded;
	Actor* nextTarget;
	Actor* roundingStar;
	DirectX::XMFLOAT3 axis;//��¼ԭ����Ĺ�ת��
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

