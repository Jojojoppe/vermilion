#ifdef VMCORE_OPENGL
#pragma once

#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

#include <glad/glad.h>
#include "../../Buffer.hpp"
#include "../../Renderable.hpp"

namespace Vermilion{
namespace Core{

class Instance;

namespace OpenGL{

class API;

class Renderable : public Vermilion::Core::Renderable{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		Renderable(API * api, std::shared_ptr<Vermilion::Core::Buffer> vertexBuffer, std::shared_ptr<Vermilion::Core::Buffer> indexBuffer, unsigned int vertexOffset, unsigned int indexOffset, unsigned int length);
		~Renderable();
};

}}}
#endif
