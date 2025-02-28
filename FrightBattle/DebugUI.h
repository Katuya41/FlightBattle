#pragma once

#include <d3d11.h>
#include <vector>
#include <functional>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"

class DebugUI {
    static std::vector<std::function<void(void)>> m_debugfunction;

public:

    static void Init(ID3D11Device* device, ID3D11DeviceContext* context, HWND& _hwnd);

    // デバッグ表示関数の登録
    static int RedistDebugFunction(std::function<void(void)> f);

    // デバッグ表示関数の解除
    static void UnregisterDebugFunction(int id);

    static void Render();

    static void DisposeUI();
};

