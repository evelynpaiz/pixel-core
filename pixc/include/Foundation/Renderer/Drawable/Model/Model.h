#pragma once

#include "Foundation/Core/ClassUtils.h"
#include "Foundation/Core/Library.h"

#include "Foundation/Renderer/Drawable/Mesh/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents a bounding box around a 3D model.
 *
 * The `BBox` structure defines a bounding box by storing the minimum and maximum coordinates
 * along the x, y, and z axes. It represents the extent or volume occupied by a 3D model.
 */
struct BBox
{
    glm::vec3 min = { 0.0f, 0.0f, 0.0f };   ///< The minimum coordinates of the bounding box.
    glm::vec3 max = { 0.0f, 0.0f, 0.0f };   ///< The maximum coordinates of the bounding box.
};

/**
 * @brief Represents a basic model used for rendering geometry.
 *
 * The `BaseModel` class provides functionality for drawing the model with a specified transformation
 * matrix and primitive type. It encapsulates information about the model's position, rotation, scale,
 * model matrix, and up axis direction. Derived classes must implement protected virtual methods
 * for updating the model matrix.
 */
class BaseModel
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Delete the base model.
    virtual ~BaseModel() = default;
    
    // Render
    // ----------------------------------------
    /// @brief Draw the model using the specified transformation matrix.
    /// @param transform The transformation matrix for the model.
    /// @param primitive The type of primitive to be drawn (e.g., Points, Lines, Triangles).
    virtual void DrawModelWithTransform(const glm::mat4 &transform = glm::mat4(1.0f)) = 0;
    /// @brief Draw the model using the model matrix transformation.
    /// @param primitive The type of primitive to be drawn (e.g., Points, Lines, Triangles).
    void DrawModel()
    {
        UpdateModelMatrix();
        DrawModelWithTransform(m_ModelMatrix);
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the model position (x, y, z).
    /// @return The model position coordinates.
    const glm::vec3& GetPosition() const { return m_Position; }
    /// @brief Get the model orientation (yaw, pitch, roll).
    /// @return The model rotation angles.
    const glm::vec3& GetRotation() const { return m_Rotation; }
    
    /// @brief Get the model matrix (transformation from model space to world space).
    /// @return The view matrix.
    const glm::mat4& GetModelMatrix() const { return m_ModelMatrix; }
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Sets the material for all the meshes in the model.
    /// @param material The material defining the surface of the meshes.
    virtual void SetMaterial(const std::shared_ptr<Material>& material) = 0;
    
    /// @brief Change the model position (x, y, z).
    /// @param position The model center position.
    void SetPosition(const glm::vec3 &position)
    {
        m_Position = position;
        UpdateModelMatrix();
    }
    /// @brief Change the model orientation (yaw, pitch, roll).
    /// @param orientation The model rotation angles.
    void SetRotation(const glm::vec3 &rotation)
    {
        m_Rotation = rotation;
        UpdateModelMatrix();
    }
    /// @brief Set the scaling factor for the model in the x, y, and z axis.
    /// @param position The model scaling factor.
    void SetScale(const glm::vec3 &scale)
    {
        m_Scale = scale;
        UpdateModelMatrix();
    }
    /// @brief Set the up axis for the model.
    /// @param upAxis A vector representing the up axis.
    void SetUpAxis(const glm::vec3 &upAxis)
    {
        m_UpAxis = glm::normalize(upAxis);
        UpdateModelMatrix();
    }
    
protected:
    // Constructor(s)
    // ----------------------------------------
    /// @brief Define a base model.
    /// @param primitives The type of the primitive that defines the model.
    BaseModel(const PrimitiveType &primitive)
    : m_Primitive(primitive)
    {}
    
    // Transformation matrices
    // ----------------------------------------
    virtual void UpdateModelMatrix() = 0;
    
    // Model variables
    // ----------------------------------------
protected:
    ///< Model position (x, y, z).
    glm::vec3 m_Position = glm::vec3(0.0f);
    /// Model orientation (yaw, pitch and roll angles).
    glm::vec3 m_Rotation = glm::vec3(0.0f);
    ///< Model scale factor.
    glm::vec3 m_Scale = glm::vec3(1.0f);
    
    ///< Model matrix.
    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
    ///< Model up axis direction.
    glm::vec3 m_UpAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    
    ///< Primitive type defined for the model.
    PrimitiveType m_Primitive;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(BaseModel);
};

