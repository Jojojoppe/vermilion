#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>

#include "vkRenderPass.hpp"
#include <vermilion/RenderTarget.hpp>

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class RenderTarget : public Vermilion::Core::RenderTarget{
	public:
		std::unique_ptr<vkRenderPass> renderpass;

	private:
		Vermilion::Core::Instance * instance;

	public:
		RenderTarget(Vermilion::Core::Instance * instance, int width, int height);
		~RenderTarget();

	private:
};

}}}
#endif
