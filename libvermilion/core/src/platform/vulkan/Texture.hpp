#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>
#include <stdint.h>
#include <memory>
#include <string>
#include <vector>
#include <vermilion/Texture.hpp>
#include "vk_mem_alloc.h"
#include "vkImageView.hpp"

namespace Vermilion{
namespace Core{

class Instance;

namespace Vulkan{

class API;

class Texture : public Vermilion::Core::Texture{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:

		VkImage vk_image;
		VmaAllocation vma_allocation;
		std::unique_ptr<vkImageView2D> vk_imageView;

		Texture(API * api, const std::string& path, size_t width, size_t height, unsigned int channels);
		~Texture();
};

class Sampler : public Vermilion::Core::Sampler{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:

		VkSampler vk_sampler;
		std::shared_ptr<Texture> texture;

		Sampler(API * api, std::shared_ptr<Vermilion::Core::Texture> texture);
		~Sampler();
};

}}}
#endif
