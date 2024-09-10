#pragma once
#include "d3dApp.h"
#include "Camera.h"
#include "Actor.h"
#include "lighting.h"
#include <memory>
#include "RenderTargetResource.h"


struct VSConstantBuffer
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
	DirectX::XMMATRIX WorldInvTranspose;
};

struct VsLightViewProjBuffer {
	DirectX::XMMATRIX lightView;
	DirectX::XMMATRIX lightproj;
};

class GameApp : public D3DApp
{
public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
	Camera* currentCamera = nullptr;
	Camera* mFPCamera = nullptr;
	Camera* mTPCamera = nullptr;
	Camera* lightCamera = nullptr;
	Actor* sphere = nullptr;
	Actor* sun = nullptr;
	Actor* airCraft = nullptr;
	Actor* moon = nullptr;
	SkyBox* skySphere = nullptr;
	RenderTargetResource* renderTarget = nullptr;

private:
	bool InitRenderer();
	bool InitResource();


private:
	ComPtr<ID3D11InputLayout> m_pVertexLayout;
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
	ComPtr<ID3D11VertexShader> m_pVertexShader;
	ComPtr<ID3D11VertexShader> m_pVertexShaderSkyBox;
	ComPtr<ID3D11VertexShader> m_pVertexShaderSun;
	ComPtr<ID3D11PixelShader> m_pPixelShaderMoon;
	ComPtr<ID3D11PixelShader> m_pPixelShaderSun;
	ComPtr<ID3D11PixelShader> m_pPixelShaderEarth;
	ComPtr<ID3D11PixelShader> m_pPixelShaderPhong;
	ComPtr<ID3D11Buffer> m_pVSConstantBuffer;
	ComPtr<ID3D11Buffer> m_pPSConstantBuffer;
	ComPtr<ID3D11Buffer> m_eyePosBuffer;
	ComPtr<ID3D11Buffer> m_pShadowVsConstantBuffer;
	VSConstantBuffer m_CBuffer;
	PSConstantBuffer m_PSBuffer;
	VsLightViewProjBuffer m_LightViewProjBuffer;
	ComPtr<ID3D11SamplerState> m_pSamplerState;
	ComPtr<ID3D11RasterizerState> RSNoCull;
	ComPtr<ID3D11RasterizerState> RSCullClockWise;
	void DrawActor(Actor* actor);
	void DrawSkybox(SkyBox* skybox);
	void DrawShadowMap(Actor* actor);
	void GameAppShadowRenderPass(const ComPtr<ID3D11RenderTargetView>& RTV, const ComPtr<ID3D11DepthStencilView>& DSV);
	void GameAppScenePass(const ComPtr<ID3D11RenderTargetView>& RTV, const ComPtr<ID3D11DepthStencilView>& DSV);
};
