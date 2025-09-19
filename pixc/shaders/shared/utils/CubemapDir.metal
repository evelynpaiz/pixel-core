/**
 * @brief Converts a direction vector to match Metal's cubemap coordinate convention.
 *
 * @param dir A 3D direction vector in world or sample space.
 * 
 * @return The converted 3D direction vector suitable for sampling a Metal cubemap.
 */
inline float3 toCubemapDir(float3 dir)
{
    dir.y = -dir.y;
    return dir;
}
