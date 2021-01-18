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

void createBuffer(API * api, size_t size, VkBufferUsageFlags usage, VmaMemoryUsage properties, VkBuffer& buffer, VmaAllocation& memory);

class Buffer : public Vermilion::Core::Buffer{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		VkBuffer vk_buffer;
		VmaAllocation vk_allocation;

		Buffer(API * api, size_t, Vermilion::Core::BufferType type, Vermilion::Core::BufferUsage usage, Vermilion::Core::BufferDataUsage dataUsage);
		~Buffer();

		virtual void setData(void * data, size_t size) override;
		virtual void getData(void * data, size_t size) override;
};

}}}
#endif
