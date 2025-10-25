#include <pch.h>
#include "renderer.h"
#include <render/dx11-hook.h>
#include <CheatMenu.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace renderer
{
	static WNDPROC OriginalWndProcHandler;
	static ID3D11RenderTargetView* mainRenderTargetView;

	static LRESULT CALLBACK hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static void OnInitializeDX11(HWND window, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, IDXGISwapChain* pChain)
	{
		LOG_DEBUG("ImGUI: DirectX11 backend initialized successfully.");

		ImGui::CreateContext();

		//Set OriginalWndProcHandler to the Address of the Original WndProc function
		OriginalWndProcHandler = reinterpret_cast<WNDPROC>(SetWindowLongPtr(window, GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(hWndProc)));
		
		ImGuiIO& io = ImGui::GetIO();
		auto font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\comic.ttf", 24.f);
		io.IniFilename = NULL;

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX11_Init(pDevice, pContext);

		ID3D11Texture2D* pBackBuffer;
		pChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pBackBuffer));
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
		pBackBuffer->Release();
	}

	static void OnRenderDX11(ID3D11DeviceContext* pContext)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::NewFrame();

		cheat::CheatMenu::GetInstance().OnRender();

		ImGui::EndFrame();
		ImGui::Render();

		pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	LRESULT APIENTRY hWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (uMsg == WM_KEYUP) {
			switch (wParam) {
			case VK_F1:
				cheat::CheatMenu::bShowMenu = !cheat::CheatMenu::bShowMenu;
				break;
			default:
				break;
			}
		}

		if (cheat::CheatMenu::bShowMenu) {
			ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
			return true;
		}
		//ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		return CallWindowProc(OriginalWndProcHandler, hwnd, uMsg, wParam, lParam);
	}

	void Init() {
		LOG_DEBUG("Initialize ImGUI...");
		backend::DX11Events::InitializeEvent = &OnInitializeDX11;
		backend::DX11Events::RenderEvent = &OnRenderDX11;
		backend::InitializeDX11Hooks();
	}
}