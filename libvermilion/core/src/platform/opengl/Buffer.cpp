#ifdef VMCORE_OPENGL
#include "Buffer.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

Vermilion::Core::OpenGL::Buffer::Buffer(Vermilion::Core::OpenGL::API* api, size_t size, Vermilion::Core::BufferType type, Vermilion::Core::BufferUsage usage, Vermilion::Core::BufferDataUsage dataUsage){
	this->api = api;
	this->instance = api->instance;
	this->size = size;
	this->type = type;
	this->usage = usage;
	this->dataUsage = dataUsage;

	glGenBuffers(1, &this->buffer);
	switch(type){
		case Vermilion::Core::BufferType::BUFFER_TYPE_VERTEX:
			destination = GL_ARRAY_BUFFER;
			break;
		case Vermilion::Core::BufferType::BUFFER_TYPE_INDEX:
			destination = GL_ELEMENT_ARRAY_BUFFER;
			break;
		case Vermilion::Core::BufferType::BUFFER_TYPE_UNIFORM:
			destination = GL_UNIFORM_BUFFER;
			break;
		case Vermilion::Core::BufferType::BUFFER_TYPE_STORAGE:
			destination = GL_SHADER_STORAGE_BUFFER;
			break;
		default:
			instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Unknown buffer type");
			throw std::runtime_error("Vermilion::Core::OpenGL::Buffer::Buffer() - Unknown buffer type");
			break;
	}
	switch(dataUsage){
		case Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_STATIC:
			glusage = GL_STATIC_DRAW;
			break;
		case Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_DYNAMIC:
			glusage = GL_DYNAMIC_DRAW;
			break;
		default:
			instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Unknown buffer data usage");
			throw std::runtime_error("Vermilion::Core::OpenGL::Buffer::Buffer() - Unknown buffer data usage");
			break;
	}

	// Fill buffer with empty data
    char * dat = new char[this->size];
    for(int i=0; i<this->size; i++){
        dat[i] = 0;
    }
	glBindBuffer(destination, buffer);
	glBufferData(destination, this->size, dat, glusage);
	glBindBuffer(destination, 0);
    delete[] dat;
}

Vermilion::Core::OpenGL::Buffer::~Buffer(){
	glDeleteBuffers(1, &this->buffer);
}

void Vermilion::Core::OpenGL::Buffer::setData(void * data, size_t size){
	if(this->usage==BUFFER_USAGE_READ_WRITE || this->usage==BUFFER_USAGE_WRITE_ONLY){
		if(size==0 || size>this->size){
			size = this->size;
		}
		glBindBuffer(destination, buffer);
		glBufferSubData(destination, 0, size, data);
		glBindBuffer(destination, 0);
	}
}

void Vermilion::Core::OpenGL::Buffer::getData(void * data, size_t size){
	if(this->usage==BUFFER_USAGE_READ_ONLY || this->usage==BUFFER_USAGE_READ_WRITE){
		if(size>this->size){
			size = this->size;
		}
		glBindBuffer(destination, buffer);
		glGetBufferSubData(destination, 0, size, data);
		glBindBuffer(destination, 0);
	}
}

#endif
