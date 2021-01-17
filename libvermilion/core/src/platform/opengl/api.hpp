#ifdef VMCORE_OPENGL
#pragma once

#include <vermilion/instance.hpp>

namespace Vermilion{
namespace Core{
namespace OpenGL{

class API : public Vermilion::Core::API{
	public:

	private:
		Vermilion::Core::Instance * instance;

	public:
		API(Vermilion::Core::Instance * instance);
		virtual ~API() override;

		virtual void init() override;
		virtual void startRender() override;
		virtual void endRender(std::initializer_list<std::shared_ptr<Vermilion::Core::RenderTarget>> extraRenderTargets) override;

		virtual std::shared_ptr<Vermilion::Core::RenderTarget> getDefaultRenderTarget() override;

		virtual std::shared_ptr<Vermilion::Core::Shader> createShader(const std::string& source, Vermilion::Core::ShaderType type) override;
		virtual std::shared_ptr<Vermilion::Core::ShaderProgram> createShaderProgram(std::initializer_list<std::shared_ptr<Vermilion::Core::Shader>> shaders) override;

	private:

};

}}}

#endif
