#pragma once

/*
 * Vermilion basic information
 * ---------------------------
 */
#define VERMILION_VERSION_MAJOR 0
#define VERMILION_VERSION_MINOR 1

#define VERMILION_AUTHOR_NAME "Joppe Blondel"
#define VERMILION_AUTHOR_EMAIL "vermilion@joppeb.nl"

#define VERMILION_LICENCE "BSD-2"
/*
 * ---------------------------
 */

#include <vermilion/instance.hpp>
#include <memory>

typedef Vermilion::Core::Instance VmInstance;
typedef std::shared_ptr<Vermilion::Core::RenderTarget> VmRenderTarget;
typedef std::shared_ptr<Vermilion::Core::Shader> VmShader;
typedef std::shared_ptr<Vermilion::Core::ShaderProgram> VmShaderProgram;
typedef std::shared_ptr<Vermilion::Core::Pipeline> VmPipeline;
typedef std::shared_ptr<Vermilion::Core::VertexBuffer> VmVertexBuffer;
typedef std::shared_ptr<Vermilion::Core::IndexBuffer> VmIndexBuffer;
typedef std::shared_ptr<Vermilion::Core::UniformBuffer> VmUniformBuffer;
typedef std::shared_ptr<Vermilion::Core::Renderable> VmRenderable;
typedef std::shared_ptr<Vermilion::Core::Texture> VmTexture;
typedef std::shared_ptr<Vermilion::Core::Sampler> VmSampler;
typedef std::shared_ptr<Vermilion::Core::Binding> VmBinding;