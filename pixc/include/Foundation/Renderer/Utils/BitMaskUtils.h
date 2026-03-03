#pragma once

#include <type_traits>

/**
 * @brief Enable bitwise operators for a specific enum type.
 *
 * Generated operators:
 * - operator|  : Combine flags (bitwise OR). Use to add/merge flags.
 * - operator&  : Intersect flags (bitwise AND). Use to test if a flag is present.
 * - operator^  : Toggle flags (bitwise XOR).
 * - operator~  : Bitwise NOT. Use to invert a mask (e.g., clear specific bits).
 * - operator|= : In-place OR (add/merge flags).
 * - operator&= : In-place AND (keep only overlapping flags).
 * - operator^= : In-place XOR (toggle flags).
 *
 * Usage:
 *   enum class MyFlags : uint32_t { None = 0, A = 1<<0, B = 1<<1 };
 *   ENABLE_ENUM_BITMASK(MyFlags)
 *
 * Notes:
 * - Place the macro after the enum definition, ideally in the same namespace,
 *   so ADL finds the operators.
 * - Works with any enum underlying type (uses std::underlying_type_t).
 */
#define ENABLE_ENUM_BITMASK(EnumType)\
    /** @brief Combine flags (bitwise OR): add/merge flags */\
    inline constexpr EnumType operator|(EnumType a, EnumType b) noexcept\
    {\
        using U = std::underlying_type_t<EnumType>;\
        return static_cast<EnumType>(static_cast<U>(a) | static_cast<U>(b));\
    }\
    /** @brief Intersect flags (bitwise AND): test overlap / check presence */\
    inline constexpr EnumType operator&(EnumType a, EnumType b) noexcept\
    {\
        using U = std::underlying_type_t<EnumType>;\
        return static_cast<EnumType>(static_cast<U>(a) & static_cast<U>(b));\
    }\
    /** @brief Toggle flags (bitwise XOR) */\
    inline constexpr EnumType operator^(EnumType a, EnumType b) noexcept\
    {\
        using U = std::underlying_type_t<EnumType>;\
        return static_cast<EnumType>(static_cast<U>(a) ^ static_cast<U>(b));\
    }\
    /** @brief Invert mask (bitwise NOT) */\
    inline constexpr EnumType operator~(EnumType a) noexcept\
    {\
        using U = std::underlying_type_t<EnumType>;\
        return static_cast<EnumType>(~static_cast<U>(a));\
    }\
    /** @brief In-place OR: add/merge flags */\
    inline constexpr EnumType& operator|=(EnumType& a, EnumType b) noexcept\
    {\
        a = (a | b);\
        return a;\
    }\
    /** @brief In-place AND: keep only overlapping flags */\
    inline constexpr EnumType& operator&=(EnumType& a, EnumType b) noexcept\
    {\
        a = (a & b);\
        return a;\
    }\
    /** @brief In-place XOR: toggle flags */\
    inline constexpr EnumType& operator^=(EnumType& a, EnumType b) noexcept\
    {\
        a = (a ^ b);\
        return a;\
    }
