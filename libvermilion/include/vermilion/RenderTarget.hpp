#pragma once

#include <vermilion/instance.hpp>
#include <vermilion/window.hpp>
#include <vermilion/logger.hpp>

#include <memory>

namespace Vermilion{
namespace Core{

class RenderTarget{
	public:
		~RenderTarget() = default;

		static RenderTarget * create(Vermilion::Core::Instance * instance, int width, int height);
};

}}
