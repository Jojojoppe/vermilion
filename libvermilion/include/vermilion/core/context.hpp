#pragma once

#include <memory>
#include <vector>
#include <string>

#define VMCORE_API_OPENGL 1
#define VMCORE_API_VULKAN 2

namespace Vermilion{
namespace Core{

class Instance;

struct ContextProperties{
	int hintAPI;

	int width;
	int height;
	int API;

#ifdef VMCORE_OPENGL
	void * opengl_procAddress;
#endif
};

class Context{
	public:

	public:
		Context(){};
		~Context() = default;

		virtual void init(ContextProperties * properties){};
};

}}
