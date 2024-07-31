#include "enginepch.h"
#include "Platform/OpenGL/Shader/OpenGLShader.h"

#include "Platform/OpenGL/OpenGLRendererUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

/**
 * Generate a shader program.
 *
 * @param name The name for the shader.
 * @param filePath Path to the source file.
 */
OpenGLShader::OpenGLShader(const std::string& name, const std::filesystem::path& filePath)
    : Shader(name, filePath)
{
    // Parse the shader and divide it in the different program sources
    OpenGLShaderSource source = ParseShader(filePath);
    // Create the shader program
    m_ID = CreateShader(source.VertexSource, source.FragmentSource,
                        source.GeometrySource);
    // Define the shader attributes and uniforms
    ExtractShaderResources();
}

/**
 * Generate a shader program.
 *
 * @param filePath Path to the source file.
 */
OpenGLShader::OpenGLShader(const std::filesystem::path& filePath)
    : OpenGLShader(filePath.stem().string(), filePath)
{}

/**
 * Delete the shader program.
 */
OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(m_ID);
}

/**
 * Activate the shader.
 */
void OpenGLShader::Bind() const
{
    glUseProgram(m_ID);
}

/**
 * Deactivate the shader.
 */
void OpenGLShader::Unbind() const
{
    glUseProgram(0);
}

/**
 * Set the uniform with a bool value.
 *
 * @param name Uniform name.
 * @param value Uniform value.
 */
void OpenGLShader::SetBool(const std::string& name, bool value)
{
    auto& uniform = SetUniformData(name, value);
    if (uniform.Update)
        glUniform1i(uniform.Location, (int)value);
}

/**
 * Set the uniform with an integer value.
 *
 * @param name Uniform name.
 * @param value Uniform value.
 */
void OpenGLShader::SetInt(const std::string& name, int value)
{
    auto& uniform = SetUniformData(name, value);
    if (uniform.Update)
        glUniform1i(uniform.Location, value);
}

/**
 * Set the uniform with a float value.
 *
 * @param name Uniform name.
 * @param value Uniform value.
 */
void OpenGLShader::SetFloat(const std::string& name, float value)
{
    auto& uniform = SetUniformData(name, value);
    if (uniform.Update)
        glUniform1f(uniform.Location, value);
}

/**
 * Set the uniform with a vector with 2 values (x, y).
 *
 * @param name Uniform name.
 * @param value Vector input value.
 */
void OpenGLShader::SetVec2(const std::string& name, const glm::vec2& value)
{
    auto& uniform = SetUniformData(name, value);
    if (uniform.Update)
        glUniform2fv(uniform.Location, 1, &value[0]);
}

/**
 * Set the uniform with a vector with 3 values (x, y, z).
 *
 * @param name Uniform name.
 * @param value Vector input value.
 */
void OpenGLShader::SetVec3(const std::string& name, const glm::vec3& value)
{
    auto& uniform = SetUniformData(name, value);
    if (uniform.Update)
        glUniform3fv(uniform.Location, 1, &value[0]);
}

/**
 * Set the uniform with a vector with 4 values (x, y, z, w).
 *
 * @param name Uniform name.
 * @param value Vector input value.
 */
void OpenGLShader::SetVec4(const std::string& name, const glm::vec4& value)
{
    auto& uniform = SetUniformData(name, value);
    if (uniform.Update)
        glUniform4fv(uniform.Location, 1, &value[0]);
}

/**
 * Set the uniform with a matrix with 2x2 values.
 *
 * @param name Uniform name.
 * @param value Matrix input value.
 */
void OpenGLShader::SetMat2(const std::string& name, const glm::mat2& value)
{
    auto& uniform = SetUniformData(name, value);
    if (uniform.Update)
        glUniformMatrix2fv(uniform.Location, 1, GL_FALSE, &value[0][0]);
}

/**
 * Set the uniform with a matrix with 3x3 values.
 *
 * @param name Uniform name.
 * @param value Matrix input value.
 */
void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& value)
{
    auto& uniform = SetUniformData(name, value);
    if (uniform.Update)
        glUniformMatrix3fv(uniform.Location, 1, GL_FALSE, &value[0][0]);
}

/**
 * Set the uniform with a matrix with 4x4 values.
 *
 * @param name Uniform name.
 * @param value Matrix input value.
 */
void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
{
    auto& uniform = SetUniformData(name, value);
    if (uniform.Update)
        glUniformMatrix4fv(uniform.Location, 1, GL_FALSE, &value[0][0]);
}

