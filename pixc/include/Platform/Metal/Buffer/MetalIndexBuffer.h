#pragma once

#include "Common/Renderer/Buffer/IndexBuffer.h"

/**
 * @namespace pixc
 * @brief Main namespace of the Pixel Core rendering engine.
 */
namespace pixc {

/**
 * @brief Concrete implementation of `IndexBuffer` for the Metal rendering API.
 *
 * The `MetalIndexBuffer` handles the creation, binding, and management of index buffers
 * specifically for Metal. It uses Metal functions to interact with the GPU.
 *
 * Copying or moving `MetalIndexBuffer` objects is disabled to ensure single ownership
 * and prevent unintended buffer duplication.
 */
class MetalIndexBuffer : public IndexBuffer
{
public:
    // Constructor(s)/Destructor
    // ----------------------------------------
    MetalIndexBuffer(const uint32_t *indices, const uint32_t count);
    /// @brief Delete the index buffer.
    ~MetalIndexBuffer() override = default;
    
    // Usage
    // ----------------------------------------
    /// @brief Bind the index buffer.
    /// @note Not necessary for Metal API.
    void Bind() const override {};
    /// @brief Unbind the index buffer.
    /// @note Not necessary for Metal API.
    void Unbind() const override {};
    
    // Getter(s)
    // ----------------------------------------
    /// @brief Returns the instance of this as a Metal Buffer.
    /// @returns The index buffer as a void pointer.
    void* GetBuffer() const { return m_Buffer; }
    
    // Index buffer variables
    // ----------------------------------------
private:
    ///< Index buffer.
    void* m_Buffer;
    
    // Disable the copying or moving of this resource
    // ----------------------------------------
public:
    DISABLE_COPY_AND_MOVE(MetalIndexBuffer);
};

} // namespace pixc
