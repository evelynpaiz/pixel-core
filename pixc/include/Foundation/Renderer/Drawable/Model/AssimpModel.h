#pragma once

#include "Foundation/Renderer/Drawable/Mesh/Mesh.h"
#include "Foundation/Renderer/Drawable/Model/Model.h"

#include <glm/glm.hpp>

struct aiNode;
struct aiScene;
struct aiMesh;

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Represents the different data that a vertex from an assimp model contains.
 */
struct AssimpVertexData
{
    glm::vec4 position;         ///< Vertex position.
    glm::vec2 uv;               ///< Texture coordinate.
    glm::vec3 normal;           ///< Normal vector.
};

/**
 * @brief Represents a model loaded using the ASSIMP library.
 *
 * The `AssimpModel` class extends the base `Model` class and provides functionality for loading
 *  and processing models. It inherits the ability to load and render meshes from the base class and
 * adds specific processing using ASSIMP, such as parsing nodes and meshes from an ASSIMP scene.
 */
class AssimpModel : public LoadedModel<AssimpVertexData>
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    /// @brief Define an assimp model from a file source.
    /// @param filePath The path to the model file.
    /// @param primitive The primitive type of the model.
    AssimpModel(const std::filesystem::path& filePath,
                const PrimitiveType &primitive = PrimitiveType::Triangle)
    : LoadedModel<AssimpVertexData>(filePath, primitive)
    {
        LoadModel(filePath);
    }
    
    /// @brief Delete the model.
    virtual ~AssimpModel() override = default;
    
    // Loading
    // ----------------------------------------
    virtual void LoadModel(const std::filesystem::path& filePath) override;
    
private:
    // Mesh processing
    // ----------------------------------------
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh<AssimpVertexData> ProcessMesh(aiMesh *mesh);
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(AssimpModel);
};

} // namespace pixc
