#include <av/core/logger.h>
#include <av/scene/menu_scene.h>
#include <av/scene/scene_manager.h>

#include <imgui.h>

namespace av {

void MenuScene::on_attach() {
    AV_INFO("[MenuScene] attached");
}

void MenuScene::on_detach() {
    AV_INFO("[MenuScene] detached");
}

void MenuScene::on_ui(SceneManager& sm) {
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(360.0f, 0.0f), ImGuiCond_Always);

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

    if (ImGui::Begin("algovisual — Main Menu", nullptr, flags)) {
        ImGui::TextUnformatted("Choose a visualization:");
        ImGui::Separator();

        ImGui::BeginDisabled(true);
        ImGui::Button("Sorting algorithms", ImVec2(-FLT_MIN, 0.0f));
        ImGui::Button("Graph algorithms", ImVec2(-FLT_MIN, 0.0f));
        ImGui::Button("2D physics sandbox", ImVec2(-FLT_MIN, 0.0f));
        ImGui::EndDisabled();
        ImGui::TextDisabled("(implemented from Stage 4 onwards)");

        ImGui::Separator();

        if (ImGui::Button("Reload menu", ImVec2(-FLT_MIN, 0.0f)))
            sm.replace(std::make_unique<MenuScene>());
    }
    ImGui::End();
}

} // namespace av
