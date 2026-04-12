#include <av/core/logger.h>

#include <spdlog/sinks/basic_file_sink.h>

namespace av {
std::shared_ptr<spdlog::logger> Logger::s_coreLogger;
std::shared_ptr<spdlog::logger> Logger::s_appLogger;

void Logger::init() {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_coreLogger = spdlog::basic_logger_mt("AV", "logs/av.log");
    s_coreLogger->set_level(spdlog::level::trace);

    s_appLogger = spdlog::basic_logger_mt("APP", "logs/app.log");
    s_appLogger->set_level(spdlog::level::trace);
}
} // namespace av
