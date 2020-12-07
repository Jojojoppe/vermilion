#pragma once

#include <vermilion/core/logger.hpp>
#include <vermilion/core/context.hpp>
#include <memory>

namespace Vermilion{
namespace Core{

class Instance{
	public:
		Logger logger;
	private:
		std::unique_ptr<Vermilion::Core::Context> context;

	public:
		Instance();
		~Instance();

		void createContext(ContextProperties * properties);
		void initContext(ContextProperties * properties);
};

}}
