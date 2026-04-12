#pragma once
#include <spdlog/spdlog.h>

#include <memory>

namespace av {
class Logger {
public:
    static void init();
    static std::shared_ptr<spdlog::Logger>& getCoreLogger();
    static std::shared_ptr<spdlog::Logger>& getAppLogger();

private:
    static std::shared_ptr<spdlog::Logger> s_coreLogger;
    static std::shared_ptr<spdlog::Logger> s_appLogger;
};
} // namespace av

#define AV_TRACE(...) ::av::Logger::core()->trace(__VA_ARGS__)
#define AV_INFO(...) ::av::Logger::core()->info(__VA_ARGS__)
#define AV_WARN(...) ::av::Logger::core()->warn(__VA_ARGS__)
#define AV_ERROR(...) ::av::Logger::core()->error(__VA_ARGS__)
#define AV_FATAL(...) ::av::Logger::core()->critical(__VA_ARGS__)
