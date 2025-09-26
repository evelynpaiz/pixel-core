#include "pixcpch.h"
#include "Foundation/Core/FileDialogs.h"

#include "Foundation/Core/Application.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace pixc {

/**
 * @brief Displays a directory selection dialog on macOS using NSOpenPanel.
 *
 * @return The selected directory path. If the user cancels the dialog or an error occurs, an empty string is returned.
 */
std::string FileDialogs::SelectDirectory()
{
    // Enter an autorelease pool to manage memory
    @autoreleasepool
    {
        // Create an instance of NSOpenPanel (macOS file dialog)
        NSOpenPanel* panel = [NSOpenPanel openPanel];
        
        // Set the title of the file dialog
        [panel setTitle:@"Please, select a folder"];
        
        // Configure the dialog to allow selecting directories only, not files
        [panel setCanChooseFiles:NO];
        [panel setCanChooseDirectories:YES];
        [panel setAllowsMultipleSelection:NO];
        
        // Use the application's window as the parent window for the dialog
        NSWindow* parentWindow = (NSWindow*) glfwGetCocoaWindow(
                                                                (GLFWwindow *)Application::Get().GetWindow().GetNativeWindow());
        
        // Get the bundle path of the application
        NSString *bundlePath = [[NSBundle mainBundle] bundlePath];
        NSURL *bundleURL = [NSURL fileURLWithPath:bundlePath];
        
        // Set the initial directory of the dialog to the application's bundle path
        [panel setDirectoryURL:bundleURL];
        
        // Define a variable to store the selected directory path
        __block std::string selectedDirectory;
        
        // Show the file dialog as a sheet attached to the parent window
        [panel beginSheetModalForWindow:parentWindow completionHandler:^(NSInteger result) {
            if (result == NSModalResponseOK) {
                // Get the selected directory path as an NSString
                NSString *selectedPath = [[panel URL] path];
                
                // Convert the NSString to a C++ std::string
                selectedDirectory = std::string([selectedPath UTF8String]);
            }
            
            // Stop the modal loop, allowing the program to continue
            [NSApp stopModal];
        }];
        
        // Block the main thread until the user selects a directory
        [NSApp runModalForWindow:parentWindow];
        
        // Return the selected directory path after the user's selection
        return selectedDirectory;
    }
    
    // Return an empty string by default
    return std::string();
}

/**
 * @brief Process a filter string and set allowed file types for an NSOpenPanel.
 *
 * This function parses a filter string in the format "*.ext1;*.ext2" and sets the allowed file types
 * for an NSOpenPanel to the specified extensions (e.g., ext1, ext2). The filter string is typically used to restrict
 * the types of files that can be selected in an open file dialog.
 *
 * @param filter The filter string in the format "*.ext1;*.ext2".
 * @param openPanel The NSOpenPanel instance to set the allowed file types for.
 */
