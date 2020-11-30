#pragma once

#include <vermilion/core/logger.hpp>

namespace Vermilion{
namespace Core{

class Instance{
	public:
		Logger logger;

	public:
		Instance();
		~Instance();
};

}}
