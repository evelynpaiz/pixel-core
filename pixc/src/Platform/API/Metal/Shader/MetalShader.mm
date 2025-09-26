#include "pixcpch.h"
#include "Platform/Metal/Shader/MetalShader.h"

#include "Platform/Metal/MetalRendererUtils.h"
#include "Platform/Metal/Texture/MetalTexture.h"

#include <Metal/Metal.h>

namespace pixc {

/**
 * @brief Internal representation of compiled Metal shader source.
 */
struct MetalShader::MetalShaderSource
{
    ///< Shading library
    id<MTLLibrary> Library;
    
    ///< Compiled Metal vertex function.
    id<MTLFunction> VertexFunction;
    ///< Compiled Metal fragment function.
    id<MTLFunction> FragmentFunction;
    
    ///< Uniforms buffer.
    std::vector<id<MTLBuffer>> UniformBuffers;
};

/**
 * @brief Generate a shader program.
 *
 * @param name The name for the shader.
 * @param filePath Path to the source file.
 */
MetalShader::MetalShader(const std::string& name, const std::filesystem::path& filePath)
    : Shader(name, filePath)
{
    // Get the Metal graphics context and save it
    MetalContext& context = dynamic_cast<MetalContext&>(GraphicsContext::Get());
    PIXEL_CORE_ASSERT(&context, "Graphics context is not Metal!");
    m_Context = &context;
    
    // Initialize
    m_ShaderSource = std::make_shared<MetalShaderSource>();
    // Compile the source file and define the shader functions
    CompileShader(filePath);
    
    // Define the shader attributes and uniforms
    ExtractShaderResources();
    InitUniformBuffers();
}

/**
 * @brief Generate a shader program.
 *
 * @param filePath Path to the source file.
 */
MetalShader::MetalShader(const std::filesystem::path& filePath)
    : MetalShader(filePath.stem().string(), filePath)
{}

/**
 * @brief Delete the shader program.
 */
MetalShader::~MetalShader()
{}

/**
 * @brief Activate the shader.
 */
void MetalShader::Bind() const
{}

/**
 * @brief Deactivate the shader.
 */
void MetalShader::Unbind() const
{}

/**
 * @brief Gets the compiled Metal vertex function.
 *
 * @return A pointer to the compiled Metal vertex function as a void pointer,
 * or nullptr if the function was not compiled or found.
 */
void* MetalShader::GetVertexFunction() const
{
    return reinterpret_cast<void*>(m_ShaderSource->VertexFunction);
}

/**
 * @brief Gets the compiled Metal fragment function.
 *
 * @return A pointer to the compiled Metal vertex function as a void pointer,
 * or nullptr if the function was not compiled or found.
 */
void* MetalShader::GetFragmentFunction() const
{
    return reinterpret_cast<void*>(m_ShaderSource->FragmentFunction);
}

/**
 * @brief Set the uniform with a bool value.
 *
 * @param name Uniform name.
 * @param value Uniform value.
 */
void MetalShader::SetBool(const std::string& name, bool value)
{
    SET_UNIFORM(name, value, UpdateUniformBuffer(name));
}

/**
 * @brief Set the uniform with an integer value.
 *
 * @param name Uniform name.
 * @param value Uniform value.
 */
void MetalShader::SetInt(const std::string& name, int value)
{
    SET_UNIFORM(name, value, UpdateUniformBuffer(name));
}

/**
 * @brief Set the uniform with a float value.
 *
 * @param name Uniform name.
 * @param value Uniform value.
 */
void MetalShader::SetFloat(const std::string& name, float value)
{
    SET_UNIFORM(name, value, UpdateUniformBuffer(name));
}

/**
 * @brief Set the uniform with a vector with 2 values (x, y).
 *
 * @param name Uniform name.
 * @param value Vector input value.
 */
void MetalShader::SetVec2(const std::string& name, const glm::vec2& value)
{
    SET_UNIFORM(name, value, UpdateUniformBuffer(name));
}

/**
 * @brief Set the uniform with a vector with 3 values (x, y, z).
 *
 * @param name Uniform name.
 * @param value Vector input value.
 */
void MetalShader::SetVec3(const std::string& name, const glm::vec3& value)
{
    SET_UNIFORM(name, value, UpdateUniformBuffer(name));
}

/**
 * @brief Set the uniform with a vector with 4 values (x, y, z, w).
 *
 * @param name Uniform name.
 * @param value Vector input value.
 */
void MetalShader::SetVec4(const std::string& name, const glm::vec4& value)
{
    SET_UNIFORM(name, value, UpdateUniformBuffer(name));
}

/**
 * @brief Set the uniform with a matrix with 2x2 values.
 *
 * @param name Uniform name.
 * @param value Matrix input value.
 */
void MetalShader::SetMat2(const std::string& name, const glm::mat2& value)
{
    SET_UNIFORM(name, value, UpdateUniformBuffer(name));
}

/**
 * @brief Set the uniform with a matrix with 3x3 values.
 *
 * @param name Uniform name.
 * @param value Matrix input value.
 */
void MetalShader::SetMat3(const std::string& name, const glm::mat3& value)
{
    SET_UNIFORM(name, value, UpdateUniformBuffer(name));
}

/**
 * @brief Set the uniform with a matrix with 4x4 values.
 *
 * @param name Uniform name.
 * @param value Matrix input value.
 */
void MetalShader::SetMat4(const std::string& name, const glm::mat4& value)
{
    SET_UNIFORM(name, value, UpdateUniformBuffer(name));
}

/**
 * @brief Set a texture map in the shader program.
 *
 * @param texture The texture map.
 * @param name Uniform name.
 * @param slot The texture slot.
 */
void MetalShader::SetTexture(const std::string &name,
                             const std::shared_ptr<Texture>& texture,
                             int slot)
{
    // Return early if the texture pointer is null
    if(!texture)
        return;
    
    @autoreleasepool
    {
        // Get the current Metal render command encoder
        auto encoder = reinterpret_cast<id<MTLRenderCommandEncoder>>(m_Context->GetCommandEncoder());
        
        // Dynamically cast the Texture to a MetalTexture
        auto metalTexture = std::dynamic_pointer_cast<MetalTexture>(texture);
        if (!metalTexture)
            return;
        
        // Get the Metal texture and sampler
        auto rawTexture = reinterpret_cast<id<MTLTexture>>(metalTexture->MTLGetTexture());
        auto rawSampler = reinterpret_cast<id<MTLSamplerState>>(metalTexture->MTLGetSampler());
        
        // Get the texture uniform information
        auto [group, member] = utils::SplitString(name);
        std::string uniformName = group + (member.empty() ? member : ("_" + member));
        auto& uniform = m_Textures.Get(uniformName);
        
        // Bind the texture to the appropriate shader stages based on its associated shader types
        for (const auto& type : uniform.ShaderTypes)
        {
            switch (type)
            {
                case ShaderType::VERTEX:
                    [encoder setVertexTexture:rawTexture atIndex:slot];
                    [encoder setVertexSamplerState:rawSampler atIndex:slot];
                    break;
                case ShaderType::FRAGMENT:
                    [encoder setFragmentTexture:rawTexture atIndex:slot];
                    [encoder setFragmentSamplerState:rawSampler atIndex:slot];
                    break;
                default:
                    PIXEL_CORE_WARN("Unsupported shader type for texture!");
                    break;
            }
        }
    } // autoreleasepool
}

/**
 * @brief Returns a shared MTLVertexDescriptor used for shader reflection.
 *
 * The descriptor is created once and reused for the lifetime of the application.
 *
 * @return A pointer to the shared MTLVertexDescriptor instance.
 */
void* MetalShader::GetShaderVertexDescriptor()
{
    // Static descriptor, initialized only once
    static MTLVertexDescriptor* descriptor = nil;
    
    if (descriptor)
        return descriptor;
    
    @autoreleasepool
    {
        // Initialize the vertex descriptor
        descriptor = [[MTLVertexDescriptor alloc] init];
        
        // Define a dummy buffer layout with a single attribute (position vector)
        BufferLayout layout = {
            { "a_Position", DataType::Vec4 },
            { "a_TextureCoord", DataType::Vec2 },
            { "a_Normal", DataType::Vec3 }
        };
        
        // Fill in attribute descriptions
        int index = 0;
        for (const auto& name : layout.GetBufferOrder())
        {
            // Define the vertex attribute
            auto& element = layout.Get(name);
            auto *attribute = descriptor.attributes[index];
            attribute.format = utils::graphics::mtl::ToMetalFormat(element.Type);
            attribute.offset = element.Offset;
            attribute.bufferIndex = 0;
            index++;
        }
        
        // Set layout properties
        auto *layouts = descriptor.layouts[0];
        layouts.stride = layout.GetStride();
        
        return descriptor;
    } // autoreleasepool
}

/**
 * @brief Compiles the shader source code for Metal.
 *
 * @param filePath Path to the source file.
 */
void MetalShader::CompileShader(const std::filesystem::path& filePath)
{
    @autoreleasepool {
        // Get the Metal device from the context
        auto device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
        
        // Load the shader source program
        std::string sourceFromFile = ParseShader(filePath);
        NSString* sourceCode = [NSString stringWithUTF8String:sourceFromFile.c_str()];
        
        // Compile the shader source code into a Metal library
        NSError* error = nil;
        m_ShaderSource->Library = [device newLibraryWithSource:sourceCode options:nil error:&error];
        PIXEL_CORE_ASSERT(!error, "Failed to compiled shader!");
        
        // Define the shader functions
        m_ShaderSource->VertexFunction = [m_ShaderSource->Library newFunctionWithName:@"vertex_main"];
        m_ShaderSource->FragmentFunction = [m_ShaderSource->Library newFunctionWithName:@"fragment_main"];
        
        // Note: Metal Shaders need to have a function called "vertex_main"
        // for the vertex shader and "fragment_main" for the fragment shader
        PIXEL_CORE_ASSERT(m_ShaderSource->VertexFunction, "Failed to create vertex shader function 'vertex_main'!");
        PIXEL_CORE_ASSERT(m_ShaderSource->FragmentFunction, "Failed to create fragment shader function 'fragment_main'!");
    } // autoreleasepool
}

/**
 * @brief Parse shader input file.
 *
 * @param filepath Path to the shader file.
 *
 * @return The program source.
 */
std::string MetalShader::ParseShader(const std::filesystem::path& filepath)
{
    // Open the file
    std::ifstream stream(filepath);
    
    // Parse the file
    std::string line;
    std::stringstream ss;
    while (getline(stream, line))
    {
        // Include statement handling
        if (line.find("#import") != std::string::npos)
        {
            std::string includePath = line.substr(line.find_first_of('"') + 1,
                                                  line.find_last_of('"') - line.find_first_of('"') - 1);
            std::string includedSource = ReadFile(includePath);
            ss << includedSource;
        }
        else
            ss << line << '\n';
    }
    
    // Return the shader sources
    return ss.str();
}

/**
 * @brief Processes a texture argument from Metal reflection data.
 *
 * @param name  The name of the texture uniform.
 * @param index The binding index of the texture in the shader.
 * @param type  The shader type (e.g., Vertex, Fragment) associated with the texture.
 */
void MetalShader::ReflectTextureBinding(const char* name, int32_t index, ShaderType type)
{
    // Verify if the texture uniform already exists with the same binding index
    if (m_Textures.Exists(name))
    {
        auto& uniform = m_Textures.Get(name);
        if (uniform.Index == index)
        {
            // Add the shader type to the list
            uniform.AddShaderType(type);
            return;
        }
        else
        {
            PIXEL_CORE_WARN("Texture uniform {0} repeated with different binding indices!", name);
            return;
        }
    }
    
    // Define a new texture uniform and add it to the collection
    TextureElement uniform(index);
    uniform.AddShaderType(type);
    m_Textures.Add(name, uniform);
}

/**
 * @brief Reflects a Metal buffer binding and extracts its layout information.
 *
 * @param arg   A `void*` pointer to the `MTLArgument` object.
 * @param name  The name of the uniform group (buffer).
 * @param index The binding index of the buffer in the shader.
 * @param type  The shader stage (e.g., Vertex, Fragment) associated with the buffer.
 */
void MetalShader::ReflectBufferBinding(void *arg, const char *name, int32_t index,
                                       ShaderType type)
{
    @autoreleasepool
    {
        // Check if this buffer uniform already exists in the registry
        if (m_Uniforms.Library<Uniform>::Exists(name))
        {
            auto& uniform = m_Uniforms.Library<Uniform>::Get(name);
            
            // Update shader type if binding matches
            if (uniform.GetIndex() == index)
            {
                uniform.AddShaderType(type);
                return;
            }

            // Log warning for mismatched binding indices
            PIXEL_CORE_WARN("Uniform {0} repeated with different binding indices!", name);
            return;
        }

        // Cast the raw argument to Metal buffer binding type
        auto argument = reinterpret_cast<id<MTLBufferBinding>>(arg);
        
        // If this buffer contains a struct, process its members recursively
        if (argument.bufferDataType == MTLDataTypeStruct)
        {
            ReflectStructMembers("", name, argument.bufferStructType);
        }
        else
        {
            // Otherwise, treat it as a single uniform element
            UniformElement element(utils::graphics::mtl::ToDataType(argument.bufferDataType));
            m_Uniforms.Add(name, "", element);
        }

        // Finalize metadata for the newly added uniform
        auto& uniform = m_Uniforms.Library<Uniform>::Get(name);
        uniform.SetIndex(index);
        uniform.AddShaderType(type);
    } // autoreleasepool
}

/**
 * @brief Recursively reflects all members of a Metal struct type.
 *
 * @param uniformName The current nested name of the struct member (e.g., "Transform.Model").
 * @param parentName  The top-level buffer name to which these members belong.
 * @param type A `void*` pointer to the `MTLStructType` being processed.
 */
void MetalShader::ReflectStructMembers(const std::string &uniformName, const std::string &parentName,
                                       void *type)
{
    // Cast the raw argument to Metal struct type
    auto structType = reinterpret_cast<MTLStructType*>(type);
    
    // Iterate over all members of the struct
    for (MTLStructMember* member in structType.members)
    {
        const char* memberNameCStr = [member.name UTF8String];
        
        // Build full hierarchical member name (e.g., "Transform.Model" or "Bones[0].Position")
        std::string memberName = uniformName.empty()
                                   ? memberNameCStr
                                   : uniformName + "." + memberNameCStr;

        // Handle arrays of basic types or structs
        if (member.dataType == MTLDataTypeArray)
        {
            MTLArrayType* arrayType = member.arrayType;
            NSUInteger length = arrayType.arrayLength;

            for (NSUInteger i = 0; i < length; ++i)
            {
                std::string indexedName = fmt::format("{}[{}]", memberName, i);

                // Recurse if array contains structs, otherwise add as uniform element
                if (arrayType.elementType == MTLDataTypeStruct)
                {
                    ReflectStructMembers(indexedName, parentName, arrayType.elementStructType);
                }
                else
                {
                    UniformElement element(utils::graphics::mtl::ToDataType(arrayType.elementType));
                    m_Uniforms.Add(parentName, indexedName, element);
                }
            }
        }
        // Handle nested structs (non-array)
        else if (member.dataType == MTLDataTypeStruct)
        {
            ReflectStructMembers(memberName, parentName, member.structType);
        }
        // Handle basic types
        else
        {
            UniformElement element(utils::graphics::mtl::ToDataType(member.dataType));
            m_Uniforms.Add(parentName, memberName, element);
        }
    }
}

/**
 * @brief Processes a single shader argument (uniform) from Metal reflection data.
 *
 * @param arg  A `void*` pointer to the `MTLArgument` object.
 * @param type The shader type associated with the argument.
 */
void MetalShader::ReflectShaderBinding(void* arg, ShaderType type)
{
    @autoreleasepool
    {
        // Get the Metal argument definition
        auto argument = reinterpret_cast<id<MTLBinding>>(arg);
        
        // Ignore inactive arguments
        if (!argument.isUsed)
            return;
        
        // Get the uniform group name (assuming the "u_" prefix convention)
        const char* name = [argument.name UTF8String];
        if (strncmp(name, "u_", 2) != 0)
            return;
        
        // Get the binding index for the uniform buffer
        int32_t index = static_cast<int32_t>(argument.index);
        
        switch (argument.type)
        {
                // Uniform is a texture
            case MTLBindingTypeTexture:
            {
                ReflectTextureBinding(name, index, type);
                return;
            }
                // Uniform is a buffer
            case MTLBindingTypeBuffer:
            {
                ReflectBufferBinding(arg, name, index, type);
                break;
            }
            default:
                PIXEL_CORE_ASSERT(true, "Metal argument type not supported!");
        }
    } // autoreleasepool
}

/**
 * @brief Extracts and stores information about active attributes and uniforms from the shader program.
 *
 * @param descriptor A pointer to the `MTLRenderPipelineDescriptor` object.
 *
 * @post  `m_Attributes` will contain information about the active attributes.
 * @post  `m_Uniforms` will contain information about the active uniforms.
 */
void MetalShader::ExtractShaderResources()
{
    @autoreleasepool
    {
        // Return early if the data has been already set
        if (!m_Attributes.IsEmpty() || !m_Uniforms.IsEmpty())
            return;
        
        // Get the Metal device from the context
        auto device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
        
        // Define a Metal pipeline descriptor
        MTLRenderPipelineDescriptor* pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineDescriptor.vertexFunction = m_ShaderSource->VertexFunction;
        pipelineDescriptor.fragmentFunction = m_ShaderSource->FragmentFunction;
        pipelineDescriptor.vertexDescriptor = reinterpret_cast<MTLVertexDescriptor*>(GetShaderVertexDescriptor());
        pipelineDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
        
        // Obtain shader reflection information
        NSError *error = nil;
        MTLRenderPipelineReflection *reflection = nil;
        MTLPipelineOption option = MTLPipelineOptionBindingInfo | MTLPipelineOptionBufferTypeInfo;
        
        [device
            newRenderPipelineStateWithDescriptor:pipelineDescriptor
            options:option
            reflection:&reflection
            error:&error
        ];
        PIXEL_CORE_ASSERT(!error, "Error creating a reflection of the shader program!");
        
        // Process vertex and fragment uniforms
        for (id<MTLBinding> argument in reflection.vertexBindings)
            ReflectShaderBinding(reinterpret_cast<void*>(argument), ShaderType::VERTEX);
        for (id<MTLBinding> argument in reflection.fragmentBindings)
            ReflectShaderBinding(reinterpret_cast<void*>(argument), ShaderType::FRAGMENT);
        
        // Release the pipeline descriptor
        [pipelineDescriptor release];
    } // autoreleasepool
}

/**
 * @brief Initializes Metal uniform buffers for each uniform block in the shader.
 *
 * @note  Assumes that `m_Uniforms` has been populated with valid `UniformLayout`
 *        objects and that the `DataElement` objects within the layouts have
 *        accurate data pointers (`Data`), sizes (`Size`), and offsets (`Offset`).
 */
void MetalShader::InitUniformBuffers()
{
    @autoreleasepool
    {
        // Retrieve the Metal device from the rendering context
        auto device = reinterpret_cast<id<MTLDevice>>(m_Context->GetDevice());
        // Iterate over each uniform block layout defined for the shader
        for (auto& [uniform, layout] : m_Uniforms)
        {
            // Determine the required buffer size for this uniform layout
            uint32_t stride = layout.GetStride();
            // Create a new Metal buffer for this uniform block
            id<MTLBuffer> buffer = [device
                                        newBufferWithLength:stride
                                        options:MTLResourceStorageModeShared
            ];
            // Store the buffer in the shader source's uniform buffer list
            m_ShaderSource->UniformBuffers.emplace_back(buffer);
            // Link the buffer to the uniform
            layout.SetBufferOfData(reinterpret_cast<void*>(buffer));
        }
    } // autoreleasepool
}

/**
 * @brief Updates a specific uniform buffer if any of its members have changed.
 *
 *@param name The name of the uniform to be updated.
 *
 * @note  Assumes that `m_Uniforms` has been populated with valid `UniformLayout`
 *        objects and that the `DataElement` objects within the layouts have
 *        accurate data pointers (`Data`), sizes (`Size`), and offsets (`Offset`).
 */
void MetalShader::UpdateUniformBuffer(const std::string& name)
{
    // Ensure the uniform with the given name exists in the shader
    PIXEL_CORE_ASSERT(IsUniform(name), "Uniform '" + name + "' not found!");
    
    // Split the full uniform name into the group and member parts
    auto [groupName, memberName] = utils::SplitString(name);
    
    // Retrieve the uniform from the library
    auto& uniform = m_Uniforms.Library<Uniform>::Get(groupName);
    
    // Get the total size (stride) of this uniform group’s buffer layout
    uint32_t stride = uniform.GetStride();
    
    // Get the Metal buffer that stores the raw uniform data for this group
    id<MTLBuffer> buffer = reinterpret_cast<id<MTLBuffer>>(uniform.GetBufferOfData());
    // Obtain a writable pointer to the buffer's contents
    char* content = reinterpret_cast<char*>(buffer.contents);
    
    // Retrieve the specific uniform member from the uniform library by group and member name
    auto& element = uniform.GetElement(memberName);
    // Ensure that copying this uniform’s data will not overflow the allocated buffer size
    PIXEL_CORE_ASSERT(element.Offset + element.Size <= stride, "Uniform data overflow!");
    // Copy the member’s raw data into the appropriate offset within the buffer’s contents
    std::memcpy(content + element.Offset, element.Data, element.Size);
}

} // namespace pixc
