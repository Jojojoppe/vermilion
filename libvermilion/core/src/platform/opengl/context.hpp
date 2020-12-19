#ifdef VMCORE_OPENGL
#pragma once

#include <vermilion/core/context.hpp>

namespace Vermilion{
namespace Core{

class Instance;

class OPENGL_Context : public Context{
	public:
		Instance * vmCoreInstance;

	private:
		int glVersionMajor;
		int glVersionMinor;

	public:
		OPENGL_Context(Instance * instance);
		~OPENGL_Context();

		virtual void init(ContextProperties * properties) override;
};

}}
#endif