/**
 * Compile the shader from its input file source.
 *
 * @param type Shader type.
 * @param source Shader input source.
 *
 * @return the ID of the shader program.
 */
unsigned int OpenGLShader::CompileShader(unsigned int type, const std::string& source)
{
    // Define the shader from the input source and compile
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        std::string shaderType;
        switch (type) {
        case GL_VERTEX_SHADER:
            shaderType = "vertex";
            break;
        case GL_FRAGMENT_SHADER:
            shaderType = "fragment";
            break;
        case GL_GEOMETRY_SHADER:
            shaderType = "geometry";
            break;
        default:
            shaderType = "unknown";
            break;
        }

        CORE_ERROR("Failed to compile " + shaderType + " shader!");
        CORE_ASSERT(result == GL_TRUE, message);
        glDeleteShader(id);
        return 0;
    }

    return id;
}

/**
 * Generate a shader program from vertex and fragment inputs.
 *
 * @param vertexShader Source of vertex shader.
 * @param fragmentShader Source of fragment shader.
 *
 * @return ID of the shader program.
 */
unsigned int OpenGLShader::CreateShader(const std::string& vertexShader,
                                        const std::string& fragmentShader,
                                        const std::string& geometryShader)
{
    // Define a shader program
    unsigned int program = glCreateProgram();
    
    // Vertex shader
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    glAttachShader(program, vs);
    // Fragment shader
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    glAttachShader(program, fs);
    // Geometry shader
    unsigned int gs = 0;
    if (!geometryShader.empty())
    {
        unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
        glAttachShader(program, gs);
    }
    
    // Link shaders
    glLinkProgram(program);
    glValidateProgram(program);
    
    // De-allocate the shader resources
    glDeleteShader(vs);
    glDeleteShader(fs);
    if (!geometryShader.empty())
        glDeleteShader(gs);
    
    // Return the shader program
    return program;
}

/**
 * Extracts and stores information about active attributes and uniforms from the shader program.
 *
 * @pre  The shader program should be successfully linked (`m_ID` should be valid).
 * @post  `m_Attributes` will contain information about the active attributes.
 * @post  `m_Uniforms` will contain information about the active uniforms.
 */
void OpenGLShader::ExtractShaderResources()
{
    // Counter of attributes/uniformss
    GLint count;
    // Buffer to store attribute/uniform values temporaly
    GLchar name[256];
    GLenum type;

    // Retrieve attributes
    glGetProgramiv(m_ID, GL_ACTIVE_ATTRIBUTES, &count);
    for (GLint i = 0; i < count; ++i)
    {
        glGetActiveAttrib(m_ID, i, sizeof(name), nullptr, nullptr, &type, name);
        
        DataElement element(utils::graphics::gl::ToDataType(type));
        m_Attributes.Add(name, element);
    }

    // Retrieve uniforms
    glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &count);
    for (GLint i = 0; i < count; ++i)
    {
        glGetActiveUniform(m_ID, i, sizeof(name), nullptr, nullptr, &type, name);

        auto [group, member] = utils::SplitString(name);
        GLint location = glGetUniformLocation(m_ID, name);

        UniformElement element(utils::graphics::gl::ToDataType(type));
        element.Location = static_cast<int32_t>(location);
        m_Uniforms.Add(group, member, element);
    }
}

/**
 * Parse shader input file.
 *
 * @param filepath Path to the shader file.
 *
 * @return The vertex and fragment program source.
 */
OpenGLShader::OpenGLShaderSource OpenGLShader::ParseShader(const std::filesystem::path& filepath)
{
    // Open the file
    std::ifstream stream(filepath);
    
    // Parse the file
    std::string line;
    std::stringstream ss[3];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            // Set mode to vertex
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            // Set mode to fragment
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
            // Set mode to fragment
            else if (line.find("geometry") != std::string::npos)
                type = ShaderType::GEOMETRY;
        }
        else
        {
            // Include statement handling
            if (line.find("#include") != std::string::npos)
            {
                std::string includePath = line.substr(line.find_first_of('"') + 1,
                                                      line.find_last_of('"') - line.find_first_of('"') - 1);
                std::string includedSource = ReadFile(includePath);
                ss[(int)type] << includedSource << '\n';
            }
            else
                ss[(int)type] << line << '\n';
        }
    }
    
    // Return the shader sources
    return OpenGLShaderSource(ss[0].str(), ss[1].str(), ss[2].str());
}
