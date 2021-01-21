#pragma once

#include <vermilion/vermilion.hpp>
#include <vermilion/logger.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Vermilion{
namespace Core{

class Buffer{
	public:
		BufferType type;
		BufferUsage usage;
		BufferDataUsage dataUsage;
		size_t size;

		~Buffer() = default;
		virtual void setData(void * data, size_t size=0){};
		virtual void getData(void * data, size_t size){};
};

}}