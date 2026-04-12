#include <av/core/logger.h>

#include <spdlog/sinks/stdout_color_sinks.h>

namespace av {
std::shared_ptr<spdlog::logger> Logger::s_core;
std::shared_ptr<spdlog::logger> Logger::s_app;

void Logger::init() {
    spdlog::set_pattern("%^[%T] [%n] [%l]%$ %v");
    s_core = spdlog::stdout_color_mt("engine");
    s_app = spdlog::stdout_color_mt("app");
    s_core->set_level(spdlog::level::trace);
    s_app->set_level(spdlog::level::trace);
}
} // namespace av
