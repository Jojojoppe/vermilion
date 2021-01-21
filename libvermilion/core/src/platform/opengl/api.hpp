#ifdef VMCORE_OPENGL
#pragma once

#include <vermilion/instance.hpp>
#include "../../api.hpp"

namespace Vermilion{
namespace Core{
namespace OpenGL{

class API : public Vermilion::Core::API{
	public:
		Vermilion::Core::Instance * instance;

	public:
		API(Vermilion::Core::Instance * instance);
		virtual ~API() override;

		virtual void init() override;
		virtual void startRender() override;
		virtual void endRender(std::vector<std::shared_ptr<Vermilion::Core::RenderTarget>>& extraRenderTargets) override;

        virtual std::shared_ptr<Vermilion::Core::Texture> createTexture(size_t width, size_t height, unsigned int channels) override;
        virtual std::shared_ptr<Vermilion::Core::Sampler> createSampler(std::shared_ptr<Vermilion::Core::Texture> texture);

        virtual std::shared_ptr<Vermilion::Core::Shader> createShader(const std::string& source, Vermilion::Core::ShaderType type) override;
        virtual std::shared_ptr<Vermilion::Core::ShaderProgram> createShaderProgram(std::vector<std::shared_ptr<Vermilion::Core::Shader>>& shaders) override;

		virtual std::shared_ptr<Vermilion::Core::RenderTarget> getDefaultRenderTarget() override;

	private:

};

}}}

#endif