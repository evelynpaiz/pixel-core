#include "enginepch.h"
#include "Common/Renderer/Shader/Shader.h"

#include "Common/Renderer/Renderer.h"

#include "Platform/OpenGL/Shader/OpenGLShader.h"
#include "Platform/Metal/Shader/MetalShader.h"

// ----------------------------------------
// Shader
// ----------------------------------------

std::shared_ptr<Shader> Shader::Create(const std::string &name, const std::filesystem::path &filePath)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
            
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(name, filePath);
             
#ifdef __APPLE__
        case RendererAPI::API::Metal:
             return std::make_shared<MetalShader>(name, filePath);
#endif
    }
    
    CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
}

std::shared_ptr<Shader> Shader::Create(const std::filesystem::path &filePath)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
            
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(filePath);
             
#ifdef __APPLE__
        case RendererAPI::API::Metal:
             return std::make_shared<MetalShader>(filePath);
#endif
    }
    
    CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
}

/**
 * @brief Reads the contents of a file and returns it as a string.
 *
 * @param filePath The path to the file to be read.
 *
 * @return The contents of the file as a string.
 */
std::string Shader::ReadFile(const std::filesystem::path& filePath)
{
    std::ifstream fileStream(filePath);
    CORE_ASSERT(fileStream.is_open(), "Failed to open file: " + filePath.string());

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

// ----------------------------------------
// Shader Library
// ----------------------------------------

/**
 * Loads a shader from a file and adds it to the library.
 *
 * @param filePath The path to the file containing the shader.
 *
 * @return The loaded shader.
 */
std::shared_ptr<Shader> ShaderLibrary::Load(const std::filesystem::path& filePath)
{
    return ShaderLibrary::Load(filePath.stem().string(), filePath);
}

/**
 * Loads a shader from a file with a specified name and adds it to the library.
 *
 * @param name The name to associate with the loaded shader.
 * @param filePath The path to the file containing the shader.
 *
 * @return The loaded shader.
 */
std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name,
                                            const std::filesystem::path& filePath)
{
    auto shader = Shader::Create(name, filePath);
    Add(name, shader);
    return shader;
}
