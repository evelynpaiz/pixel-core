#pragma once

#include <string>
#include <filesystem>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Manages resource paths for assets in a project, supporting both general and specific overrides.
 *
 * The `ResourcesManager` class provides static interfaces to define and resolve file paths for resources.
 * A "general" path can be set for shared or default assets, and a "specific" path can override it for cases like
 * per-level or per-user assets.
 *
 * It uses `std::filesystem::path` for path operations to ensure portability and correctness.
 */
class ResourcesManager {
    
public:
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Sets the general resources path of the project.
    /// @param path The general base path to the resources directory.
    static void SetGeneralPath(const std::filesystem::path& path) { s_GeneralPath = path; }
    /// @brief Sets a specific resources path to override the general one.
    /// @param path The override path to a specific resources directory.
    static void SetSpecificPath(const std::filesystem::path& path) { s_SpecificPath = path; }

    // Getter(s)
    // ----------------------------------------
    /// @brief Retrieves the full general path for a given relative resource path.
    /// @param relativePath A path relative to the general resources directory.
    /// @return Full filesystem path to the resource.
    static std::filesystem::path GeneralPath(const std::filesystem::path& relativePath)
    {
        return s_GeneralPath / relativePath;
    }
    /// @brief Retrieves the full specific path for a given relative resource path.
    /// @param relativePath A path relative to the specific resources directory.
    /// @return Full filesystem path to the resource.
    static std::filesystem::path SpecificPath(const std::filesystem::path& relativePath)
    {
        return s_SpecificPath / relativePath;
    }

    // Resource manager variables
    // ----------------------------------------
private:
    ///< The general fallback path for resources.
    inline static std::filesystem::path s_GeneralPath = "";
    ///< A specific override path for resources.
    inline static std::filesystem::path s_SpecificPath = "";
};

} // namespace pixc
