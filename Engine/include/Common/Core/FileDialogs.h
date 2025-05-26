#pragma once

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief File and directory selection dialogs.
 *
 * The `FileDialogs` class offers static methods for displaying file and directory selection dialogs
 * on both Windows and macOS platforms. It allows users to interactively select files and directories
 * and returns their paths as strings.
 *
 * Example usage:
 *
 * ```cpp
 * std::string selectedDirectory = FileDialogs::SelectDirectory();
 * std::string selectedFile = FileDialogs::OpenFile("All Files (*.*)\0*.*\0");
 * ```
 */
class FileDialogs
{
public:
    // Dialogs
    // ----------------------------------------
    static std::string SelectDirectory();
    static std::string OpenFile(const char *filter);
    static std::string SaveFile(const char *filter);
};

} // namespace pixc
