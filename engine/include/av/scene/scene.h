#pragma once
namespace av {
class IScene {
public:
    virtual ~IScene() = default;
    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void on_update(float /*dt*/) {}
    virtual void on_ui() {}
    virtual const char* name() const = 0;
};
} // namespace av
