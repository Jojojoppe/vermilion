#pragma once

#include <vermilion/logger.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Vermilion{
namespace Core{

enum BufferType{
	BUFFER_TYPE_VERTEX,
	BUFFER_TYPE_INDEX,
	BUFFER_TYPE_UNIFORM,
	BUFFER_TYPE_STORAGE
};

enum BufferUsage{
	BUFFER_USAGE_READ_WRITE,
	BUFFER_USAGE_WRITE_ONLY,
	BUFFER_USAGE_READ_ONLY
};

enum BufferDataUsage{
	BUFFER_DATA_USAGE_STATIC,
	BUFFER_DATA_USAGE_DYNAMIC
};

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

// --------------------------------------------------

enum BufferLayoutElementType{
	BUFFER_LAYOUT_ELEMENT_TYPE_NONE = 0,
	BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT1,
	BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT2,
	BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT3,
	BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT4,
	BUFFER_LAYOUT_ELEMENT_TYPE_INT32,
	BUFFER_LAYOUT_ELEMENT_TYPE_BYTE1,
	BUFFER_LAYOUT_ELEMENT_TYPE_BYTE2,
	BUFFER_LAYOUT_ELEMENT_TYPE_BYTE3,
	BUFFER_LAYOUT_ELEMENT_TYPE_BYTE4,
};

struct BufferLayoutElement{
	std::string name;
	unsigned int count;
	unsigned int size;
	bool normalized;
	unsigned int offset;
	BufferLayoutElementType type;
	BufferLayoutElement(std::string name, unsigned int count, unsigned int size, bool normalized);
};

struct BufferLayoutElementFloat1 : public BufferLayoutElement{
	BufferLayoutElementFloat1(std::string name, bool normalized=false) :
		BufferLayoutElement(name, 1, sizeof(float), normalized){
			type = BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT1;
		}
};
struct BufferLayoutElementFloat2 : public BufferLayoutElement{
	BufferLayoutElementFloat2(std::string name, bool normalized=false) :
		BufferLayoutElement(name, 2, sizeof(float), normalized){
			type = BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT2;
		}
};
struct BufferLayoutElementFloat3 : public BufferLayoutElement{
	BufferLayoutElementFloat3(std::string name, bool normalized=false) :
		BufferLayoutElement(name, 3, sizeof(float), normalized){
			type = BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT3;
		}
};
struct BufferLayoutElementFloat4 : public BufferLayoutElement{
	BufferLayoutElementFloat4(std::string name, bool normalized=false) :
		BufferLayoutElement(name, 4, sizeof(float), normalized){
			type = BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT4;
		}
};
struct BufferLayoutElementByte4 : public BufferLayoutElement{
	BufferLayoutElementByte4(std::string name, bool normalized=false) :
		BufferLayoutElement(name, 4, sizeof(char), normalized){
			type = BUFFER_LAYOUT_ELEMENT_TYPE_BYTE4;
		}
};

}}