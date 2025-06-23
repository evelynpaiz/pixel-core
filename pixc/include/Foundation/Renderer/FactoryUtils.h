#pragma once

#ifdef __APPLE__
    #define CREATE_RENDERER_OBJECT(PointerType, ObjectType, ...)\
        switch (Renderer::GetAPI())\
        {\
            case RendererAPI::API::None:\
                PIXEL_CORE_ASSERT(false, "RendererAPI::None is not supported!");\
                return nullptr;\
            case RendererAPI::API::OpenGL:\
                return PointerType<OpenGL##ObjectType>(__VA_ARGS__);\
            case RendererAPI::API::Metal:\
                return PointerType<Metal##ObjectType>(__VA_ARGS__);\
        }\
        PIXEL_CORE_ASSERT(false, "Unknown Renderer API!");\
        return nullptr;
#else
    #define CREATE_RENDERER_OBJECT(PointerType, ObjectType, ...)\
        switch (Renderer::GetAPI())\
        {\
            case RendererAPI::API::None:\
                PIXEL_CORE_ASSERT(false, "RendererAPI::None is not supported!");\
                return nullptr;\
            case RendererAPI::API::OpenGL:\
                return PointerType<OpenGL##ObjectType>(__VA_ARGS__);\
        }\
        PIXEL_CORE_ASSERT(false, "Unknown Renderer API!");\
        return nullptr;
#endif
