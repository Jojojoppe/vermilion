#pragma once

#include <stdarg.h>

namespace Vermilion{
namespace Core{

#define VMCORE_LOGLEVEL_TRACE 0
#define VMCORE_LOGLEVEL_DEBUG 1
#define VMCORE_LOGLEVEL_INFO 2
#define VMCORE_LOGLEVEL_WARNING 3
#define VMCORE_LOGLEVEL_ERROR 4
#define VMCORE_LOGLEVEL_FATAL 5

class Logger{
	private:
		int loglevel = 0;
	public:
		Logger();
		~Logger();

		int getLevel();
		void setLevel(int level);

		void log(int level, const char * format, ...);
};

}}
