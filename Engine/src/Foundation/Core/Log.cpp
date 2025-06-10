#include "enginepch.h"
#include "Foundation/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

// --------------------------------------------
// Log
// --------------------------------------------

// Variable definition
std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

/**
 * Initialize the logging manager.
 */
void Log::Init()
{
    // Define the patter to be used in the logger
    // ([timestamp] loggerName: message)
#ifdef __APPLE__
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<FlagLogSymbol>('*').set_pattern("%^[%T] %n %*: %v%$");
    spdlog::set_formatter(std::move(formatter));
#else
    spdlog::set_pattern("%^[%T] %n: [%l] %v%$");
#endif
    
    s_CoreLogger = spdlog::stdout_color_mt("CORE");
    s_CoreLogger->set_level(spdlog::level::trace);
}

// --------------------------------------------
// Flag formatter for log symbols
// --------------------------------------------

/**
 * Format the log message with log level symbols.
 *
 * @param msg Log message.
 * @param tm Time information.
 * @param dest Destination buffer.
 *
 * @return A unique pointer to the log level symbol flag.
 */
void FlagLogSymbol::format(const spdlog::details::log_msg &msg,
                           const std::tm &tm, spdlog::memory_buf_t &dest)
{
    std::string symbol = m_Symbols[msg.level];
    dest.append(symbol.data(), symbol.data() + symbol.size());
}

/**
 * Create a copy of the `FlagLogSymbol` object.
 *
 * @return A unique pointer to the log level symbol flag.
 */
std::unique_ptr<spdlog::custom_flag_formatter> FlagLogSymbol::clone() const
{
    return spdlog::details::make_unique<FlagLogSymbol>();
}
