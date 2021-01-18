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

class VertexBuffer : public Vermilion::Core::VertexBuffer{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		VkBuffer vk_buffer;
		VmaAllocation vk_allocation;

		VertexBuffer(API * api, std::vector<float>& vertices, Vermilion::Core::BufferType type);
		~VertexBuffer();

		virtual void setData(void * data, size_t size) override;
};

class IndexBuffer : public Vermilion::Core::IndexBuffer{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		VkBuffer vk_buffer;
		VmaAllocation vk_allocation;

		IndexBuffer(API * api, std::vector<unsigned int>& indices, Vermilion::Core::BufferType type);
		~IndexBuffer();

		virtual void setData(void * data, size_t size) override;
};

class UniformBuffer : public Vermilion::Core::UniformBuffer{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		VkBuffer vk_buffer;
		VmaAllocation vk_allocation;

		UniformBuffer(API * api, size_t size, Vermilion::Core::BufferType type);
		~UniformBuffer();

		virtual void setData(void * data, size_t size) override;
};

}}}
#endif
