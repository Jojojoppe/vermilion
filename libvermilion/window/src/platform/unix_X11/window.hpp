#ifdef VMWINDOW_UNIX_X11
#pragma once

#include <vermilion/window/window.hpp>

namespace Vermilion{
namespace Window{

class UNIX_X11_WindowInstance : public WindowInstance{
	public:
		UNIX_X11_WindowInstance(std::shared_ptr<Vermilion::Core::Instance> instance);
		~UNIX_X11_WindowInstance();
};

}}
#endif
