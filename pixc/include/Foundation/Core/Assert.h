#pragma once

// Enable assertion messages if we are in DEBUG mode
// and define a debug break according to the used platform
#ifndef NDEBUG
    #define ENGINE_ENABLE_ASSERTS
    // Windows support
    #if defined(_WIN32) || defined(_WIN64)
        #define DEBUGBREAK() __debugbreak()
    // Apple support
    #elif defined(__APPLE__)
        #define DEBUGBREAK() __builtin_debugtrap()
    // Linux support
    #elif defined(__linux__)
        #include <signal.h>
        #define DEBUGBREAK() raise(SIGTRAP)
    #else
        #error "Platform doesn't support debugbreak yet!"
    #endif
#endif

#ifdef ENGINE_ENABLE_ASSERTS
    #define PIXEL_CORE_ASSERT(x, ...)                                                           \
        do                                                                                      \
        {                                                                                       \
            if (!(x))                                                                           \
            {                                                                                   \
                PIXEL_CORE_ERROR("{0}", __VA_ARGS__);                                           \
                DEBUGBREAK();                                                                   \
            }                                                                                   \
        } while (false)
#else
    #include <iostream>
    #include <cstdlib>
    #define PIXEL_CORE_ASSERT(x, ...)                                                           \
        do {                                                                                    \
            if (!(x)) {                                                                         \
                PIXEL_CORE_ERROR("Fatal error. Please run in debug mode for more information!");\
                std::exit(EXIT_FAILURE);                                                        \
            }                                                                                   \
        } while (false)
#endif
