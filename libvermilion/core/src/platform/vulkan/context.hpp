#ifdef VMCORE_VULKAN
#pragma once

#include <vermilion/core/context.hpp>

#include <vulkan/vulkan.hpp>

namespace Vermilion{
namespace Core{

class Instance;

class VULKAN_Context : public Context{
	public:
		Instance * vmCoreInstance;

	private:

	public:
		VULKAN_Context(Instance * instance);
		~VULKAN_Context();

		virtual void init(ContextProperties * properties) override;
};

}}
#endif
