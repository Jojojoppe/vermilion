#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

#include <vermilion/Buffer.hpp>
#include <vermilion/Renderable.hpp>

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

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
