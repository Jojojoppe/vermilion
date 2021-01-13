#pragma once

#include <vermilion/logger.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Vermilion{
namespace Core{

enum VertexBufferLayoutElementType{
	VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_NONE = 0,
	VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT1,
	VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT2,
	VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT3,
	VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT4,
	VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_INT32,
	VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_BYTE,
};

struct VertexBufferLayoutElement{
	std::string name;
	unsigned int count;
	unsigned int size;
	bool normalized;
	unsigned int offset;
	VertexBufferLayoutElementType type;
	VertexBufferLayoutElement(std::string name, unsigned int count, unsigned int size, bool normalized);
};

struct VertexBufferLayoutElementFloat1 : public VertexBufferLayoutElement{
	VertexBufferLayoutElementFloat1(std::string name, bool normalized=false) :
		VertexBufferLayoutElement(name, 1, sizeof(float), normalized){
			type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT1;
		}
};
struct VertexBufferLayoutElementFloat2 : public VertexBufferLayoutElement{
	VertexBufferLayoutElementFloat2(std::string name, bool normalized=false) :
		VertexBufferLayoutElement(name, 2, sizeof(float), normalized){
			type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT2;
		}
};
struct VertexBufferLayoutElementFloat3 : public VertexBufferLayoutElement{
	VertexBufferLayoutElementFloat3(std::string name, bool normalized=false) :
		VertexBufferLayoutElement(name, 3, sizeof(float), normalized){
			type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT3;
		}
};
struct VertexBufferLayoutElementFloat4 : public VertexBufferLayoutElement{
	VertexBufferLayoutElementFloat4(std::string name, bool normalized=false) :
		VertexBufferLayoutElement(name, 4, sizeof(float), normalized){
			type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT4;
		}
};

class Buffer{
	public:
		size_t size;
		unsigned int count;
		size_t element_size;
		~Buffer() = default;
};

class VertexBuffer : public Buffer{
	public:
		~VertexBuffer() = default;
};

class IndexBuffer : public Buffer{
	public:
		~IndexBuffer() = default;
};

}}