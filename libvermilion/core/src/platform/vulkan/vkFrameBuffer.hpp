#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <vector>
#include <optional>
#include <memory>

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

class vkFrameBuffer{
	public:
		VkFramebuffer vk_frameBuffer;

	private:
		API * api;
		Vermilion::Core::Instance * instance;

	public:
		vkFrameBuffer(API * api, int width, int height, VkImageView imageview, VkRenderPass renderpass);
		~vkFrameBuffer();

	private:
};

}}}
#endif
