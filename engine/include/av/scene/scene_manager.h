#pragma once
#include "av/scene/scene.h"

#include <memory>
#include <vector>

namespace av {
class SceneManager {
public:
    SceneManager() = default;
    ~SceneManager();

    void push(std::unique_ptr<IScene> scene);
    void pop();

    void replace(std::unique_ptr<IScene> scene);

    void reset(std::unique_ptr<IScene> scene);

    void process_pending();

    void on_update(float dt);

    void on_ui();

    IScene* active() const;

    bool empty() const { return stack_.empty(); }
    std::size_t size() const { return stack_.size(); }

private:
    enum class ActionKind { Push, Pop, Replace, Reset };
    struct PendingAction {
        ActionKind kind;
        std::unique_ptr<IScene> scene;
    };

    std::vector<std::unique_ptr<IScene>> stack_;
    std::vector<PendingAction> pending_;
};
} // namespace av
