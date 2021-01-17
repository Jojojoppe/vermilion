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

class vkRenderPass{
	public:
		VkRenderPass vk_renderPass;
	
	private:
		API * api;
		Vermilion::Core::Instance * instance;

	public:
		vkRenderPass(API * api, VkFormat format, VkFormat depthFormat);
		~vkRenderPass();

	private:
};

}}}
#endif
