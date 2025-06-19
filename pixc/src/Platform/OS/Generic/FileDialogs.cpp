#include "enginepch.h"
#include "Foundation/Core/FileDialogs.h"

namespace pixc {

/**
 * Placeholder (select directory) function for unsupported platforms.
 *
 * @return An empty string, as file dialogs are not supported on this platform.
 */
std::string FileDialogs::SelectDirectory()
{
    // Log a warning message indicating that file dialogs are not supported on this platform
    CORE_WARN("Platform doesn't support file dialogs yet!");
    
    // Return an empty string by default
    return std::string();
}

/**
 * Placeholder (open file) function for unsupported platforms.
 *
 * @return An empty string, as file dialogs are not supported on this platform.
 */
std::string FileDialogs::OpenFile(const char *filter)
{
    // Log a warning message indicating that file dialogs are not supported on this platform
    CORE_WARN("Platform doesn't support file dialogs yet!");
    
    // Return an empty string by default
    return std::string();
}

/**
 * Placeholder (save file) function for unsupported platforms.
 *
 * @return An empty string, as file dialogs are not supported on this platform.
 */
std::string FileDialogs::SaveFile(const char *filter)
{
    // Log a warning message indicating that file dialogs are not supported on this platform
    CORE_WARN("Platform doesn't support file dialogs yet!");
    
    // Return an empty string by default
    return std::string();
}

} // namespace pixc
