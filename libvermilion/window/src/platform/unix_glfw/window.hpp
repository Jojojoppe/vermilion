#ifdef VMWINDOW_UNIX_GLFW
#pragma once

#include <vermilion/window/window.hpp>

namespace Vermilion{
namespace Window{

class UNIX_GLFW_WindowInstance : public WindowInstance{
	public:
		UNIX_GLFW_WindowInstance(std::shared_ptr<Vermilion::Core::Instance> instance);
		~UNIX_GLFW_WindowInstance();
};

}}
#endif
