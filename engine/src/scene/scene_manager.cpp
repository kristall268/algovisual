#include <av/core/logger.h>
#include <av/scene/scene_manager.h>

namespace av {

SceneManager::~SceneManager() {
    // Only the top scene is in the "attached" state; deeper scenes were
    // detached when something was pushed on top of them (see Push/Replace).
    if (!stack_.empty())
        stack_.back()->on_detach();
    stack_.clear();
}

// ---- Stack manipulation ---------------------------------------------------

void SceneManager::push(std::unique_ptr<IScene> scene) {
    pending_.push_back({ActionKind::Push, std::move(scene)});
}

void SceneManager::pop() {
    pending_.push_back({ActionKind::Pop, nullptr});
}

void SceneManager::replace(std::unique_ptr<IScene> scene) {
    pending_.push_back({ActionKind::Replace, std::move(scene)});
}

void SceneManager::reset(std::unique_ptr<IScene> scene) {
    pending_.push_back({ActionKind::Reset, std::move(scene)});
}

// ---- Frame lifecycle ------------------------------------------------------

void SceneManager::process_pending() {
    if (pending_.empty())
        return;

    for (auto& action : pending_) {
        switch (action.kind) {
        case ActionKind::Push: {
            if (!stack_.empty())
                stack_.back()->on_detach();
            AV_INFO("[SceneManager] Push: {}", action.scene->name());
            stack_.push_back(std::move(action.scene));
            stack_.back()->on_attach();
            break;
        }
        case ActionKind::Pop: {
            if (stack_.empty()) {
                AV_WARN("[SceneManager] Pop called on empty stack — ignoring");
                break;
            }
            AV_INFO("[SceneManager] Pop: {}", stack_.back()->name());
            stack_.back()->on_detach();
            stack_.pop_back();
            if (!stack_.empty())
                stack_.back()->on_attach();
            break;
        }
        case ActionKind::Replace: {
            if (!stack_.empty()) {
                AV_INFO("[SceneManager] Replace: {} -> {}",
                        stack_.back()->name(),
                        action.scene->name());
                stack_.back()->on_detach();
                stack_.pop_back();
            } else {
                AV_INFO("[SceneManager] Replace (empty stack) -> {}", action.scene->name());
            }
            stack_.push_back(std::move(action.scene));
            stack_.back()->on_attach();
            break;
        }
        case ActionKind::Reset: {
            AV_INFO("[SceneManager] Reset -> {}", action.scene ? action.scene->name() : "<empty>");
            if (!stack_.empty())
                stack_.back()->on_detach();
            stack_.clear();
            if (action.scene) {
                stack_.push_back(std::move(action.scene));
                stack_.back()->on_attach();
            }
            break;
        }
        }
    }

    pending_.clear();
}

void SceneManager::on_update(float dt) {
    if (!stack_.empty())
        stack_.back()->on_update(dt);
}

void SceneManager::on_ui() {
    if (!stack_.empty())
        stack_.back()->on_ui(*this);
}

// ---- Queries --------------------------------------------------------------

IScene* SceneManager::active() const {
    return stack_.empty() ? nullptr : stack_.back().get();
}

} // namespace av
