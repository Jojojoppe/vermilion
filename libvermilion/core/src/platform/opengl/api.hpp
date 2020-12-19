#ifdef VMCORE_OPENGL
#pragma once

#include <vermilion/instance.hpp>

namespace Vermilion{
namespace Core{
namespace OpenGL{

class API : public Vermilion::Core::API{
	public:

	private:
		Vermilion::Core::Instance * instance;

	public:
		API(Vermilion::Core::Instance * instance);
		virtual ~API() override;

		virtual void init() override;
		virtual void startRender() override;
		virtual void endRender() override;

	private:

};

}}}

#endif
