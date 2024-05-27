#include "enginepch.h"
#include "Common/Renderer/Shader/Shader.h"

#include <GL/glew.h>

// ----------------------------------------
// Shader
// ----------------------------------------

/**
 * Generate a shader program.
 *
 * @param name The name for the shader.
 * @param filePath Path to the source file.
 */
Shader::Shader(const std::string& name, const std::filesystem::path& filePath)
    : m_Name(name), m_FilePath(filePath)
{
    ShaderProgramSource source = ParseShader(filePath);
    m_ID = CreateShader(source.VertexSource, source.FragmentSource,
                        source.GeometrySource);
}

/**
 * Generate a shader program.
 *
 * @param filePath Path to the source file.
 */
Shader::Shader(const std::filesystem::path& filePath)
    : Shader(filePath.stem(), filePath)
{}

/**
 * Delete the shader program.
 */
Shader::~Shader()
{
    glDeleteProgram(m_ID);
}

/**
 * Activate the shader.
 */
void Shader::Bind() const
{
    glUseProgram(m_ID);
}

/**
 * Deactivate the shader.
 */
void Shader::Unbind() const
{
    glUseProgram(0);
}

/**
 * Get the location number of a uniform.
 *
 * @param name Name of the uniform.
 *
 * @return Uniform location.
 */
int Shader::GetUniformLocation(const std::string& name)
{
    // Verify that the location of the uniform is not cached
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];
    
    // Retrieve the location of the uniform and cache it too
    int location = glGetUniformLocation(m_ID, name.c_str());
    if (location == -1)
        CORE_WARN("Uniform " + name + " doesn't exist!");
    
    m_UniformLocationCache[name] = location;
    return location;
}

/**
 * Set the uniform with a bool value.
 *
 * @param name Uniform name.
 * @param value Uniform value.
 */
void Shader::SetBool(const std::string& name, bool value)
{
    glUniform1i(GetUniformLocation(name), (int)value);
}

/**
 * Set the uniform with an integer value.
 *
 * @param name Uniform name.
 * @param value Uniform value.
 */
void Shader::SetInt(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

/**
 * Set the uniform with a float value.
 *
 * @param name Uniform name.
 * @param value Uniform value.
 */
void Shader::SetFloat(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}

/**
 * Set the uniform with a vector with 2 values (x, y).
 *
 * @param name Uniform name.
 * @param value Vector input value.
 */
void Shader::SetVec2(const std::string& name, const glm::vec2& value)
{
    glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}

/**
 * Set the uniform with a vector with 3 values (x, y, z).
 *
 * @param name Uniform name.
 * @param value Vector input value.
 */
void Shader::SetVec3(const std::string& name, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}

/**
 * Set the uniform with a vector with 4 values (x, y, z, w).
 *
 * @param name Uniform name.
 * @param value Vector input value.
 */
void Shader::SetVec4(const std::string& name, const glm::vec4& value)
{
    glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}

/**
 * Set the uniform with a matrix with 2x2 values.
 *
 * @param name Uniform name.
 * @param value Matrix input value.
 */
void Shader::SetMat2(const std::string& name, const glm::mat2& value)
{
    glUniformMatrix2fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

/**
 * Set the uniform with a matrix with 3x3 values.
 *
 * @param name Uniform name.
 * @param value Matrix input value.
 */
void Shader::SetMat3(const std::string& name, const glm::mat3& value)
{
    glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

/**
 * Set the uniform with a matrix with 4x4 values.
 *
 * @param name Uniform name.
 * @param value Matrix input value.
 */
void Shader::SetMat4(const std::string& name, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

/**
 * Compile the shader from its input file source.
 *
 * @param type Shader type.
 * @param source Shader input source.
 *
 * @return the ID of the shader program.
 */
unsigned int Shader::CompileShader(unsigned int type, const std::string& source) 
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
unsigned int Shader::CreateShader(const std::string& vertexShader,
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
 * Parse shader input file.
 *
 * @param filepath Path to the shader file.
 *
 * @return The vertex and fragment program source.
 */
ShaderProgramSource Shader::ParseShader(const std::filesystem::path& filepath)
{
    // Open the file
    std::ifstream stream(filepath);
    
    // Define the different shader classes available
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2
    };
    
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
    return ShaderProgramSource(ss[0].str(), ss[1].str(), ss[2].str());
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
 * Adds a shader to the library.
 *
 * @param shader The shader to add.
 *
 * @note If a shader with the same name already exists in the library,
 *       an assertion failure will occur.
 */
void ShaderLibrary::Add(const std::shared_ptr<Shader> &shader)
{
    auto& name = shader->GetName();
    CORE_ASSERT(!Exists(name), "Shader already exists!");
    m_Shaders[name] = shader;
}

/**
 * Loads a shader from a file and adds it to the library.
 *
 * @param filePath The path to the file containing the shader.
 *
 * @return The loaded shader.
 */
std::shared_ptr<Shader> ShaderLibrary::Load(const std::filesystem::path& filePath)
{
    auto shader = std::make_shared<Shader>(filePath);
    Add(shader);
    return shader;
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
    auto shader = std::make_shared<Shader>(name, filePath);
    Add(shader);
    return shader;
}

/**
 * Retrieves a shader from the library by its name.
 *
 * @param name The name of the shader to retrieve.
 *
 * @return The retrieved shader.
 *
 * @note If the shader with the specified name does not exist in the library,
 *       an assertion failure will occur.
 */
std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name)
{
    CORE_ASSERT(Exists(name), "Shader not found!");
    return m_Shaders[name];
}

/**
 * Checks if a shader with a given name exists in the library.
 *
 * @param name The name of the shader to check for existence.
 *
 * @return True if a shader with the specified name exists in the library, otherwise false.
 */
bool ShaderLibrary::Exists(const std::string& name) const
{
    return m_Shaders.find(name) != m_Shaders.end();
}
