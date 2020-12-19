#ifdef VMCORE_VULKAN
#include "validationlayers.hpp"

#include <vector>
#include <stdint.h>
#include <string.h>
#include <vulkan/vulkan.h>

namespace Vermilion{
namespace Core{
namespace Vulkan{

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

bool checkValidationLayerSupport(){
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for(const char* layerName : validationLayers){
		bool layerFound = false;

		for(const auto& layerProperties : availableLayers){
			if(strcmp(layerName, layerProperties.layerName) == 0){
				layerFound = true;
				break;
			}
		}
		if(!layerFound){
			return false;
		}
	}
	return true;
}

}}}
#endif
