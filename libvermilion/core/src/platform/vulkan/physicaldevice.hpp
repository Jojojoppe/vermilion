#ifdef VMCORE_VULKAN
#pragma once

#include <vulkan/vulkan.h>

namespace Vermilion{
namespace Core{
namespace Vulkan{

bool isDeviceSuitable(VkPhysicalDevice device);

}}}
#endif
