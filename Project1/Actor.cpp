#include "Actor.h"
using namespace DirectX;
Actor::Actor(GameApp* mainGame):Object(mainGame)
{
}

void Actor::selfRotate(float deltatime)
{
    selfRotating.angel = selfRotating.angel + deltatime * selfRotating.speed;
    //������0-2pi
    if (selfRotating.angel > XM_2PI)
        selfRotating.angel = selfRotating.angel - XM_2PI;
    else if (selfRotating.angel < 0)
        selfRotating.angel = XM_2PI + selfRotating.angel;
	XMVECTOR rotationVec = XMLoadFloat3(&transform.rotation);
	
	XMMATRIX R =XMMatrixRotationAxis(XMLoadFloat3(&selfRotating.axis),selfRotating.angel);
    XMFLOAT4X4 rotationMatrix;
    XMStoreFloat4x4(&rotationMatrix, R);
    float c = sqrtf(1.0f - rotationMatrix(2, 1) * rotationMatrix(2, 1));
    // ��ֹr[2][1]���ִ���1�����
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
    //������0-2pi
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
    //����Ŀ�����ǵ���תƽ��ͷ�������תƽ���ཻ�γɵķ���������ʹ�����ߵĹ�ת���˵ó��Ľ��
    XMVECTOR intersection;
    intersection=XMVector3Cross(XMLoadFloat3(&(track.roundingStar->rotating.axis)), XMLoadFloat3(&rotating.axis));
    //����Ŀ��������ת����λ�õı�׼����
    intersection = XMVector3Normalize(intersection);
    XMVECTOR vAngel=XMVector3AngleBetweenNormals(track.roundingStar->rotating.FVector, intersection);
    float rad = XMVectorGetX(vAngel);
    //���ڴ�����ʱ����ת��˳ʱ����ת�ó��ĽǶȵ����ֿ��ܣ���Դ˽ǶȽ��в����Ƿ����ھ�����ת��õ��ཻ����
    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&track.roundingStar->rotating.axis), rad);
    XMVECTOR possibleIntersection= XMVector3Transform(track.roundingStar->rotating.FVector,R);
    XMVECTOR vDot = XMVector3Dot(possibleIntersection, intersection);
    float dotResult = XMVectorGetX(vDot);
    if (1 - dotResult > 0.0001f) {
        rad = XM_2PI - rad;
    }
    //����Ŀ��������ת���غϹ��������ʱ��
    float deltaAngel = rad - track.roundingStar->rotating.angel;
    float speed = track.roundingStar->rotating.speed;
    if (deltaAngel * speed > 0)
    {
        deltaAngel = abs(deltaAngel);
        speed = abs(speed);
        if (deltaAngel < 3*XM_PIDIV4) //��Ϊ�ཻ��Ϊ�෴���������������Ǿ�����deltaAngel�ĽǶȲ�Ҫ��С����֤�㹻ʱ����
        {
            deltaAngel += XM_PI;
            intersection = -intersection; //ͬʱҲҪȡ���ཻ������
        }
    }
    else if (deltaAngel * speed < 0) 
    {
        deltaAngel = XM_2PI - abs(deltaAngel);
        speed = abs(speed);
        if (deltaAngel < 3 * XM_PIDIV4) //ͬ��
        {
            deltaAngel += XM_PI;
            intersection = -intersection; //ͬʱҲҪȡ���ཻ������
        }
    }
    float time = deltaAngel / speed;
    //��������򷵻ص�������й��������Ӧ�ú�����������Ӧ�������෴
    if (track.roundingStar == track.rounded) {
        intersection = -intersection;
    }
    //�������������Ļ���
    vAngel = XMVector3AngleBetweenNormals(rotating.FVector, intersection);
  

    rad = XMVectorGetX(vAngel);
    //���ڴ�����ʱ����ת��˳ʱ����ת�ó��ĽǶȵ����ֿ��ܣ���Դ˽ǶȽ��в����Ƿ����ھ�����ת��õ��ཻ����
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
    float w = deltaAngel / time;//������Ľ��ٶ�
    float r= (track.roundingStar->rotating.r + distance-rotating.r) / time;//�������뾶�ı仯����
    //ע������Ϣ
    

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
    //�л�����ת���
    rotating.axis = track.axis;
    rotating.speed = track.oldW;
    GetVectorF();
    mState = State::raising;
}



void Actor::StartLand(float distance)
{
    //��¼��ת�����Ϣ
    track.axis = rotating.axis;
    track.oldR = rotating.r;
    track.oldW = rotating.speed;
    //��¼����ľ���
    track.neededR = distance;
    mState = State::landing;
}
