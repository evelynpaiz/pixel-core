#pragma once

#include "Foundation/Renderer/Drawable/Mesh/Mesh.h"
#include "Foundation/Renderer/Drawable/Model/Model.h"

#include "Foundation/Renderer/Drawable/Mesh/MeshUtils.h"

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @namespace utils::geometry
 * @brief Utility functions and types related to geometric operations.
 */
namespace utils { namespace geometry
{
/**
 * @brief Generate a buffer layout for a vector of 3D vertices with position information.
 *
 * @param vertices A vector of 3D vertices with position information.
 *
 * @return The buffer layout specifying the layout of the vertex data in the buffer.
 */
inline BufferLayout BufferLayoutGeometry(std::vector<GeoVertexData<glm::vec4>>&)
{
    return {
        { "a_Position", { DataType::Vec4 } }
    };
}

/**
 * @brief Generate a buffer layout for a vector of 3D vertices with position and UV coordinate information.
 *
 * @param vertices A vector of 3D vertices with position and UV coordinate information.
 *
 * @return The buffer layout specifying the layout of the vertex data in the buffer.
 */
inline BufferLayout BufferLayoutGeometry(std::vector<GeoVertexData<glm::vec4, glm::vec2>>&)
{
    return {
        { "a_Position", { DataType::Vec4 } },
        { "a_TextureCoord", { DataType::Vec2 } }
    };
}

/**
 * @brief Generate a buffer layout for a vector of 3D vertices with position and normal vector information.
 *
 * @param vertices A vector of 3D vertices with position and normal vector information.
 *
 * @return The buffer layout specifying the layout of the vertex data in the buffer.
 */
inline BufferLayout BufferLayoutGeometry(std::vector<GeoVertexData<glm::vec4, glm::vec3>>&)
{
    return {
        { "a_Position", { DataType::Vec4 } },
        { "a_Normal", { DataType::Vec3 } }
    };
}

/**
 * @brief Generate a buffer layout for a vector of 3D vertices with position, UV coordinate, and normal vector information.
 *
 * @param vertices A vector of 3D vertices with position, UV coordinate, and normal vector information.
 *
 * @return The buffer layout specifying the layout of the vertex data in the buffer.
 */
inline BufferLayout BufferLayoutGeometry(std::vector<GeoVertexData<glm::vec4, glm::vec2, glm::vec3>>&)
{
    return {
        { "a_Position", { DataType::Vec4 } },
        { "a_TextureCoord", {  DataType::Vec2 } },
        { "a_Normal", { DataType::Vec3 } }
    };
}

/**
 * @brief Generate a model from a specified geometry and material.
 *
 * This function generates a model by defining the geometry using a specific vertex data type and providing a material.
 * The `DefineGeometry` function pointer is used to define the geometry data, such as vertices and indices.
 *
 * @tparam VertexData The type of vertex data used to define the geometry.
 *
 * @param material A shared pointer to the material to be applied to the model.
 * @param DefineGeometry A function pointer to a function that defines the geometry data.
 *                       The function must have the signature `void DefineGeometry(std::vector<VertexData>&, std::vector<uint32_t>&)`.
 *                       It should fill the provided vectors with vertex data and indices.
 *
 * @return The generated model with the specified geometry and material.
 */
template<typename VertexData>
inline std::shared_ptr<Model<VertexData>>
GenerateModel(void (*DefineGeometry)(std::vector<VertexData>&, std::vector<uint32_t>&),
              const std::shared_ptr<Material>& material)
{
    std::vector<VertexData> vertices;
    std::vector<uint32_t> indices;
    DefineGeometry(vertices, indices);
    
    BufferLayout layout = BufferLayoutGeometry(vertices);
    
    Mesh<VertexData> mesh;
    mesh.DefineMesh(vertices, indices, layout);
    
    if (material)
        mesh.SetMaterial(material);
    
    return std::make_shared<Model<VertexData>>(mesh);
}

/**
 * @brief Generate a model for a plane using the specified material.
 *
 * @tparam VertexData The type of vertex data used to define the geometry.
 *
 * @param material A shared pointer to the material to be applied to the model.
 *
 * @return The generated model for the plane with the specified material.
 */
template<typename VertexData>
inline std::shared_ptr<Model<VertexData>>
ModelPlane(const std::shared_ptr<Material>& material = nullptr)
{
    return GenerateModel<VertexData>(utils::geometry::DefinePlaneGeometry, material);
}

/**
 * @brief Generate a model for a cube using the specified material.
 *
 * @tparam VertexData The type of vertex data used to define the geometry.
 *
 * @param material A shared pointer to the material to be applied to the model.
 *
 * @return The generated model for the cube with the specified material.
 */
template<typename VertexData>
inline std::shared_ptr<Model<VertexData>>
ModelCube(const std::shared_ptr<Material>& material = nullptr)
{
    return GenerateModel<VertexData>(utils::geometry::DefineCubeGeometry, material);
}

/**
 * @b rief Generate a model for a sphere using the specified material.
 *
 * @tparam VertexData The type of vertex data used to define the geometry.
 *
 * @param material A shared pointer to the material to be applied to the model.
 * @param radius The radius of the sphere.
 * @param sectorCount The number of sectors (longitude divisions) in the sphere.
 * @param stackCount The number of stacks (latitude divisions) in the sphere.
 *
 * @return The generated model for the sphere with the specified material.
 */
template<typename VertexData>
inline std::shared_ptr<Model<VertexData>>
ModelSphere(const std::shared_ptr<Material>& material = nullptr)
{
    return GenerateModel<VertexData>(utils::geometry::DefineSphereGeometry, material);
}

} // namespace Geometry
} // namespace utils
} // namespace pixc
