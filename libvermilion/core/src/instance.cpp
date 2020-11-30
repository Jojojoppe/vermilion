#include <vermilion/core/instance.hpp>

#include <vermilion/vermilion.hpp>

Vermilion::Core::Instance::Instance(){
	this->logger.VMCORE_LOGINFO("Vermilion core version %d.%d", VERMILION_VERSION_MAJOR, VERMILION_VERSION_MINOR);
	this->logger.VMCORE_LOGINFO("%s - %s [%s]", VERMILION_AUTHOR_NAME, VERMILION_AUTHOR_EMAIL, VERMILION_LICENCE);
	this->logger.VMCORE_LOGINFO("----");
}

Vermilion::Core::Instance::~Instance(){

}
