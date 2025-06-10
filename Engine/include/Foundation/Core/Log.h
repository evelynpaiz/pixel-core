#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/pattern_formatter.h>

/**
 * Logging manager used as a wraper for spdlog.
 *
 * The `Log` class provides a centralized logging manager that serves as a wrapper for the spdlog
 * library. It allows initializing the logging system and provides access to the logger instance.
 */
class Log
{
public:
    // Initialization
    // ----------------------------------------
    static void Init();
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the engine logger instance.
    /// @return Engine logger.
    static std::shared_ptr<spdlog::logger> &GetCoreLogger()
    {
        return s_CoreLogger;
    }
    
    // Log variables
    // ----------------------------------------
private:
    ///< Core logger
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
};

/**
 * Custum flag (log level symbols) used to format the log messages to its destination.
 *
 * The `FlagLogSymbol` class is a custom flag formatter for spdlog that formats log messages
 * with log level symbols. It allows customizing the symbols associated with each log level for more
 * visually appealing and informative logs.
 */
class FlagLogSymbol : public spdlog::custom_flag_formatter
{
public:
    // Format
    // ----------------------------------------
    void format(const spdlog::details::log_msg &msg,
                const std::tm &, spdlog::memory_buf_t &dest) override;
    
    // Copy/Clone
    // ----------------------------------------
    std::unique_ptr<spdlog::custom_flag_formatter> clone() const override;
    
    // Flag log symbol variables
    // ----------------------------------------
private:
    ///< Log level symbol equivalence.
    const std::vector<std::string> m_Symbols = { "📓", "📘", "📗", "⚠️", "🛑", "⚡️", "" };
};

// --------------------------------------------
// Definition of the logging macros.
// --------------------------------------------
#define CORE_TRACE(...)     ::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_DEBUG(...)     ::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define CORE_INFO(...)      ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)      ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...)     ::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...)  ::Log::GetCoreLogger()->critical(__VA_ARGS__)
