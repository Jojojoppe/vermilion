#pragma once

#include <vermilion/api.hpp>
#include <vermilion/window.hpp>
#include <vermilion/logger.hpp>

#include <memory>

namespace Vermilion{
namespace Core{

/**
 * @brief Hint types
 */
enum HintType{
	HINT_TYPE_NONE = 0,
	HINT_TYPE_WINDOW_PLATFORM,
	HINT_TYPE_RENDER_PLATFORM,
	HINT_TYPE_WINDOW_HEIGHT,
	HINT_TYPE_WINDOW_WIDTH,
	HINT_TYPE_LOGLEVEL,
};

/**
 * @brief The instance class it the most top level class of Vermilion
 */
class Instance{
	public:
		Logger logger;
		std::unique_ptr<API> api;
		std::unique_ptr<Window> window;

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

		void startRender();
		void endRender();

	private:
		int parseHintType_RENDER_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_PLATFORM(int * hintType, int * hintValue);
		int parseHintType_WINDOW_HEIGHT(int * hintType, int * hintValue);
		int parseHintType_WINDOW_WIDTH(int * hintType, int * hintValue);
		int parseHintType_LOGLEVEL(int * hintType, int * hintValue);
};

}}
