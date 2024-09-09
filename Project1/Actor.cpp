#include "Actor.h"
using namespace DirectX;
Actor::Actor(GameApp* mainGame):Object(mainGame)
{
}

void Actor::selfRotate(float deltatime)
{
    selfRotating.angel = selfRotating.angel + deltatime * selfRotating.speed;
    //限制在0-2pi
    if (selfRotating.angel > XM_2PI)
        selfRotating.angel = selfRotating.angel - XM_2PI;
    else if (selfRotating.angel < 0)
        selfRotating.angel = XM_2PI + selfRotating.angel;
	XMVECTOR rotationVec = XMLoadFloat3(&transform.rotation);
	
	XMMATRIX R =XMMatrixRotationAxis(XMLoadFloat3(&selfRotating.axis),selfRotating.angel);
    XMFLOAT4X4 rotationMatrix;
    XMStoreFloat4x4(&rotationMatrix, R);
    float c = sqrtf(1.0f - rotationMatrix(2, 1) * rotationMatrix(2, 1));
    // 防止r[2][1]出现大于1的情况
    if (isnan(c))
        c = 0.0f;
    XMFLOAT3 rotation;
    rotation.z = atan2f(rotationMatrix(0, 1), rotationMatrix(1, 1));
    rotation.x = atan2f(-rotationMatrix(2, 1), c);
    rotation.y = atan2f(rotationMatrix(2, 0), rotationMatrix(2, 2));
    transform.rotation = rotation;
}

void Actor::Rotating(float deltatime)
{
    if (rotating.target == nullptr)
        return;
    rotating.angel= rotating.angel + deltatime * rotating.speed;
    //限制在0-2pi
    if (rotating.angel > XM_2PI)
        rotating.angel = rotating.angel - XM_2PI;
    else if (rotating.angel < 0)
        rotating.angel = XM_2PI + rotating.angel;
    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&rotating.axis), rotating.angel);
    XMVECTOR p = XMVector3Transform(rotating.FVector, R);
    p = p * rotating.r;
    XMVECTOR vt = XMLoadFloat3(&rotating.target->transform.position);
    
    XMStoreFloat3(&transform.position,p+vt);


}

void Actor::GetVectorF()
{
    XMFLOAT3 v1(1, 0, 0);
    XMVECTOR v = XMVector3Cross(XMLoadFloat3(&v1), XMLoadFloat3(&rotating.axis));
    v = XMVector3Normalize(v);
    rotating.FVector = v;
}

