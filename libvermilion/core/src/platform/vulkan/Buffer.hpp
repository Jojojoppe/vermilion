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

		VertexBuffer(API * api, void * data, size_t length);
		~VertexBuffer();
};

class IndexBuffer : public Vermilion::Core::IndexBuffer{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		VkBuffer vk_buffer;
		VmaAllocation vk_allocation;

		IndexBuffer(API * api, void * data, size_t length);
		~IndexBuffer();
};

}}}
#endif
