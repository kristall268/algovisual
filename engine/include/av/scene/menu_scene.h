#pragma once
#include <av/scene/scene.h>

namespace av {

class MenuScene : public IScene {
public:
    void on_attach() override;
    void on_detach() override;
    void on_ui(SceneManager& sm) override;
    const char* name() const override { return "Menu"; }
};

} // namespace av