/**
 * @brief A library for managing models used in rendering.
 *
 * The `ModelLibrary` class provides functionality to add, load, retrieve, and check
 * for the existence of models within the library. Each model is associated with
 * a unique name.
 */
class ModelLibrary : public Library<std::shared_ptr<BaseModel>>
{
public:
    // Constructor
    // ----------------------------------------
    /// @brief Create a new model library.
    ModelLibrary() : Library("Model") {}
    
    // Create
    // ----------------------------------------
    /// @brief Loads a model and adds it to the library.
    /// @tparam Type The type of object to load.
    /// @tparam Args The types of arguments to forward to the object constructor.
    /// @param name The name to associate with the loaded object.
    /// @param args The arguments to forward to the object constructor.
    /// @return The model created.
    template<typename Type, typename... Args>
    std::shared_ptr<Type> Create(const std::string& name, Args&&... args)
    {
        auto model = std::make_shared<Type>(std::forward<Args>(args)...);
        
        std::string message = GetTypeName() + " '" + name + "' is not of the specified type!";
        PIXEL_CORE_ASSERT(std::dynamic_pointer_cast<BaseModel>(model), message);
        
        Add(name, model);
        return model;
    }
};

/**
 * @brief Represents a model used for rendering geometry.
 *
 * The `Model` class inherits from the `BaseModel` class and provides additional
 * functionality specific to defining models. It encapsulates information required to and render
 * a model, such as a collection of meshes.
 *
 * @tparam VertexData The type of vertex data used by the meshes in the model.
 */
template<typename VertexData>
class Model : public BaseModel
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Define a model.
    /// @param primitive The primitive type of the model.
    Model(const PrimitiveType &primitive = PrimitiveType::Triangle) : BaseModel(primitive) {};
    /// @brief Define with a specific mesh.
    /// @param mesh The set of meshes defining the model.
    /// @param primitive The primitive type of the model.
    Model(const Mesh<VertexData>& mesh,
          const PrimitiveType &primitive = PrimitiveType::Triangle)
    : BaseModel(primitive)
    {
        m_Meshes.push_back(mesh);
    }
    /// @brief Delete the model.
    virtual ~Model() = default;
    
    // Render
    // ----------------------------------------
    /// @brief Draw the model using the specified transformation matrix.
    /// @param transform The transformation matrix for the model.
    /// @param primitive The type of primitive to be drawn (e.g., Points, Lines, Triangles).
    void DrawModelWithTransform(const glm::mat4 &transform = glm::mat4(1.0f)) override
    {
        for(unsigned int i = 0; i < m_Meshes.size(); i++)
            m_Meshes[i].DrawMesh(transform, m_Primitive);
    }
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the number of meshes representing the model.
    /// @return The number of meshes.
    int GetMeshNumber() const { return (int)m_Meshes.size(); }
    
    // Setter(s)
    // ----------------------------------------
    /// @brief Sets the material for all the meshes in the model.
    /// @param material The material defining the surface of the meshes.
    void SetMaterial(const std::shared_ptr<Material>& material) override
    {
        for(unsigned int i = 0; i < m_Meshes.size(); i++)
            m_Meshes[i].SetMaterial(material);
    }
    /// @brief Sets the material for a specific mesh in the model.
    /// @param index The index of the mesh to set the material for.
    /// @param material The material defining the surface of the mesh.
    void SetMaterialForMesh(unsigned int index, const std::shared_ptr<Material>& material)
    {
        if (index >= 0 && index < m_Meshes.size())
            m_Meshes[index].SetMaterial(material);
    }
    
protected:
    // Bounding box definition
    // ----------------------------------------
    void UpdateBBoxWithVertex(const glm::vec3 &v);
    
    // Transformation matrices
    // ----------------------------------------
    void UpdateModelMatrix() override;
    
    // Model variables
    // ----------------------------------------
protected:
    ///< Bounding box.
    BBox m_BBox;
    ///< Set of meshes defining the model.
    std::vector<Mesh<VertexData>> m_Meshes;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(Model);
};

/**
 * @brief Represents a model loaded from a file.
 *
 * The `LoadedModel` class inherits from the Model class and adds functionality specific to
 * loading models from a file. It encapsulates methods for loading meshes and associated data
 * from a file and populating the model with the loaded data.
 *
 * @tparam VertexData The type of vertex data used by the meshes in the model.
 */
