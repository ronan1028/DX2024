#pragma once
#include "Transform.h"
#include "Mesh.h"
class GameApp;
class Object {
public:
	Object(GameApp* MainGame);
	Transform transform;
	GameApp* myGameApp;
	virtual void update(float deltaTime);
};
class SkyBox :public Object {
public:
	SkyBox(GameApp* MainGame);
	Mesh mesh;
};