#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>

#include "vkRenderPass.hpp"
#include "vkFrameBuffer.hpp"
#include "../../RenderTarget.hpp"
#include "../../Buffer.hpp"
#include "../../Renderable.hpp"
#include "vk_mem_alloc.h"
#include "vkImageView.hpp"
#include "Texture.hpp"

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

class RenderTarget : public Vermilion::Core::RenderTarget{
		public:
		std::unique_ptr<vkRenderPass> renderpass;
		std::unique_ptr<vkFrameBuffer> framebuffer;

		VkCommandBuffer vk_commandBuffer;

		std::shared_ptr<Texture> texture;

		VkImage depthImage;
		VmaAllocation depthImageMemory;
		std::unique_ptr<vkImageView2D> depthImageView;

	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		RenderTarget(API * api, std::shared_ptr<Vermilion::Core::Texture> texture);
		~RenderTarget();

		virtual void reset();

		virtual void start(float r, float g, float b, float a) override;
		virtual void end() override;

		virtual void draw(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, std::shared_ptr<Vermilion::Core::Binding> binding, std::shared_ptr<Vermilion::Core::Renderable> renderable, int instanceCount, int firstInstance) override;

		virtual void setUniform(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, const std::string& name, void * data) override;

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
};

class DefaultRenderTarget : public RenderTarget{
	public:
		std::vector<std::unique_ptr<vkFrameBuffer>> framebuffers;

		std::vector<VkCommandBuffer> vk_commandBuffers;
		VkExtent2D extent;

		VkImage depthImage;
		VmaAllocation depthImageMemory;
		std::unique_ptr<vkImageView2D> depthImageView;

	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		DefaultRenderTarget(API * api, std::shared_ptr<Vermilion::Core::Texture> texture = nullptr);
		~DefaultRenderTarget();

		virtual void start(float r, float g, float b, float a) override;
		virtual void end() override;

		virtual void draw(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, std::shared_ptr<Vermilion::Core::Binding> binding, std::shared_ptr<Vermilion::Core::Renderable> renderable, int instanceCount, int firstInstance) override;

		virtual void setUniform(std::shared_ptr<Vermilion::Core::Pipeline> pipeline, const std::string& name, void * data) override;

		void create();
		virtual void reset() override;
};

}}}
#endif
