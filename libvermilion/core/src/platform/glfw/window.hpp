#ifdef VMCORE_GLFW
#pragma once

#include <vermilion/instance.hpp>

namespace Vermilion{
namespace Core{
namespace GLFW{

class Window : public Vermilion::Core::Window{
	public:

	private:
		Vermilion::Core::Instance * instance;

	public:
		Window(Vermilion::Core::Instance * instance);
		virtual ~Window() override;

	private:

};

}}}

#endif
