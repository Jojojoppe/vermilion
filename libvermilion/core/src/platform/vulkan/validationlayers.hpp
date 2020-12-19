#ifdef VMCORE_VULKAN
#pragma once

#include <vector>

namespace Vermilion{
namespace Core{
namespace Vulkan{

	extern const std::vector<const char *> validationLayers;
	bool checkValidationLayerSupport();

}}}
#endif
