#ifdef VMCORE_VULKAN
#include "physicaldevice.hpp"

namespace Vermilion{
namespace Core{
namespace Vulkan{

bool isDeviceSuitable(VkPhysicalDevice device){
	return true;
}

}}}

#endif
