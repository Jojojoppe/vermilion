#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

#include <vermilion/Buffer.hpp>
#include "vk_mem_alloc.h"

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

class VertexBuffer : public Vermilion::Core::VertexBuffer{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		VkBuffer vk_buffer;
		VmaAllocation vk_allocation;

		VertexBuffer(API * api, std::vector<float>& vertices);
		~VertexBuffer();
};

class IndexBuffer : public Vermilion::Core::IndexBuffer{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		VkBuffer vk_buffer;
		VmaAllocation vk_allocation;

		IndexBuffer(API * api, std::vector<unsigned int>& indices);
		~IndexBuffer();
};

}}}
#endif
