// Made by Ítalo Seara (https://github.com/italoseara)

#ifndef OVERLAY_HPP
#define OVERLAY_HPP

#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Overlay
{
    HINSTANCE hInstance = nullptr;
    INT nCmdShow = 0;
    void (*drawLoop)();

    // Window
    HWND window = nullptr;
    int width = 0;
    int height = 0;

    // DirectX
    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;

    LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam))
            return 0L;

        if (message == WM_DESTROY)
        {
            PostQuitMessage(0);
            return 0L;
        }

        return DefWindowProc(window, message, wParam, lParam);
    }

    void InitializeWindow()
    {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProcedure;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"UCheatOverlay";
        if (!RegisterClassExW(&wc))
        {
            std::cerr << "Failed to register window class." << std::endl;
            exit(1);
        }

        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
        window = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
            wc.lpszClassName,
            L"UCheat Overlay",
            WS_POPUP,
            0, 0,
            width, height,
            nullptr,
            nullptr,
            wc.hInstance,
            nullptr
        );

        if (!window)
        {
            std::cerr << "Failed to create window." << std::endl;
            exit(1);
        }

        SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

        {
            RECT clientArea{};
            if (!GetClientRect(window, &clientArea))
            {
                std::cerr << "Failed to get client area." << std::endl;
                exit(1);
            }

            RECT windowArea{};
            if (!GetWindowRect(window, &windowArea))
            {
                std::cerr << "Failed to get window rect." << std::endl;
                exit(1);
            }

            POINT diff{};
            if (!ClientToScreen(window, &diff))
            {
                std::cerr << "Failed to convert client to screen." << std::endl;
                exit(1);
            }

            const MARGINS margins{
                windowArea.left + (diff.x - windowArea.left),
                windowArea.top + (diff.y - windowArea.top),
                clientArea.right,
                clientArea.bottom
            };

            if (FAILED(DwmExtendFrameIntoClientArea(window, &margins)))
            {
                std::cerr << "Failed to extend frame into client area." << std::endl;
                exit(1);
            }
        }

        ShowWindow(window, nCmdShow);
        UpdateWindow(window);
    }

    void InitializeDirectX()
    {
        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferDesc.RefreshRate.Numerator = 60U;
        sd.BufferDesc.RefreshRate.Denominator = 1U;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1U;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1U;
        sd.OutputWindow = window;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        constexpr D3D_FEATURE_LEVEL levels[2]{
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_0,
        };

        if (FAILED(D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0U,
            levels,
            2U,
            D3D11_SDK_VERSION,
            &sd,
            &swapChain,
            &device,
            nullptr,
            &deviceContext)))
        {
            std::cerr << "Failed to create device and swap chain." << std::endl;
            exit(1);
        }

        ID3D11Texture2D* backBuffer{ nullptr };
        if (FAILED(swapChain->GetBuffer(0U, IID_PPV_ARGS(&backBuffer))))
        {
            std::cerr << "Failed to get swap chain buffer." << std::endl;
            exit(1);
        }

        if (backBuffer)
        {
            if (FAILED(device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView)))
            {
                std::cerr << "Failed to create render target view." << std::endl;
                exit(1);
            }
            backBuffer->Release();
        }
        else
        {
            std::cerr << "Failed to get back buffer." << std::endl;
            exit(1);
        }
    }

    void InitializeImGui()
    {
        if (!ImGui::CreateContext())
        {
            std::cerr << "Failed to create ImGui context." << std::endl;
            exit(1);
        }
        ImGui::StyleColorsDark();

        if (!ImGui_ImplWin32_Init(window))
        {
            std::cerr << "Failed to initialize ImGui Win32." << std::endl;
            exit(1);
        }
        if (!ImGui_ImplDX11_Init(device, deviceContext))
        {
            std::cerr << "Failed to initialize ImGui DX11." << std::endl;
            exit(1);
        }
    }

    void RenderLoop()
    {
        MSG msg;
        bool running = true;
        while (true)
        {
            while (PeekMessageW(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);

                if (msg.message == WM_QUIT)
                    running = false;
            }

            if (!running) break;

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            // Draw your stuff here
            drawLoop();

            ImGui::Render();

            constexpr FLOAT clearColor[4]{ 0.0f, 0.0f, 0.0f, 0.0f };
            deviceContext->OMSetRenderTargets(1U, &renderTargetView, nullptr);
            deviceContext->ClearRenderTargetView(renderTargetView, clearColor);

            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            if (FAILED(swapChain->Present(1U, 0U)))
            {
                std::cerr << "Failed to present swap chain." << std::endl;
                exit(1);
            }
        }
    }

    void Cleanup()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        if (swapChain)        swapChain->Release();
        if (deviceContext)    deviceContext->Release();
        if (device)           device->Release();
        if (renderTargetView) renderTargetView->Release();

        DestroyWindow(window);
        UnregisterClassW(L"UCheatOverlay", hInstance);
    }

    void Run(HINSTANCE hInstance, INT nCmdShow, void (*drawLoop)())
    {
        Overlay::hInstance = hInstance;
        Overlay::nCmdShow = nCmdShow;
        Overlay::drawLoop = drawLoop;

        InitializeWindow();
        InitializeDirectX();
        InitializeImGui();

        RenderLoop();
        Cleanup();
    }
}

#endif // OVERLAY_HPP