#pragma once

#include <stdarg.h>

namespace Vermilion{
namespace Core{

#define VMCORE_LOGLEVEL_DEBUG 0
#define VMCORE_LOGLEVEL_INFO 1
#define VMCORE_LOGLEVEL_WARNING 2
#define VMCORE_LOGLEVEL_ERROR 3
#define VMCORE_LOGLEVEL_FATAL 4

#define VMCORE_LOGDEBUG(...) log(0, __VA_ARGS__)
#define VMCORE_LOGINFO(...) log(1, __VA_ARGS__)
#define VMCORE_LOGWARN(...) log(2, __VA_ARGS__)
#define VMCORE_LOGERROR(...) log(3, __VA_ARGS__)
#define VMCORE_LOGFATAL(...) log(4, __VA_ARGS__)

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
