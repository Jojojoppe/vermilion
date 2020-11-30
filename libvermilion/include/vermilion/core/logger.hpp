#pragma once

#include <stdarg.h>

namespace Vermilion{
namespace Core{

#define VMCORE_LOGLEVEL_DEBUG 0
#define VMCORE_LOGLEVEL_INFO 1
#define VMCORE_LOGLEVEL_WARNING 2
#define VMCORE_LOGLEVEL_ERROR 3
#define VMCORE_LOGLEVEL_FATAL 4

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
