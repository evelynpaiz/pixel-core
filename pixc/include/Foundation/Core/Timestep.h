#pragma once

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a time interval used for simulation and animation.
 */
class Timestep
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Constructs a timestep with a specified time interval.
    /// @param time The time interval in seconds.
    Timestep(const float time = 0.0f)
    : m_Time(time)
    {}
    /// @brief Delete the timestep.
    ~Timestep() = default;
    
    // Operator(s)
    // ----------------------------------------
    /// @brief Conversion operator to retrieve the time interval in seconds.
    /// @return The time interval in seconds.
    operator float() const { return m_Time; }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the time interval in seconds.
    /// @return The time interval in seconds.
    float GetSeconds() const { return m_Time; }
    /// @brief Get the time interval in milliseconds.
    /// @return The time interval in milliseconds.
    float GetMilliseconds() const { return m_Time * 1000.0f; }
    
    /// @brief Calculate the estimated frames per second (FPS) based on the reciprocal of the time interval.
    /// @note This method provides an estimate of the FPS assuming a constant time interval.
    /// @return The estimated frames per second (FPS).
    int GetFPS() const { return (int)(1.0f / m_Time); }
    
    // Timestep variables
    // ----------------------------------------
private:
    ///< The time interval in seconds.
    float m_Time;
};

} // namespace pixc