void ProcessFilterString(const char* filter, NSOpenPanel* panel)
{
    // Check if the filter string is empty or null
    if (filter == nullptr || strlen(filter) == 0)
    {
        // If the filter is empty or null, disallow all files by setting an empty array
        [panel setAllowedContentTypes:@[]];
        return;
    }
    
    // Convert the filter string to an NSString
    NSString *filterList = @(filter);
    
    // Remove leading and trailing spaces from the filter string
    filterList = [filterList stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    
    // Extract individual file extensions separated by semicolons
    NSArray *filterExtensions = [filterList componentsSeparatedByString:@";"];
    NSMutableArray *allowedFileTypes = [NSMutableArray array];
    
    // Iterate through the extracted extensions
    for (NSString *extension in filterExtensions) {
        if (![extension isEqualToString:@""]) {
            // Remove leading and trailing spaces from the extension
            NSString *trimmedExtension = [extension stringByTrimmingCharactersInSet:
                                          [NSCharacterSet whitespaceCharacterSet]];
            
            // Remove the '*' prefix if it exists in the extension
            trimmedExtension = [trimmedExtension stringByReplacingOccurrencesOfString:@"*."
                                                                           withString:@""];
            
            // Check if the trimmed extension is "*"
            if ([trimmedExtension isEqualToString:@"*"]) {
                // Allow all files by setting a wildcard array
                [panel setAllowedContentTypes:@[@"public.item"]];
                return;
            }
            
            // Add the lowercase extension to the allowed file types
            [allowedFileTypes addObject:[trimmedExtension lowercaseString]];
        }
    }
    
    // Set the allowed file types for the NSOpenPanel
    [panel setAllowedFileTypes:allowedFileTypes];
}

/**
 * @brief Displays an open file dialog on macOS using NSOpenPanel.
 *
 * @param filter A pointer to a filter string that specifies the file types to display.
 * @return The selected file path. If the user cancels the dialog or an error occurs, an empty string is returned.
 *
 * @example
 * @code
 * std::string path = FileDialogs::OpenFile("HDR Image (*.hdr)\0*.hdr\0");
 * @endcode
 */
std::string FileDialogs::OpenFile(const char *filter)
{
    // Separate description and extensions from the filter string
    const char *description = filter;
    const char *extensions = filter + strlen(description) + 1;
    
    @autoreleasepool
    {
        // Create an instance of NSOpenPanel for opening files
        NSOpenPanel* openPanel = [NSOpenPanel openPanel];
        
        // Set the title of the open file dialog
        [openPanel setTitle:@"Please, select a file"];
        
        // Configure the dialog to allow selecting files and disallow directories
        [openPanel setCanChooseFiles:YES];
        [openPanel setCanChooseDirectories:NO];
        [openPanel setAllowsMultipleSelection:NO];
        
        // Use the application's window as the parent window for the dialog
        NSWindow* parentWindow = (NSWindow*)glfwGetCocoaWindow((GLFWwindow *)
                                                               Application::Get().GetWindow().GetNativeWindow());
        
        // Define a variable to store the selected file path
        __block std::string selectedFilePath;
        
        // Process the filter string and set allowed file types
        ProcessFilterString(extensions, openPanel);
        
        // Show the open file dialog as a sheet attached to the parent window
        [openPanel beginSheetModalForWindow:parentWindow completionHandler:^(NSInteger result) {
            if (result == NSModalResponseOK) {
                NSString *selectedPath = [[openPanel URL] path];
                
                // Convert the NSString to a C++ std::string
                selectedFilePath = std::string([selectedPath UTF8String]);
            }
            
            // Stop the modal loop, allowing the program to continue
            [NSApp stopModal];
        }];
        
        // Block the main thread until the user selects a file
        [NSApp runModalForWindow:parentWindow];
        
        // Return the selected file path after the user's selection
        return selectedFilePath;
    }
    
    // Return an empty string by default
    return std::string();
}

/**
 * @brief Displays a "Save File" dialog on macOS using NSSavePanel.
 *
 * This function presents a "Save File" dialog to the user on macOS, allowing
 * the user to select a location and specify a file name for saving a file.
 *
 * @param filter A pointer to a filter string that specifies the allowed file types.
 * 
 * @return The selected file path. If the user cancels the dialog or an error occurs,
 *         an empty string is returned.
 *
 * @example
 * @code
 * std::string filepath = FileDialogs::SaveFile("Pixel Core Scene (*.pixc)\0*.pixc\0");
 * @endcode
 *
 * @note TODO: Add filter information as the extension of the saved file.
 */
std::string FileDialogs::SaveFile(const char *filter)
{
    @autoreleasepool
    {
        // Create an instance of NSSavePanel
        NSSavePanel *panel = [NSSavePanel savePanel];
        
        // Set the title of the save file dialog
        [panel setTitle:@"Please, select a file name"];
        
        // Use the application's window as the parent window for the dialog
        NSWindow* parentWindow = (NSWindow*)glfwGetCocoaWindow((GLFWwindow *)
                                                               Application::Get().GetWindow().GetNativeWindow());
        
        // Define a variable to store the selected file path
        __block std::string selectedFile;
        
        // Show the "Save File" dialog as a sheet attached to the parent window
        [panel beginSheetModalForWindow:parentWindow completionHandler:^(NSInteger result) {
            if (result == NSModalResponseOK) {
                // Get the selected file URL
                NSURL *selectedURL = [panel URL];
                
                // Convert the NSURL to a C++ std::string
                selectedFile = std::string([[selectedURL path] UTF8String]);
            }
            
            // Stop the modal loop, allowing the program to continue
            [NSApp stopModal];
        }];
        
        // Block the main thread until the user selects a file
        [NSApp runModalForWindow:parentWindow];
        
        // Return the selected file path after the user's selection
        return selectedFile;
    }
    
    // Return an empty string by default
    return std::string();
}

} // namespace pixc
