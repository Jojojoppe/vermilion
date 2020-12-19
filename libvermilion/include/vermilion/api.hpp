#pragma once

#include <string>

namespace Vermilion{
namespace Core{

/**
 * @brief Type of render API
 */
enum RenderPlatform{
	RENDER_PLATFORM_NONE = 0,
	RENDER_PLATFORM_OPENGL,
	RENDER_PLATFORM_VULKAN
};
const std::string RenderPlatformString[] = {
	"NONE",
	"OpenGL",
	"Vulkan"
};

extern const int renderPlatform[];

class API{

};

}}