void Actor::CalculatingTrack(Actor* target,float distance,float wSpeed)
{
    //计算目标行星的周转平面和飞行器周转平面相交形成的方向向量，使用两者的公转轴叉乘得出的结果
    XMVECTOR intersection;
    intersection=XMVector3Cross(XMLoadFloat3(&(track.roundingStar->rotating.axis)), XMLoadFloat3(&rotating.axis));
    //计算目标行星周转到此位置的标准弧度
    intersection = XMVector3Normalize(intersection);
    XMVECTOR vAngel=XMVector3AngleBetweenNormals(track.roundingStar->rotating.FVector, intersection);
    float rad = XMVectorGetX(vAngel);
    //由于存在逆时针旋转和顺时针旋转得出的角度的两种可能，需对此角度进行测试是否能在经过旋转后得到相交向量
    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&track.roundingStar->rotating.axis), rad);
    XMVECTOR possibleIntersection= XMVector3Transform(track.roundingStar->rotating.FVector,R);
    XMVECTOR vDot = XMVector3Dot(possibleIntersection, intersection);
    float dotResult = XMVectorGetX(vDot);
    if (1 - dotResult > 0.0001f) {
        rad = XM_2PI - rad;
    }
    //计算目标行星周转至重合轨道的所需时间
    float deltaAngel = rad - track.roundingStar->rotating.angel;
    float speed = track.roundingStar->rotating.speed;
    if (deltaAngel * speed > 0)
    {
        deltaAngel = abs(deltaAngel);
        speed = abs(speed);
        if (deltaAngel < 3*XM_PIDIV4) //因为相交线为相反的两个向量，我们尽量让deltaAngel的角度不要过小，保证足够时间变轨
        {
            deltaAngel += XM_PI;
            intersection = -intersection; //同时也要取反相交的向量
        }
    }
    else if (deltaAngel * speed < 0) 
    {
        deltaAngel = XM_2PI - abs(deltaAngel);
        speed = abs(speed);
        if (deltaAngel < 3 * XM_PIDIV4) //同上
        {
            deltaAngel += XM_PI;
            intersection = -intersection; //同时也要取反相交的向量
        }
    }
    float time = deltaAngel / speed;
    //如果从月球返回地球则飞行轨道的向量应该和月球轨道所对应的向量相反
    if (track.roundingStar == track.rounded) {
        intersection = -intersection;
    }
    //计算飞行器轨道的弧度
    vAngel = XMVector3AngleBetweenNormals(rotating.FVector, intersection);
  

    rad = XMVectorGetX(vAngel);
    //由于存在逆时针旋转和顺时针旋转得出的角度的两种可能，需对此角度进行测试是否能在经过旋转后得到相交向量
    R = XMMatrixRotationAxis(XMLoadFloat3(&rotating.axis), rad);
    possibleIntersection = XMVector3Transform(rotating.FVector, R);
    vDot = XMVector3Dot(possibleIntersection, intersection);
    dotResult = XMVectorGetX(vDot);
    if (1 - dotResult > 0.0001f) {
        rad = XM_2PI - rad;
    }
    deltaAngel = rad - rotating.angel;
    speed = rotating.speed;
    if (deltaAngel * speed > 0) {
        deltaAngel = abs(deltaAngel);
    }
    else if (deltaAngel * speed < 0)
    {
        deltaAngel = XM_2PI - abs(deltaAngel);
    }
    float w = deltaAngel / time;//计算变轨的角速度
    float r= (track.roundingStar->rotating.r + distance-rotating.r) / time;//计算轨道半径的变化速率
    //注册轨道信息
    

    track.time = time;
    track.speedw = w;
    track.speedr = r;
    track.nextTarget = track.rounded;
    track.rounded = target;
    track.neededW = wSpeed;
    track.neededR = distance;
    rotating.speed = w;
    
    mState = State::transfer;
}

void Actor::update(float deltaTime)
{
    if (mState == State::transfer)
    {
        rotating.r += track.speedr * deltaTime;
        track.usedTime += deltaTime;
        if (track.usedTime > track.time) {
            track.usedTime = 0;
            rotating.r = track.neededR;
            rotating.speed = track.neededW;
            rotating.target = track.rounded;
            mState = State::rotate;
        }
    }
    else if (mState == State::landing) {
        rotating.r = rotating.r - 0.2f * deltaTime;
        if (rotating.r < track.neededR) {
            rotating.r = track.neededR;
            rotating.axis = rotating.target->selfRotating.axis;
            rotating.speed = rotating.target->selfRotating.speed;
            GetVectorF();
            mState = State::landTo;
        }
    }
    else if (mState == State::raising) {
        rotating.r = rotating.r + 0.2f*deltaTime;
        if (rotating.r > track.oldR) {
            rotating.r = track.oldR;
            mState = State::rotate;
        }
    }
    selfRotate(deltaTime);
    Rotating(deltaTime);

}

void Actor::StartRaise()
{
    //切换至公转轨道
    rotating.axis = track.axis;
    rotating.speed = track.oldW;
    GetVectorF();
    mState = State::raising;
}



void Actor::StartLand(float distance)
{
    //记录公转轨道信息
    track.axis = rotating.axis;
    track.oldR = rotating.r;
    track.oldW = rotating.speed;
    //记录离地心距离
    track.neededR = distance;
    mState = State::landing;
}