template<typename VertexData>
class LoadedModel : public Model<VertexData>
{
public:
    // Destructor
    // ----------------------------------------
    /// @brief Delete the model.
    ~LoadedModel() override = default;
    
    // Loading
    // ----------------------------------------
    /// @brief Load the model from the specified file path.
    /// @param filePath The path to the model file.
    virtual void LoadModel(const std::filesystem::path& filePath) = 0;
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Get the file path of the model.
    /// @return The path to the file.
    std::filesystem::path GetPath() const { return m_FilePath; }
    /// @brief Get the name of the loaded model (file name).
    /// @return The model name.
    std::string GetName() { return m_FilePath.filename().string(); }
    /// @brief Get the directory where the model file is located.
    /// @return The directory of the model.
    std::string GetDirectory() { return m_FilePath.parent_path().string(); }
    
protected:
    /// @brief Define a model from a file source.
    /// @param filePath The path to the model file.
    /// @param primitive The primitive type of the model.
    LoadedModel(const std::filesystem::path& filePath,
                const PrimitiveType &primitive)
    : Model<VertexData>(primitive), m_FilePath(filePath)
    {}
    
    // Model variables
    // ----------------------------------------
protected:
    ///< Path to the file.
    std::filesystem::path m_FilePath;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(LoadedModel);
};

/**
 * @brief Update the boundaries of the bounding box using a vertex coordinate.
 *
 * @param v The vertex coordinate to be considered when updating the bounding box.
 */
template<typename VertexData>
void Model<VertexData>::UpdateBBoxWithVertex(const glm::vec3& v)
{
    // Check if the x-coordinate is smaller than the current minimum x-coordinate
    if (v.x < m_BBox.min.x)
        m_BBox.min.x = v.x;
    // Check if the x-coordinate is greater than the current maximum x-coordinate
    if (v.x > m_BBox.max.x)
        m_BBox.max.x = v.x;
    
    // Check if the y-coordinate is smaller than the current minimum y-coordinate
    if (v.y < m_BBox.min.y)
        m_BBox.min.y = v.y;
    // Check if the y-coordinate is greater than the current maximum y-coordinate
    if (v.y > m_BBox.max.y)
        m_BBox.max.y = v.y;
    
    // Check if the z-coordinate is smaller than the current minimum z-coordinate
    if (v.z < m_BBox.min.z)
        m_BBox.min.z = v.z;
    // Check if the z-coordinate is greater than the current maximum z-coordinate
    if (v.z > m_BBox.max.z)
        m_BBox.max.z = v.z;
}

/**
 * @brief Update the model matrix with translation, scaling, and rotation transformations.
 */
template<typename VertexData>
void Model<VertexData>::UpdateModelMatrix()
{
    // Get the size of the model and its center position
    glm::vec3 size = m_BBox.max - m_BBox.min;
    glm::vec3 center = (m_BBox.max + m_BBox.min) / 2.0f;
    
    // Reset the model matrix to the identity
    m_ModelMatrix = glm::mat4(1.0f);
    
    // 1. Translate (and center) to the selected position
    m_ModelMatrix = glm::translate(m_ModelMatrix, -center);
    m_ModelMatrix = glm::translate(m_ModelMatrix, m_Position);
    
    // Translate back to the center
    m_ModelMatrix = glm::translate(m_ModelMatrix, center);
    
    // 2. Scale the model using the scaling factor
    m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
    
    // 3. Rotate with the selected user angle around the center
    m_ModelMatrix *= glm::toMat4(glm::quat(glm::radians(m_Rotation)));
    
    // 4. Rotate the model if the up-axis is defined as other than the Y-axis
    glm::vec3 referenceAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    float epsilon = std::numeric_limits<float>::epsilon();
    
    if (glm::abs(glm::dot(referenceAxis, m_UpAxis) - 1.0f) > epsilon)
    {
        float angle = glm::acos(glm::dot(referenceAxis, m_UpAxis));
        glm::vec3 axis = glm::normalize(glm::cross(referenceAxis, m_UpAxis));
        m_ModelMatrix = glm::rotate(m_ModelMatrix, angle, axis);
    }
    
    // Translate back to the original position
    m_ModelMatrix = glm::translate(m_ModelMatrix, -center);
}

} // namespace pixc
