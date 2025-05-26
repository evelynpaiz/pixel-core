#include "enginepch.h"
#include "Common/Core/FileDialogs.h"

#include "Common/Core/Application.h"

#include <ShlObj_core.h>
#include <commdlg.h>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace pixc {

/**
 * @brief Displays a directory selection dialog on Windows using SHBrowseForFolder.
 *
 * @return The selected directory path. If the user cancels the dialog or an error occurs, an empty string is returned.
 */
std::string FileDialogs::SelectDirectory()
{
    // Define a buffer to store the selected directory path
    CHAR szDir[MAX_PATH];
    
    // Initialize the BROWSEINFO structure
    BROWSEINFO br;
    ZeroMemory(&br, sizeof(BROWSEINFO));
    
    // Set the owner window of the dialog to the application's native window
    br.hwndOwner = glfwGetWin32Window((GLFWwindow *)
                                      Application::Get().GetWindow().GetNativeWindow());
    
    // Set the title of the folder selection dialog
    br.lpszTitle = "Please, select a folder";
    
    // Set the callback function for the dialog (nullptr in this case)
    br.lpfn = nullptr;
    
    // Specify flags for the dialog (returning only file system directories and using a new dialog style)
    br.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    
    // Show the folder selection dialog and retrieve the selected item's PIDL
    LPITEMIDLIST lpItem = SHBrowseForFolder(&br);
    
    // Check if an item was selected
    if (lpItem != nullptr)
    {
        // Convert the PIDL to a file system path
        SHGetPathFromIDList(lpItem, szDir);
        
        // Return the selected directory path as a std::string
        return std::string(szDir);
    }
    
    // Return an empty string if the user cancels the dialog or an error occurs
    return std::string();
}

/**
 * @brief Displays an open file dialog on Windows using GetOpenFileNameA.
 *
 * @param filter A pointer to a filter string that specifies the file types to display.
 * @return The selected file path. If the user cancels the dialog or an error occurs, an empty string is returned.
 */
std::string FileDialogs::OpenFile(const char *filter)
{
    // Initialize the OPENFILENAMEA structure
    OPENFILENAMEA ofn;
    CHAR szFile[260] = { 0 };
    CHAR currentDir[256] = { 0 };
    ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    
    // Set the owner window of the dialog to the application's native window
    ofn.hwndOwner = glfwGetWin32Window((GLFWwindow *)
                                       Application::Get().GetWindow().GetNativeWindow());
    
    // Set the buffer to store the selected file path
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    
    // Get the current working directory and set it as the initial directory for the dialog
    if (GetCurrentDirectoryA(256, currentDir))
        ofn.lpstrInitialDir = currentDir;
    
    // Set the filter for file types to display in the dialog
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    
    // Specify flags for the dialog (path must exist, file must exist, and do not change the current directory)
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    
    // Show the open file dialog and retrieve the selected file path
    if (GetOpenFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;
    
    // Return an empty string if the user cancels the dialog or an error occurs
    return std::string();
}

/**
 * @brief Displays a save file dialog on Windows using GetSaveFileNameA.
 *
 * @param filter A pointer to a filter string specifying the file types to display.
 * @return The selected file path or an empty string if canceled or on error.
 */
std::string FileDialogs::SaveFile(const char *filter)
{
    OPENFILENAMEA ofn; // Struct to hold file dialog information
    CHAR szFile[260] = { 0 }; // Buffer to store selected file path
    CHAR currentDir[256] = { 0 }; // Buffer to store the current directory
    
    // Initialize the OPENFILENAME struct
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    
    // Set the parent window for the dialog
    ofn.hwndOwner = glfwGetWin32Window((GLFWwindow *)
                                       Application::Get().GetWindow().GetNativeWindow());
    
    // Set the buffer to store the selected file path and its maximum size
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    
    // Get and set the initial directory to the current working directory
    if (GetCurrentDirectoryA(256, currentDir))
        ofn.lpstrInitialDir = currentDir;
    
    // Set the filter for allowed file types
    ofn.lpstrFilter = filter;
    
    // Set the index of the default filter (1-based)
    ofn.nFilterIndex = 1;
    
    // Set flags to control dialog behavior
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    
    // Sets the default extension by extracting it from the filter
    ofn.lpstrDefExt = strchr(filter, '\0') + 1;
    
    // Show the "Save File" dialog, and return the selected file path if successful
    if (GetSaveFileNameA(&ofn) == TRUE)
        return ofn.lpstrFile;
    
    // Return an empty string if the dialog is canceled
    return std::string();
}

} // namespace pixc
