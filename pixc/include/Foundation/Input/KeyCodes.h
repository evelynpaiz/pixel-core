#pragma once

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

using KeyCode = uint16_t;

/**
 * @namespace pixc::key
 * @brief Key code definitions for keyboard input handling.
 */
namespace key
{
/**
 * @brief Enumeration of key codes conventions.
 *
 * The `Key` namespace provides an enumeration of key codes representing different keys
 * on a keyboard. These key codes can be used to identify and handle specific keyboard input
 * in the application.
 */
enum : KeyCode
{
    // From glfw3.h
    // ----------------------------------------
    Space = 32,         ///< Space key
    Apostrophe = 39,    ///< Apostrophe key (')
    Comma = 44,         ///< Comma key (,)
    Minus = 45,         ///< Minus key (-)
    Period = 46,        ///< Period key (.)
    Slash = 47,         ///< Slash key (/)
    
    D0 = 48,            ///< 0 key
    D1 = 49,            ///< 1 key
    D2 = 50,            ///< 2 key
    D3 = 51,            ///< 3 key
    D4 = 52,            ///< 4 key
    D5 = 53,            ///< 5 key
    D6 = 54,            ///< 6 key
    D7 = 55,            ///< 7 key
    D8 = 56,            ///< 8 key
    D9 = 57,            ///< 9 key
    
    Semicolon = 59,     ///< Semicolon key (;)
    Equal = 61,         ///< Equal key (=)
    
    A = 65,             ///< A key
    B = 66,             ///< B key
    C = 67,             ///< C key
    D = 68,             ///< D key
    E = 69,             ///< E key
    F = 70,             ///< F key
    G = 71,             ///< G key
    H = 72,             ///< H key
    I = 73,             ///< I key
    J = 74,             ///< J key
    K = 75,             ///< K key
    L = 76,             ///< L key
    M = 77,             ///< M key
    N = 78,             ///< N key
    O = 79,             ///< O key
    P = 80,             ///< P key
    Q = 81,             ///< Q key
    R = 82,             ///< R key
    S = 83,             ///< S key
    T = 84,             ///< T key
    U = 85,             ///< U key
    V = 86,             ///< V key
    W = 87,             ///< W key
    X = 88,             ///< X key
    Y = 89,             ///< Y key
    Z = 90,             ///< Z key
    
    LeftBracket = 91,   ///< Left bracket key ([)
    Backslash = 92,     ///< Backslash key (\)
    RightBracket = 93,  ///< Right bracket key (])
    GraveAccent = 96,   ///< Grave accent key (`)
    
    World1 = 161,       ///< Non-US #1 key
    World2 = 162,       ///< Non-US #2 key
    
    // Function keys
    // ----------------------------------------
    Escape = 256,       ///< Escape key
    Enter = 257,        ///< Enter key
    Tab = 258,          ///< Tab key
    Backspace = 259,    ///< Backspace key
    Insert = 260,       ///< Insert key
    Delete = 261,       ///< Delete key
    Right = 262,        ///< Right arrow key
    Left = 263,         ///< Left arrow key
    Down = 264,         ///< Down arrow key
    Up = 265,           ///< Up arrow key
    PageUp = 266,       ///< Page up key
    PageDown = 267,     ///< Page down key
    Home = 268,         ///< Home key
    End = 269,          ///< End key
    CapsLock = 280,     ///< Caps Lock key
    ScrollLock = 281,   ///< Scroll Lock key
    NumLock = 282,      ///< Num Lock key
    PrintScreen = 283,  ///< Print Screen key
    Pause = 284,        ///< Pause key
    F1 = 290,           ///< F1 key
    F2 = 291,           ///< F2 key
    F3 = 292,           ///< F3 key
    F4 = 293,           ///< F4 key
    F5 = 294,           ///< F5 key
    F6 = 295,           ///< F6 key
    F7 = 296,           ///< F7 key
    F8 = 297,           ///< F8 key
    F9 = 298,           ///< F9 key
    F10 = 299,          ///< F10 key
    F11 = 300,          ///< F11 key
    F12 = 301,          ///< F12 key
    F13 = 302,          ///< F13 key
    F14 = 303,          ///< F14 key
    F15 = 304,          ///< F15 key
    F16 = 305,          ///< F16 key
    F17 = 306,          ///< F17 key
    F18 = 307,          ///< F18 key
    F19 = 308,          ///< F19 key
    F20 = 309,          ///< F20 key
    F21 = 310,          ///< F21 key
    F22 = 311,          ///< F22 key
    F23 = 312,          ///< F23 key
    F24 = 313,          ///< F24 key
    F25 = 314,          ///< F25 key
    
    // Keypad
    // ----------------------------------------
    KP0 = 320,          ///< Numeric keypad 0
    KP1 = 321,          ///< Numeric keypad 1
    KP2 = 322,          ///< Numeric keypad 2
    KP3 = 323,          ///< Numeric keypad 3
    KP4 = 324,          ///< Numeric keypad 4
    KP5 = 325,          ///< Numeric keypad 5
    KP6 = 326,          ///< Numeric keypad 6
    KP7 = 327,          ///< Numeric keypad 7
    KP8 = 328,          ///< Numeric keypad 8
    KP9 = 329,          ///< Numeric keypad 9
    KPDecimal = 330,    ///< Numeric keypad decimal point
    KPDivide = 331,     ///< Numeric keypad divide
    KPMultiply = 332,   ///< Numeric keypad multiply
    KPSubtract = 333,   ///< Numeric keypad subtract
    KPAdd = 334,        ///< Numeric keypad add
    KPEnter = 335,      ///< Numeric keypad enter
    KPEqual = 336,      ///< Numeric keypad equal
    
    LeftShift = 340,    ///< Left shift key
    LeftControl = 341,  ///< Left control key
    LeftAlt = 342,      ///< Left alt key
    LeftSuper = 343,    ///< Left super key (e.g., Windows key)
    RightShift = 344,   ///< Right shift key
    RightControl = 345, ///< Right control key
    RightAlt = 346,     ///< Right alt key
    RightSuper = 347,   ///< Right super key (e.g., Windows key)
    Menu = 348,         ///< Menu key
};

} // namespace key
} // namespace pixc
