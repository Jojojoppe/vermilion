#pragma once

#include <vermilion/api.hpp>
#include <vermilion/window.hpp>
#include <vermilion/logger.hpp>

namespace Vermilion{
namespace Core{

/**
 * @brief Hint types
 */
enum HintType{
	HINT_TYPE_NONE = 0,
	HINT_TYPE_WINDOW_PLATFORM,
	HINT_TYPE_RENDER_PLATFORM,
};

/**
 * @brief The instance class it the most top level class of Vermilion
 */
class Instance{
	public:
		Logger logger;

	private:

	public:

		/**
		 * @brief Create Vermilion instance
		 *
		 * @param hintType List of types of setup hints, terminated with zero
		 * @param hintValue List of values of setup hints, terminated with zero
		 */
		Instance(int * hintType, int * hintValue);

		~Instance();

	private:
		int parseHintType_RENDER_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_PLATFORM(int * hintType, int * hintValue);
};

}}
