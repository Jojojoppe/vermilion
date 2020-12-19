#ifdef VMCORE_VULKAN

#include "api.hpp"
#include <vermilion/instance.hpp>
#include "validationlayers.hpp"

const bool enableValidationLayers = true;

Vermilion::Core::Vulkan::API::API(Vermilion::Core::Instance * instance){
	this->instance = instance;

	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Creating Vulkan context");
}

Vermilion::Core::Vulkan::API::~API(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Destroying Vulkan context");
	
	vkDestroyInstance(this->vk_instance, nullptr);
}

void Vermilion::Core::Vulkan::API::init(){
	this->instance->logger.log(VMCORE_LOGLEVEL_DEBUG, "Initializing Vulkan context");

	this->createInstance();

	// Pick GPU
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(this->vk_instance, &deviceCount, nullptr);
	if(deviceCount==0){
		this->instance->logger.log(VMCORE_LOGLEVEL_ERROR, "No GPU's found with Vulkan support");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(this->vk_instance, &deviceCount, devices.data());

}

void Vermilion::Core::Vulkan::API::createInstance(){
	if(enableValidationLayers && !Vermilion::Core::Vulkan::checkValidationLayerSupport()){
		this->instance->logger.log(VMCORE_LOGLEVEL_ERROR, "Requested validation layers not available");
	}

	// Create instance
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vermilion";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Vermilion Render Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	uint32_t requiredExtensionCount = 0;
	const char ** requiredExtensions;
	requiredExtensions = (const char **)this->instance->window->getRequiredExtensions(&requiredExtensionCount);

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = requiredExtensionCount;
	createInfo.ppEnabledExtensionNames = requiredExtensions;
	createInfo.enabledLayerCount = 0;
	if(enableValidationLayers){
		// Set validation layers
		createInfo.enabledLayerCount = static_cast<uint32_t>(Vermilion::Core::Vulkan::validationLayers.size());
		createInfo.ppEnabledLayerNames = Vermilion::Core::Vulkan::validationLayers.data();
	}
	if(vkCreateInstance(&createInfo, nullptr, &this->vk_instance) != VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_ERROR, "Failed to create Vulkan instance");
	}
}

void Vermilion::Core::Vulkan::API::startRender(){
}

void Vermilion::Core::Vulkan::API::endRender(){
}

#endif
