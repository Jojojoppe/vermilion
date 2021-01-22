#ifdef VMCORE_OPENGL
#pragma once

#include <stdint.h>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>

#include <glad/glad.h>
#include "../../Buffer.hpp"

namespace Vermilion{
namespace Core{

class Instance;

namespace OpenGL{

class API;

class Buffer : public Vermilion::Core::Buffer{
	private:
		Vermilion::Core::Instance * instance;
		API * api;

	public:
		unsigned int buffer;
		unsigned int destination;
		unsigned int glusage;

		Buffer(API * api, size_t, Vermilion::Core::BufferType type, Vermilion::Core::BufferUsage usage, Vermilion::Core::BufferDataUsage dataUsage);
		~Buffer();

		virtual void setData(void * data, size_t size) override;
		virtual void getData(void * data, size_t size) override;
};

}}}
#endif
