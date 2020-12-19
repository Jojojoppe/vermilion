#ifdef VMCORE_VULKAN
#pragma once

#include <vermilion/instance.hpp>

namespace Vermilion{
namespace Core{
namespace Vulkan{

class API : public Vermilion::Core::API{
	public:

	private:
		Vermilion::Core::Instance * instance;

	public:
		API(Vermilion::Core::Instance * instance);
		virtual ~API() override;

	private:

};

}}}

#endif
