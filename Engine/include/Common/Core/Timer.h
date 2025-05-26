#pragma once

#include <chrono>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a high-resolution timer for measuring elapsed time.
 */
class Timer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Constructs a timer.
    Timer()
    {
        Reset();
    }
    /// @brief Delete the timer.
    ~Timer() = default;
    
    // Reset
    // ----------------------------------------
    /// @brief Resets the timer to the current time.
    void Reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Calculates the elapsed time since the timer was last reset.
    /// @return The elapsed time in seconds.
    float Elapsed()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                                                                    std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
    }
    /// @brief Calculates the elapsed time since the timer was last reset.
    /// @return The elapsed time in milliseconds.
    float ElapsedMilliseconds()
    {
        return Elapsed() * 1000.0f;
    }
    
private:
    ///< The start time of the timer.
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};

} // namespace pixc
