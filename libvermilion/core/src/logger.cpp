#include <vermilion/core/logger.hpp>
#include <cstdio>

const char * VmCoreLoggerLevelNames[] = {
	"DEBUG", 
	"INFO", 
	"WARN", 
	"ERROR", 
	"FATAL"
};
const char * VmCoreLoggerLevelColors[] = {
	"\033[00;37m",
	"\033[00;33m",
	"\033[01;33m",
	"\033[00;31m",
	"\033[01;31m"
};

Vermilion::Core::Logger::Logger(){
//	this->log(VMCORE_LOGLEVEL_DEBUG, "Vermilion logger initialized");
}

Vermilion::Core::Logger::~Logger(){
//	this->log(VMCORE_LOGLEVEL_DEBUG, "Vermilion logger deinitialized");
}

int Vermilion::Core::Logger::getLevel(){
	return this->loglevel;
}

void Vermilion::Core::Logger::setLevel(int level){
	this->loglevel = level;
}

void Vermilion::Core::Logger::log(int level, const char * format, ...){
	va_list args;
	if(level>=this->loglevel){
		fprintf(stderr, "%s%s]\t", VmCoreLoggerLevelColors[level], VmCoreLoggerLevelNames[level]);
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
		fprintf(stderr, "\033[00m\r\n");
	}
}
