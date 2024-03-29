#pragma once

#include "tppUIBackend.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "imgui.h"

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

static void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

static bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

static void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

static void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

WNDCLASSEX wc;
HWND hwnd;

bool tpp::UIBackend::InitializePlatform(const UIInitializeParams& params)
{
	wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Tweak++ 0.01", NULL };

	RegisterClassEx(&wc);

	// https://stackoverflow.com/questions/34583160/winapi-createwindow-function-creates-smaller-windows-than-set/34583716
	RECT rect =
	{
		params.windowPositionX,
		params.windowPositionY,
		params.windowPositionX + params.windowWidth,
		params.windowPositionY + params.windowHeight
	};

	UINT style = WS_OVERLAPPEDWINDOW;
	AdjustWindowRectEx(&rect, style, 0, 0);
	hwnd = ::CreateWindow(wc.lpszClassName, L"Tweak++ 0.01", style, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return false;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	return true;
}

MSG msg = {};

tpp::UIBackendResult tpp::UIBackend::PrepareNewFramePlatform()
{
	// Dispatch all messages
	while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return UIBackendResult::Quit;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	return UIBackendResult::Ok;
}

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

bool tpp::UIBackend::RenderPlatform()
{
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	g_pSwapChain->Present(1, 0); // Present with vsync

	return true;
}

bool tpp::UIBackend::ShutdownPlatform()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	
	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return true;
}

int tpp::UIBackend::GetWindowWidth()
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	return rect.right - rect.left;
}

int tpp::UIBackend::GetWindowHeight()
{
	RECT rect;
	GetClientRect(hwnd, &rect);
	return rect.bottom - rect.top;
}