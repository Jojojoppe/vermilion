#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>

#include "vkRenderPass.hpp"
#include "vkFrameBuffer.hpp"
#include <vermilion/RenderTarget.hpp>

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

class RenderTarget : public Vermilion::Core::RenderTarget{
	public:
		std::unique_ptr<vkRenderPass> renderpass;
		std::vector<std::unique_ptr<vkFrameBuffer>> framebuffers;

	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		RenderTarget(Vermilion::Core::Instance * instance, int width, int height);
		RenderTarget(API * api);
		~RenderTarget();

	private:
};

}}}
#endif
