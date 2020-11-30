#include <vermilion/core/instance.hpp>
#include <vermilion/vermilion.hpp>

Vermilion::Core::Instance::Instance(){
	this->logger.log(VMCORE_LOGLEVEL_INFO, "Vermilion core version %d.%d", VERMILION_VERSION_MAJOR, VERMILION_VERSION_MINOR);
	this->logger.log(VMCORE_LOGLEVEL_INFO, "%s - %s [%s]", VERMILION_AUTHOR_NAME, VERMILION_AUTHOR_EMAIL, VERMILION_LICENCE);
	this->logger.log(VMCORE_LOGLEVEL_INFO, "----");
}

Vermilion::Core::Instance::~Instance(){

}
