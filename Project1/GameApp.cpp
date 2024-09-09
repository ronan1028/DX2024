#include "GameApp.h"
#include "d3dUtil.h"
#include "WICTextureLoader.h"



using namespace DirectX;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance), m_CBuffer()
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{

	if (!D3DApp::Init())
		return false;
	if (!InitRenderer())
		return false;

	if (!InitResource())
		return false;
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt)
{


	//refresh eype position buffer
	D3D11_MAPPED_SUBRESOURCE mappedData;
	XMFLOAT3 pos = currentCamera->transform.position;
	XMFLOAT4 eyePosition(pos.x, pos.y, pos.z, 0);
	m_pd3dImmediateContext->Map(m_eyePosBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy_s(mappedData.pData, sizeof(XMFLOAT4), &eyePosition, sizeof(XMFLOAT4));
	m_pd3dImmediateContext->Unmap(m_eyePosBuffer.Get(), 0);
	Keyboard::State keyState = m_pKeyboard->GetState();
	//退出游戏
	if (keyState.IsKeyDown(Keyboard::Escape)) {
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
	}
	//改变轨道
	if (keyState.IsKeyDown(Keyboard::C)&&airCraft->mState== Actor::State::rotate) {
		airCraft->CalculatingTrack(airCraft->track.nextTarget, 2.0f, 0.2f);
	}
	else if (keyState.IsKeyDown(Keyboard::A) && airCraft->mState == Actor::State::rotate) {
		if (airCraft->rotating.target == sphere)
			airCraft->StartLand(0.72f);
		else airCraft->StartLand(0.36f);
	}
	else if (keyState.IsKeyDown(Keyboard::S) && airCraft->mState == Actor::State::landTo) {
		airCraft->StartRaise();
	}
	//update the logic of actors
	sun->update(dt);
	sphere->update(dt);
	moon->update(dt);
	airCraft->update(dt);
	
	//切换相机
	if (keyState.IsKeyDown(Keyboard::F1)) {
		currentCamera = mFPCamera;
	}
	else if (keyState.IsKeyDown(Keyboard::F2)) {
		currentCamera = mTPCamera;
	}

	currentCamera->update(dt);
	//refresh mvp matrix
	m_CBuffer.view = XMMatrixTranspose(currentCamera->GetViewXM());
	m_CBuffer.proj = XMMatrixTranspose(currentCamera->GetProjXM());
}
bool GameApp::InitRenderer()
{

	ComPtr<ID3DBlob> blob;

	CreateShaderFromFile(L"HLSL\\Triangle_VS.cso", L"HLSL\\Triangle_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf());
	m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf());
	CreateShaderFromFile(L"HLSL\\SkyBox_VS.cso", L"HLSL\\SkyBox_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf());
	m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShaderSkyBox.GetAddressOf());
	CreateShaderFromFile(L"HLSL\\Triangle_VSSun.cso", L"HLSL\\Triangle_VSSun.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf());
	m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShaderSun.GetAddressOf());
	m_pd3dDevice->CreateInputLayout(VertexPosNormalTex::inputLayout, ARRAYSIZE(VertexPosNormalTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf());

	CreateShaderFromFile(L"HLSL\\Triangle_PSMoon.cso", L"HLSL\\Triangle_PSMoon.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf());
	m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShaderMoon.GetAddressOf());
	CreateShaderFromFile(L"HLSL\\Triangle_PSSun.cso", L"HLSL\\Triangle_PSSun.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf());
	m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShaderSun.GetAddressOf());
	CreateShaderFromFile(L"HLSL\\Triangle_PSPhong.cso", L"HLSL\\Triangle_PSPhong.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf());
	m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShaderPhong.GetAddressOf());
	CreateShaderFromFile(L"HLSL\\Triangle_PSEarth.cso", L"HLSL\\Triangle_PSEarth.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf());
	m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShaderEarth.GetAddressOf());
	ComPtr<ID3D11DepthStencilState> DSSLessEqual;
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = false;
	m_pd3dDevice->CreateDepthStencilState(&dsDesc, DSSLessEqual.GetAddressOf());
	m_pd3dImmediateContext->OMSetDepthStencilState(DSSLessEqual.Get(), 0);
	//create render state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	//no culling
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = true;
	m_pd3dDevice->CreateRasterizerState(&rasterizerDesc, RSNoCull.GetAddressOf());
	//culling
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = true;
	m_pd3dDevice->CreateRasterizerState(&rasterizerDesc, RSCullClockWise.GetAddressOf());
	return true;
}
bool GameApp::InitResource()
{
	ComPtr<ID3D11ShaderResourceView> m_earth;
	ComPtr<ID3D11ShaderResourceView> m_sun;
	ComPtr<ID3D11ShaderResourceView> m_moon;
	ComPtr<ID3D11ShaderResourceView> m_skybox;
	ComPtr<ID3D11ShaderResourceView> m_earthNight;
	//load texture
	CreateWICTextureFromFile(m_pd3dDevice.Get(), L"model/earth.jpg", nullptr, m_earth.GetAddressOf());
	CreateWICTextureFromFile(m_pd3dDevice.Get(), L"model/sun.jpg", nullptr, m_sun.GetAddressOf());
	CreateWICTextureFromFile(m_pd3dDevice.Get(), L"model/moon.jpg", nullptr, m_moon.GetAddressOf());
	CreateWICTextureFromFile(m_pd3dDevice.Get(), L"model/skybox.jpg", nullptr, m_skybox.GetAddressOf());
	CreateWICTextureFromFile(m_pd3dDevice.Get(), L"model/earth_nightmap.jpg", nullptr, m_earthNight.GetAddressOf());
	m_pd3dImmediateContext->PSSetShaderResources(1, 1, m_earthNight.GetAddressOf());
	//set sampler desc and create sampler state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MaxAnisotropy = 4;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_pd3dDevice->CreateSamplerState(&sampDesc, m_pSamplerState.GetAddressOf());
	
//加载mesh
	std::vector<MeshData> meshs= loadModel("model/sphere.obj", m_pd3dDevice);
	sphere = new Actor(this);
	sun = new Actor(this);
	moon = new Actor(this);
	airCraft = new Actor(this);
	//setting skybox
	skySphere = new SkyBox(this);
	skySphere->mesh.Meshdata= meshs[0];
	skySphere->mesh.m_pPixelShader = m_pPixelShaderSun;
	skySphere->mesh.m_pVertexShader = m_pVertexShaderSkyBox;
	skySphere->mesh.m_diffuseTexture = m_skybox;

	//setting earth
	sphere->mesh.Meshdata = meshs[0];
	sphere->mesh.m_pPixelShader = m_pPixelShaderEarth;
	sphere->mesh.m_pVertexShader = m_pVertexShader;
	sphere->mesh.m_diffuseTexture = m_earth;
	sphere->transform.scale = XMFLOAT3(0.002f, 0.002f, 0.002f);
	sphere->transform.position = XMFLOAT3(500.0f, 0.0f, 0.0f);
	//setting self-rotate
	sphere->selfRotating.axis = XMFLOAT3(1.0f, 1.0f, 1.0f);
	sphere->selfRotating.speed = -0.3f;
	//setting rotate
	sphere->rotating.target = sun;
	sphere->rotating.axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	sphere->rotating.speed = 0.05f;
	sphere->GetVectorF();
	sphere->rotating.r = 1500.0f;

	//setting sun
	sun->mesh.Meshdata = meshs[0];
	sun->mesh.m_pPixelShader = m_pPixelShaderSun;
	sun->mesh.m_pVertexShader = m_pVertexShaderSun;
	sun->mesh.m_diffuseTexture = m_sun;
	sun->transform.scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
	sun->transform.position = XMFLOAT3(0.0f, 0.0f,0.0f);

	//setting self-rotate
	sun->selfRotating.axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	sun->selfRotating.speed = -0.3f;



	//setting moon
	moon->mesh.Meshdata = meshs[0];
	moon->mesh.m_pPixelShader = m_pPixelShaderMoon;
	moon->mesh.m_pVertexShader = m_pVertexShader;
	moon->mesh.m_diffuseTexture = m_moon;
	moon->transform.scale = XMFLOAT3(0.001f, 0.001f, 0.001f);
	moon->transform.position = XMFLOAT3(500.0f, 0.0f, 0.0f);
	//setting rotate
	moon->selfRotating.axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	moon->selfRotating.speed = -0.3f;
	moon->rotating.axis = XMFLOAT3(1.0f, 1.0f, 1.0f);
	moon->rotating.target = sphere;
	moon->rotating.speed = -0.5f;
	moon->GetVectorF();
	moon->rotating.r = 15.0f;


	
	//setting aircraft
	airCraft->mesh.Meshdata = meshs[0];
	airCraft->mesh.m_pPixelShader = m_pPixelShaderPhong;
	airCraft->mesh.m_pVertexShader = m_pVertexShader;
	airCraft->mesh.m_diffuseTexture = m_earth;
	airCraft->transform.scale = XMFLOAT3(0.0001f, 0.0001f, 0.0001f);
	airCraft->transform.position = XMFLOAT3(500.0f, 1.0f, 0.0f);
	airCraft->selfRotating.axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	airCraft->rotating.axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//setting rotate	
	airCraft->rotating.target = sphere;
	airCraft->rotating.speed = 0.2f;
	airCraft->GetVectorF();
	airCraft->rotating.r = 2.0f;
	airCraft->track.nextTarget = moon;
	airCraft->track.rounded = sphere;
	airCraft->track.roundingStar = moon;
	//create camera
	currentCamera = new FPCamera(this, airCraft);
	mFPCamera = currentCamera;
	mTPCamera = new TPCamera(this, airCraft);
	currentCamera->transform.position=XMFLOAT3(500.0f,1.0f,0.0f);
	//init constant buffer desc
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(VSConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pVSConstantBuffer.GetAddressOf());
	cbd.ByteWidth = sizeof(PSConstantBuffer);
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pPSConstantBuffer.GetAddressOf());
	cbd.ByteWidth = sizeof(XMFLOAT4);
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_eyePosBuffer.GetAddressOf());

	//setting the value of constant buffer point light
	m_PSBuffer.material.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_PSBuffer.material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_PSBuffer.material.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 5.0f);
	m_PSBuffer.pointLight.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_PSBuffer.pointLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_PSBuffer.pointLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_PSBuffer.pointLight.att = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_PSBuffer.pointLight.range = 100000.0f;
	m_PSBuffer.pointLight.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	// map data
	D3D11_MAPPED_SUBRESOURCE mappedData;
	m_pd3dImmediateContext->Map(m_pPSConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy_s(mappedData.pData, sizeof(PSConstantBuffer), &m_PSBuffer, sizeof(PSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pPSConstantBuffer.Get(), 0);
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// bind constant buffer
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pVSConstantBuffer.GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pPSConstantBuffer.GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(2, 1, m_eyePosBuffer.GetAddressOf());
	// ******************
	return true;
}

void GameApp::DrawActor(Actor* actor)
{
	//set shader
	m_pd3dImmediateContext->VSSetShader(actor->mesh.m_pVertexShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(actor->mesh.m_pPixelShader.Get(), nullptr, 0);
	//set texture
	m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
	m_pd3dImmediateContext->PSSetShaderResources(0, 1, actor->mesh.m_diffuseTexture.GetAddressOf());
	//set vertex and index buffer
	m_pd3dImmediateContext->IASetIndexBuffer(actor->mesh.Meshdata.m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	UINT stride = sizeof(VertexPosNormalTex);	// 跨越字节数
	UINT offset = 0;						// 起始偏移量
	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, actor->mesh.Meshdata.m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	//refresh constant buffer
	m_CBuffer.world = XMMatrixTranspose(actor->transform.GetLocalToWorldMatrix());
	m_CBuffer.WorldInvTranspose = XMMatrixTranspose(InverseTranspose(actor->transform.GetLocalToWorldMatrix()));
	D3D11_MAPPED_SUBRESOURCE mappedData;
	m_pd3dImmediateContext->Map(m_pVSConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy_s(mappedData.pData, sizeof(m_CBuffer), &m_CBuffer, sizeof(m_CBuffer));
	m_pd3dImmediateContext->Unmap(m_pVSConstantBuffer.Get(), 0);
	m_pd3dImmediateContext->DrawIndexed(actor->mesh.Meshdata.indexNum, 0, 0);

}
void GameApp::DrawSkybox(SkyBox* skybox) {
	//set shader
	m_pd3dImmediateContext->VSSetShader(skybox->mesh.m_pVertexShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(skybox->mesh.m_pPixelShader.Get(), nullptr, 0);
	//set texture
	m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());
	m_pd3dImmediateContext->PSSetShaderResources(0, 1, skybox->mesh.m_diffuseTexture.GetAddressOf());
	//set vertex and index buffer
	m_pd3dImmediateContext->IASetIndexBuffer(skybox->mesh.Meshdata.m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	UINT stride = sizeof(VertexPosNormalTex);
	UINT offset = 0;
	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, skybox->mesh.Meshdata.m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	//refresh the view matrix when assume camera position is (0,0,0)
	XMFLOAT3 cameraPosition = currentCamera->transform.position;
	currentCamera->transform.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_CBuffer.view = XMMatrixTranspose(currentCamera->GetViewXM());
	currentCamera->transform.position = cameraPosition;
	D3D11_MAPPED_SUBRESOURCE mappedData;
	m_pd3dImmediateContext->Map(m_pVSConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy_s(mappedData.pData, sizeof(m_CBuffer), &m_CBuffer, sizeof(m_CBuffer));
	m_pd3dImmediateContext->Unmap(m_pVSConstantBuffer.Get(), 0);
	//setting RS state
	m_pd3dImmediateContext->DrawIndexed(skybox->mesh.Meshdata.indexNum, 0, 0);
	
}

void GameApp::DrawShadow()
{

}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);
	static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DrawActor(sphere);
	DrawActor(sun);
	DrawActor(moon);
	DrawActor(airCraft);
	m_pd3dImmediateContext->RSSetState(RSNoCull.Get());
	DrawSkybox(skySphere);
	m_pd3dImmediateContext->RSSetState(RSCullClockWise.Get());
	m_pSwapChain->Present(0, 0);
}
