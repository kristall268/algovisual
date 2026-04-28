#include <av/core/application.h>
#include <av/scene/menu_scene.h>

#include <cstdio>
#include <exception>
#include <memory>

int main(int, char**) {
    try {
        av::Application app(1280, 720, "algovisual");
        app.push_scene(std::make_unique<av::MenuScene>());
        app.run();
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Fatal: %s\n", e.what());
        return 1;
    }
    return 0;
}
