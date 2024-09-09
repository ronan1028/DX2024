#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")

#include <wrl/client.h>
#include <string>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include "GameTimer.h"
#include "Mouse.h"
#include "Keyboard.h"
class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	virtual ~D3DApp();

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;

	int Run();


	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	std::unique_ptr<DirectX::Mouse> m_pMouse;
	DirectX::Mouse::ButtonStateTracker m_MouseTracker;
	std::unique_ptr<DirectX::Keyboard> m_pKeyboard;
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;
protected:
	bool InitMainWindow();
	bool InitDirect3D()
	{
		HRESULT hr = S_OK;

		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		UINT numDriverTypes = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		D3D_FEATURE_LEVEL featureLevel;
		D3D_DRIVER_TYPE d3dDriverType;
		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
		{
			d3dDriverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, m_pd3dDevice.GetAddressOf(), &featureLevel, m_pd3dImmediateContext.GetAddressOf());

			if (hr == E_INVALIDARG)
			{
				hr = D3D11CreateDevice(nullptr, d3dDriverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
					D3D11_SDK_VERSION, m_pd3dDevice.GetAddressOf(), &featureLevel, m_pd3dImmediateContext.GetAddressOf());
			}

			if (SUCCEEDED(hr))
				break;
		}

		if (FAILED(hr))
		{
			return false;
		}

		if (featureLevel != D3D_FEATURE_LEVEL_11_0 && featureLevel != D3D_FEATURE_LEVEL_11_1)
		{
			return false;
		}

		m_pd3dDevice->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality);
		assert(m_4xMsaaQuality > 0);




		ComPtr<IDXGIDevice> dxgiDevice = nullptr;
		ComPtr<IDXGIAdapter> dxgiAdapter = nullptr;
		ComPtr<IDXGIFactory1> dxgiFactory1 = nullptr;
		ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;


		m_pd3dDevice.As(&dxgiDevice);
		dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
		dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory1.GetAddressOf()));

		hr = dxgiFactory1.As(&dxgiFactory2);
		if (dxgiFactory2 != nullptr)
		{
			m_pd3dDevice.As(&m_pd3dDevice1);
			m_pd3dImmediateContext.As(&m_pd3dImmediateContext1);
			DXGI_SWAP_CHAIN_DESC1 sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.Width = m_ClientWidth;
			sd.Height = m_ClientHeight;
			sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			if (m_Enable4xMsaa)
			{
				sd.SampleDesc.Count = 4;
				sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
			}
			else
			{
				sd.SampleDesc.Count = 1;
				sd.SampleDesc.Quality = 0;
			}
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.BufferCount = 1;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			sd.Flags = 0;

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd;
			fd.RefreshRate.Numerator = 60;
			fd.RefreshRate.Denominator = 1;
			fd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			fd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			fd.Windowed = TRUE;

			dxgiFactory2->CreateSwapChainForHwnd(m_pd3dDevice.Get(), m_hMainWnd, &sd, &fd, nullptr, m_pSwapChain1.GetAddressOf());
			m_pSwapChain1.As(&m_pSwapChain);
		}
		else
		{

			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.BufferDesc.Width = m_ClientWidth;
			sd.BufferDesc.Height = m_ClientHeight;
			sd.BufferDesc.RefreshRate.Numerator = 60;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

			if (m_Enable4xMsaa)
			{
				sd.SampleDesc.Count = 4;
				sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
			}
			else
			{
				sd.SampleDesc.Count = 1;
				sd.SampleDesc.Quality = 0;
			}
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.BufferCount = 1;
			sd.OutputWindow = m_hMainWnd;
			sd.Windowed = TRUE;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			sd.Flags = 0;
			dxgiFactory1->CreateSwapChain(m_pd3dDevice.Get(), &sd, m_pSwapChain.GetAddressOf());
		}



		dxgiFactory1->MakeWindowAssociation(m_hMainWnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

		OnResize();

		return true;
	}

	void CalculateFrameStats();

protected:

	HINSTANCE m_hAppInst;
	HWND      m_hMainWnd;
	bool      m_AppPaused;
	bool      m_Minimized;
	bool      m_Maximized;
	bool      m_Resizing;
	bool	  m_Enable4xMsaa;
	UINT      m_4xMsaaQuality;


	GameTimer m_Timer;

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	ComPtr<ID3D11Device> m_pd3dDevice;
	ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;
	ComPtr<IDXGISwapChain> m_pSwapChain;
	ComPtr<ID3D11Device1> m_pd3dDevice1;
	ComPtr<ID3D11DeviceContext1> m_pd3dImmediateContext1;
	ComPtr<IDXGISwapChain1> m_pSwapChain1;
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
	D3D11_VIEWPORT m_ScreenViewport;

	std::wstring m_MainWndCaption;
	int m_ClientWidth;
	int m_ClientHeight;
};
