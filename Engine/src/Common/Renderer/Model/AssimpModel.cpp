#include "enginepch.h"
#include "Common/Renderer/Model/AssimpModel.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/**
 * Load the model from the specified file path.
 *
 * @param filePath The path to the model file.
 */
void AssimpModel::LoadModel(const std::filesystem::path &filePath)
{
    // Read the model file using the ASSIMP library
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);

    // Check for error(s) during loading
    bool success = scene && scene->mRootNode &&
                    !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE);
    CORE_ASSERT(success, "Error loading model with Assimp: " +
                std::string(importer.GetErrorString()));

    // Save the file path
    this->m_FilePath = filePath;

    // Process ASSIMP's root node recursively
    ProcessNode(scene->mRootNode, scene);
    importer.FreeScene();
    
    // Update the model matrix for the model
    this->UpdateModelMatrix();
}

/**
 * Processes the nodes in the ASSIMP scene recursively.
 *
 * @param node The current node being processed.
 * @param scene The ASSIMP scene containing the model data.
 */
void AssimpModel::ProcessNode(aiNode *node, const aiScene *scene)
{
    // Process all meshes inside each node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // The node object only contains indices to index the actual
        // objects in the scene. The scene contains all the data
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        this->m_Meshes.push_back(ProcessMesh(mesh));
    }

    // Then do the same for each child node
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

/**
 * Processes an ASSIMP mesh and creates a corresponding `Mesh` object.
 *
 * @param mesh The ASSIMP mesh to be processed.
 * @return The processed `Mesh` object.
 */
Mesh<AssimpVertexData> AssimpModel::ProcessMesh(aiMesh *mesh)
{
    // Mesh attributes
    // -----------------------
    std::vector<AssimpVertexData> vertices;
    std::vector<unsigned int> indices;
    BufferLayout layout = {
        { "a_Position", DataType::Vec4 },
        { "a_TextureCoord", DataType::Vec2 },
        { "a_Normal", DataType::Vec3 }
    };
    
    // Process the vertex data
    // -----------------------
    // Check each mesh
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        AssimpVertexData vertex;

        // Positions
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;
        vertex.position.w = 1.0f;

        // Texture coordinates
        if (mesh->mTextureCoords[0])
        { // contains uv's?
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        }
        else
            vertex.uv = glm::vec2(0.0f, 0.0f);

        // Normals
        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        // Define the vertex of the model
        vertices.push_back(vertex);
        // Update its bounding box
        this->UpdateBBoxWithVertex(vertex.position);
    }
    
    // Process indices
    // -----------------------
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        // Pass through each of the mesh's faces (triangle) and retrieve
        // the corresponding vertex indices
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    return Mesh<AssimpVertexData>(vertices, indices, layout);
}
