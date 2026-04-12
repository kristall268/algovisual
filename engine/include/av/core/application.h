#pragma once
#include <memory>
#include <string>

namespace av {
class IScene;

class Application {
public:
    Application(int w, int h, std::string title);
    ~Application();

    void register_scene(std::unique_ptr<IScene> scene); // stub для этапа 2
    void run();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
} // namespace av
