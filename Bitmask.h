#pragma once

#include <type_traits>
#include <cstdint>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

// Declare bitwise operators for a specific enum type.
// Usage: PIXC_ENABLE_ENUM_BITMASK(YourEnumType);
#define PIXC_ENABLE_ENUM_BITMASK(EnumType)                                      \
    inline constexpr EnumType operator|(EnumType a, EnumType b) noexcept {      \
        using U = std::underlying_type_t<EnumType>;                             \
        return static_cast<EnumType>(static_cast<U>(a) | static_cast<U>(b));    \
    }                                                                           \
    inline constexpr EnumType operator&(EnumType a, EnumType b) noexcept {      \
        using U = std::underlying_type_t<EnumType>;                             \
        return static_cast<EnumType>(static_cast<U>(a) & static_cast<U>(b));    \
    }                                                                           \
    inline constexpr EnumType operator^(EnumType a, EnumType b) noexcept {      \
        using U = std::underlying_type_t<EnumType>;                             \
        return static_cast<EnumType>(static_cast<U>(a) ^ static_cast<U>(b));    \
    }                                                                           \
    inline constexpr EnumType operator~(EnumType a) noexcept {                  \
        using U = std::underlying_type_t<EnumType>;                             \
        return static_cast<EnumType>(~static_cast<U>(a));                       \
    }                                                                           \
    inline constexpr EnumType& operator|=(EnumType& a, EnumType b) noexcept {   \
        a = (a | b);                                                            \
        return a;                                                               \
    }                                                                           \
    inline constexpr EnumType& operator&=(EnumType& a, EnumType b) noexcept {   \
        a = (a & b);                                                            \
        return a;                                                               \
    }                                                                           \
    inline constexpr EnumType& operator^=(EnumType& a, EnumType b) noexcept {   \
        a = (a ^ b);                                                            \
        return a;                                                               \
    }                                                                           \
    inline constexpr bool Any(EnumType a) noexcept {                            \
        using U = std::underlying_type_t<EnumType>;                             \
        return static_cast<U>(a) != static_cast<U>(0);                          \
    }                                                                           \
    inline constexpr bool None(EnumType a) noexcept {                           \
        using U = std::underlying_type_t<EnumType>;                             \
        return static_cast<U>(a) == static_cast<U>(0);                          \
    }

} // namespace pixc
