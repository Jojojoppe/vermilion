#ifdef VMCORE_OPENGL
#pragma once

#include <stdint.h>
#include <memory>
#include <string>
#include <vector>
#include "../../Texture.hpp"
#include <glad/glad.h>

namespace Vermilion{
namespace Core{

class Instance;

namespace OpenGL{

class API;

class Texture : public Vermilion::Core::Texture{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		unsigned int texture;
		unsigned int format;

		Texture(API * api, size_t width, size_t height, unsigned int channels);
		~Texture();

		virtual void setData(void * data, size_t size) override;
};

class Sampler : public Vermilion::Core::Sampler{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		std::shared_ptr<Texture> texture;
		unsigned int sampler;

		Sampler(API * api, std::shared_ptr<Vermilion::Core::Texture> texture);
		~Sampler();
};

}}}
#endif
