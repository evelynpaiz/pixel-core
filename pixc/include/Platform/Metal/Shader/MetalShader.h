#pragma once

#include "Foundation/Renderer/Shader/Shader.h"

#include "Platform/Metal/MetalContext.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Concrete implementation of the Shader class for the Metal API.
 *
 * The `MetalShader` class specializes the `Shader` base class for Metal, handling the
 * compilation of Metal Shading Language (MSL) code, management of Metal functions,
 * and interaction with Metal buffers for uniform data.
 *
 * Copying or moving `MetalShader` objects is disabled to ensure single ownership and
 * prevent unintended shader duplication.
 */
class MetalShader : public Shader
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    MetalShader(const std::string& name, const std::filesystem::path& filePath);
    MetalShader(const std::filesystem::path& filePath);
    ~MetalShader() override;
    
    // Usage
    // ----------------------------------------
    void Bind() const override;
    void Unbind() const override;
    
    // Getter(s)
    // ----------------------------------------
    void* GetVertexFunction() const;
    void* GetFragmentFunction() const;
    
    // Setter(s)
    // ----------------------------------------
    void SetBool(const std::string& name, bool value) override;
    void SetInt(const std::string& name, int value) override;
    void SetFloat(const std::string& name, float value) override;
    
    void SetVec2(const std::string& name, const glm::vec2& value) override;
    void SetVec3(const std::string& name, const glm::vec3& value) override;
    void SetVec4(const std::string& name, const glm::vec4& value) override;
    
    void SetMat2(const std::string& name, const glm::mat2& value) override;
    void SetMat3(const std::string& name, const glm::mat3& value) override;
    void SetMat4(const std::string& name, const glm::mat4& value) override;
    
    void SetTexture(const std::string &name,
                    const std::shared_ptr<Texture>& texture,
                    int slot) override;
    
private:
    // Getter(s)
    // ----------------------------------------
    void* GetShaderVertexDescriptor();
    
    // Compilation
    // ----------------------------------------
    void CompileShader(const std::filesystem::path& filePath);
    std::string ParseShader(const std::filesystem::path& filepath);
    
    // Argument(s) and Uniform(s)
    // ----------------------------------------
    void ReflectShaderBinding(void* arg, ShaderType type);
    void ReflectTextureBinding(const char* name, int32_t index, ShaderType type);
    void ReflectBufferBinding(void* arg, const char* name, int32_t index,
                              ShaderType type);
    void ReflectStructMembers(const std::string &uniformName, const std::string &parentName,
                              void* type);
    
    void ExtractShaderResources() override;
    
    void InitUniformBuffers();
    void UpdateUniformBuffer(const std::string& name);
    
    friend class MetalDrawable;
    
    // Shader variables
    // ----------------------------------------
private:
    ///< Shader program source.
    struct MetalShaderSource;
    std::shared_ptr<MetalShaderSource> m_ShaderSource;
    
    ///< List of texture uniforms supported by the shader.s
    Library<TextureElement> m_Textures;
    
    ///< Metal context.
    MetalContext* m_Context;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(MetalShader);
};

} // namespace pixc
