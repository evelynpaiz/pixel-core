#pragma once

// --------------------------------------------
// Logger
// --------------------------------------------
#include "Common/Core/Base.h"
#include "Common/Core/Log.h"
#include "Common/Core/Assert.h"

// --------------------------------------------
// Application
// --------------------------------------------
#include "Common/Core/Window.h"
#include "Common/Core/Application.h"

// --------------------------------------------
// Inputs
// --------------------------------------------
#include "Common/Input/Input.h"
#include "Common/Input/KeyCodes.h"
#include "Common/Input/MouseCodes.h"

// --------------------------------------------
// Events
// --------------------------------------------
#include "Common/Event/Event.h"
#include "Common/Event/KeyEvent.h"
#include "Common/Event/MouseEvent.h"
#include "Common/Event/WindowEvent.h"

// --------------------------------------------
// Base Layer(s)
// --------------------------------------------
#include "Common/Layer/Layer.h"
#include "Common/Layer/GuiLayer.h"

#include "Common/Layer/DrawUtils.h"

// --------------------------------------------
// Renderer
// --------------------------------------------
#include "Common/Renderer/Buffer/VertexBuffer.h"
#include "Common/Renderer/Buffer/IndexBuffer.h"
#include "Common/Renderer/Buffer/FrameBuffer.h"

#include "Common/Renderer/Shader/Shader.h"
#include "Common/Renderer/Texture/Texture.h"
#include "Common/Renderer/Texture/Texture1D.h"
#include "Common/Renderer/Texture/Texture2D.h"
#include "Common/Renderer/Texture/Texture3D.h"
#include "Common/Renderer/Texture/TextureCube.h"

#include "Common/Renderer/Light/ShadowCamera.h"
#include "Common/Renderer/Light/Light.h"
#include "Common/Renderer/Light/PositionalLight.h"
#include "Common/Renderer/Light/DirectionalLight.h"
#include "Common/Renderer/Light/EnvironmentLight.h"

#include "Common/Renderer/Material/Material.h"
#include "Common/Renderer/Material/LightedMaterial.h"
#include "Common/Renderer/Material/SimpleMaterial.h"
#include "Common/Renderer/Material/PhongMaterial.h"

#include "Common/Renderer/Drawable/Drawable.h"
#include "Common/Renderer/Drawable/Mesh/Mesh.h"
#include "Common/Renderer/Drawable/Model/Model.h"
#include "Common/Renderer/Drawable/Model/AssimpModel.h"

#include "Common/Renderer/Drawable/Mesh/MeshUtils.h"
#include "Common/Renderer/Drawable/Model/ModelUtils.h"

#include "Common/Renderer/Camera/PerspectiveCamera.h"
#include "Common/Renderer/Camera/OrthographicCamera.h"

#include "Common/Renderer/Renderer.h"

// --------------------------------------------
// Rendering Context & Scene
// --------------------------------------------
#include "Common/Scene/Viewport.h"
#include "Common/Scene/Scene.h"
