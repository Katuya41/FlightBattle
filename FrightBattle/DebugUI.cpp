#include "DebugUI.h"
#include "CScene.h"
#include "CGame.h"

std::vector<std::function<void(void)>> DebugUI::m_debugfunction;

void DebugUI::Init(ID3D11Device* device, ID3D11DeviceContext* context,HWND& _hwnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(_hwnd);
    ImGui_ImplDX11_Init(device, context);
}

void DebugUI::DisposeUI() {
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

// デバッグ表示関数の登録
int DebugUI::RedistDebugFunction(std::function<void(void)> f) {
    m_debugfunction.push_back(std::move(f));
    return (int)(m_debugfunction.size() - 1); // 関数のインデックスを返す
}

void DebugUI::UnregisterDebugFunction(int _id)
{
    if (_id >= 0 && _id < m_debugfunction.size()) {
        m_debugfunction[_id] = nullptr; // 空の関数に置き換え
    }
}

void DebugUI::Render() {
    // ImGuiの新しいフレームを開始
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (CGame::Get()->GetIsDebug())
    {
        // ウィンドウとデバッグ情報の描画
        ImGui::Begin("Debug FPS");
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::InputFloat("BGM_Volume", &CScene::Get()->GetBGMVolume(), 0.001f, 0.01f, "%.3f");
        ImGui::InputFloat("SE_Volume", &CScene::Get()->GetSeVolume(), 0.001f, 0.01f, "%.3f");


        ImGui::End();

        ImGui::Begin("Debug Scene");

        switch (CScene::Get()->GetNowScene())
        {
        case CScene::TITLE:      ImGui::Text("Scene : TITLE");		break;
        case CScene::MENU:       ImGui::Text("Scene : MENU");		break;
        case CScene::STAGE:      ImGui::Text("Scene : STAGE");		
            //当たり判定を表示
            if (ImGui::Button("DrawCollider")) {
                CScene::Get()->SetIsDrawCollider();
            }break;
        case CScene::TEST_STAGE:   ImGui::Text("Scene : TEST_STAGE");
            //当たり判定を表示
            if (ImGui::Button("DrawCollider")) {
                CScene::Get()->SetIsDrawCollider();
            }break;
        case CScene::STAGE_01:   ImGui::Text("Scene : STAGE_01");
            //当たり判定を表示
            if (ImGui::Button("DrawCollider")) {
                CScene::Get()->SetIsDrawCollider();
            }break;
        case CScene::STAGE_02:   ImGui::Text("Scene : STAGE_02");
            //当たり判定を表示
            if (ImGui::Button("DrawCollider")) {
                CScene::Get()->SetIsDrawCollider();
            }break;;
        case CScene::STAGE_03:   ImGui::Text("Scene : STAGE_03");
            //当たり判定を表示
            if (ImGui::Button("DrawCollider")) {
                CScene::Get()->SetIsDrawCollider();
            }break;;
        case CScene::STAGE_04:   ImGui::Text("Scene : STAGE_04");
            //当たり判定を表示
            if (ImGui::Button("DrawCollider")) {
                CScene::Get()->SetIsDrawCollider();
            }break;;
        case CScene::RESULT:     ImGui::Text("Scene : RESULT");		break;
        case CScene::GAMEOVER:   ImGui::Text("Scene : GAMEOVER");	break;
        case CScene::GAMECLEAR:  ImGui::Text("Scene : GAMECLEAR");	break;
        case CScene::CREATE_STAGE: ImGui::Text("Scene : CREATE_STAGE");	break;
        }

        ImGui::End();
    }

    //Xは1～1264
    //Yは0～681

    //632,340.5
    //ImGui::GetForegroundDrawList()->AddRect(ImVec2(607, 315), ImVec2(657, 365), IM_COL32(255, 255, 0, 255));


    // デバッグ関数の実行
    for (auto& f : m_debugfunction)
    {
        if (f) 
        {
            f();
        }
    }

    // フレームのレンダリングを完了
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}