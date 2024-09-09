#include "Object.h"

Object::Object(GameApp* MainGame)
{
	myGameApp = MainGame;
}

void Object::update(float deltaTime)
{
}

SkyBox::SkyBox(GameApp* MainGame) :Object(MainGame)
{
}
