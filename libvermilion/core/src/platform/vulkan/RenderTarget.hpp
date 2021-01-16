#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>

#include "vkRenderPass.hpp"
#include "vkFrameBuffer.hpp"
#include <vermilion/RenderTarget.hpp>
#include <vermilion/Buffer.hpp>
#include <vermilion/Renderable.hpp>

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

class RenderTarget : public Vermilion::Core::RenderTarget{
	public:
		std::unique_ptr<vkRenderPass> renderpass;
		std::vector<std::unique_ptr<vkFrameBuffer>> framebuffers;

		std::vector<VkCommandBuffer> vk_commandBuffers;
		VkExtent2D extent;

	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		RenderTarget(Vermilion::Core::Instance * instance, int width, int height);
		RenderTarget(API * api);
		~RenderTarget();

		virtual void start() override;
		virtual void end() override;

		virtual void draw(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, std::shared_ptr<Vermilion::Core::Binding> binding, std::shared_ptr<Vermilion::Core::Renderable> renderable, int instanceCount, int firstInstance) override;

		void create();
		void reset();

	private:
};

}}}
#endif
