#pragma once

#include "Foundation/Renderer/RendererTypes.h"
#include "Foundation/Renderer/RendererAPI.h"

#include "Foundation/Renderer/Camera/Camera.h"

#include "Foundation/Renderer/Drawable/Drawable.h"

//#include "Foundation/Renderer/Buffer/IndexBuffer.h"
//#include "Foundation/Renderer/Buffer/FrameBuffer.h"

#include "Foundation/Renderer/Material/Material.h"

#include <glm/glm.hpp>

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * Responsible for rendering geometry using a specified shader program.
 *
 * The `Renderer` class serves as the central component for performing rendering operations. It
 * provides methods to clear the screen, set the clear color, and draw geometry using a `VertexArray`
 * object and a `Shader` program.
 */
class Renderer
{
public:
    // Initialization
    // ----------------------------------------
    static void Init();
    
    // Scene parametrization
    // ----------------------------------------
    static void BeginScene();
    static void BeginScene(const std::shared_ptr<Camera>& camera);
    static void BeginScene(const glm::mat4& view, const glm::mat4& projection,
                           const glm::vec3& position = glm::vec3(0.0f));
    
    static void EndScene();
    
    
    // Render
    // ----------------------------------------
    static void Draw(const std::shared_ptr<Drawable>& drawable,
                     const PrimitiveType &primitive = PrimitiveType::Triangle);
    
    static void Draw(const std::shared_ptr<Drawable>& drawable,
                     const std::shared_ptr<Material>& material,
                     const glm::mat4 &transform = glm::mat4(1.0f),
                     const PrimitiveType &primitive = PrimitiveType::Triangle);
    
    // Getters(s)
    // ----------------------------------------
    static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    
    static MaterialLibrary& GetMaterialLibrary() { return s_MaterialLibrary; }
    
    // Setter(s)
    // ----------------------------------------
    // TODO: move to renderer command
    //static void SetFaceCulling(const FaceCulling culling);
    //static void SetCubeMapSeamless(const bool enabled);
    
    // Statistics
    // ----------------------------------------
    /**
     * Represents the information related to the statistics of the rendering.
     */
    struct RenderingStatistics
    {
        ///< Number or rendering passes.
        uint32_t RenderPasses = 0;
        ///< Number of times the draw function is called.
        uint32_t DrawCalls = 0;
    };
    
    static void ResetStats();
    static RenderingStatistics GetStats();

    
    // Renderer Structures
    // ----------------------------------------
private:
    /**
     * @brief Represents the current information of the rendered scene (useful for the shading process).
     */
    struct SceneData
    {
        ///< View position.
        glm::vec3 ViewPosition = glm::vec3(0.0f);
        
        ///< View matrix.
        glm::mat4 ViewMatrix = glm::mat4(1.0f);
        ///< Projection matrix.
        glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
    };
    
    // Renderer variables
    // ----------------------------------------
private:
    ///< Scene current general information.
    static std::unique_ptr<SceneData> s_SceneData;
    
    ///< Rendering libraries.
    static inline MaterialLibrary s_MaterialLibrary;
};

} // namespace pixc
