#pragma once

#include <string>

namespace Vermilion{
namespace Core{

/**
 * @brief Type of windowing platform
 */
enum WindowPlatform{
	WINDOW_PLATFORM_NONE = 0,
	WINDOW_PLATFORM_GLFW
};
const std::string WindowPlatformString[] = {
	"NONE",
	"GLFW"
};


extern const int windowPlatform[];

class Instance;

class Window{
	public:

	private:

	public:
		static Window * create(int platform, Instance * instance);
		virtual ~Window() = default;

	private:
};

}}
