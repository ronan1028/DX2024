#include "d3dApp.h"
#include "d3dUtil.h"
#include <sstream>

namespace
{
	D3DApp* g_pd3dApp = nullptr;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	return g_pd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance)
	: m_hAppInst(hInstance),
	m_MainWndCaption(L"game"),
	m_ClientWidth(1920),
	m_ClientHeight(1080),
	m_hMainWnd(nullptr),
	m_AppPaused(false),
	m_Minimized(false),
	m_Maximized(false),
	m_Resizing(false),
	m_Enable4xMsaa(true),
	m_4xMsaaQuality(0),
	m_pd3dDevice(nullptr),
	m_pd3dImmediateContext(nullptr),
	m_pSwapChain(nullptr),
	m_pDepthStencilBuffer(nullptr),
	m_pRenderTargetView(nullptr),
	m_pDepthStencilView(nullptr)
{
	ZeroMemory(&m_ScreenViewport, sizeof(D3D11_VIEWPORT));



	g_pd3dApp = this;
}

D3DApp::~D3DApp()
{
	if (m_pd3dImmediateContext)
		m_pd3dImmediateContext->ClearState();
}

HINSTANCE D3DApp::AppInst()const
{
	return m_hAppInst;
}

HWND D3DApp::MainWnd()const
{
	return m_hMainWnd;
}

float D3DApp::AspectRatio()const
{
	return static_cast<float>(m_ClientWidth) / m_ClientHeight;
}

int D3DApp::Run()
{
	MSG msg = { 0 };

	m_Timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_Timer.Tick();

			if (!m_AppPaused)
			{
				CalculateFrameStats();
				UpdateScene(m_Timer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool D3DApp::Init()
{
	m_pMouse = std::make_unique<DirectX::Mouse>();
	m_pKeyboard = std::make_unique<DirectX::Keyboard>();
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	return true;
}

void D3DApp::OnResize()
{
	assert(m_pd3dImmediateContext);
	assert(m_pd3dDevice);
	assert(m_pSwapChain);

	if (m_pd3dDevice1 != nullptr)
	{
		assert(m_pd3dImmediateContext1);
		assert(m_pd3dDevice1);
		assert(m_pSwapChain1);
	}

	m_pDepthStencilView.Reset();
	m_pDepthStencilBuffer.Reset();
	m_pRenderTargetView.Reset();


	ComPtr<ID3D11Texture2D> backBuffer;
	m_pSwapChain->ResizeBuffers(1, m_ClientWidth, m_ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	m_pd3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf());


	backBuffer.Reset();


	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = m_ClientWidth;
	depthStencilDesc.Height = m_ClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (m_Enable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}



	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	m_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, m_pDepthStencilBuffer.GetAddressOf());
	m_pd3dDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, m_pDepthStencilView.GetAddressOf());


	m_pd3dImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width = static_cast<float>(m_ClientWidth);
	m_ScreenViewport.Height = static_cast<float>(m_ClientHeight);


	m_pd3dImmediateContext->RSSetViewports(1, &m_ScreenViewport);

}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_AppPaused = true;
			m_Timer.Stop();
		}
		else
		{
			m_AppPaused = false;
			m_Timer.Start();
		}
		return 0;

	case WM_SIZE:
		m_ClientWidth = LOWORD(lParam);
		m_ClientHeight = HIWORD(lParam);
		if (m_pd3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_AppPaused = true;
				m_Minimized = true;
				m_Maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_AppPaused = false;
				m_Minimized = false;
				m_Maximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				if (m_Minimized)
				{
					m_AppPaused = false;
					m_Minimized = false;
					OnResize();
				}

				else if (m_Maximized)
				{
					m_AppPaused = false;
					m_Maximized = false;
					OnResize();
				}
				else if (m_Resizing)
				{

				}
				else 
				{
					OnResize();
				}
			}

		}
		return 0;

	case WM_ENTERSIZEMOVE:
		m_AppPaused = true;
		m_Resizing = true;
		m_Timer.Stop();
		return 0;


	case WM_EXITSIZEMOVE:
		m_AppPaused = false;
		m_Resizing = false;
		m_Timer.Start();
		OnResize();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;


	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_INPUT:

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:

	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		m_pMouse->ProcessMessage(msg, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		m_pKeyboard->ProcessMessage(msg, wParam, lParam);
		return 0;

	case WM_ACTIVATEAPP:
		m_pMouse->ProcessMessage(msg, wParam, lParam);
		m_pKeyboard->ProcessMessage(msg, wParam, lParam);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"D3DWndClassName", m_MainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInst, 0);
	if (!m_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	return true;
}

void D3DApp::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((m_Timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << m_MainWndCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(m_hMainWnd, outs.str().c_str());

		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}


