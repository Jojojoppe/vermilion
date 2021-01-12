#pragma once

#include <vermilion/logger.hpp>

#include <memory>
#include <vector>
#include <string>

namespace Vermilion{
namespace Core{

enum VertexBufferLayoutElementType{
	VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_NONE = 0,
	VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT,
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
			type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT;
		}
};
struct VertexBufferLayoutElementFloat2 : public VertexBufferLayoutElement{
	VertexBufferLayoutElementFloat2(std::string name, bool normalized=false) :
		VertexBufferLayoutElement(name, 2, sizeof(float), normalized){
			type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT;
		}
};
struct VertexBufferLayoutElementFloat3 : public VertexBufferLayoutElement{
	VertexBufferLayoutElementFloat3(std::string name, bool normalized=false) :
		VertexBufferLayoutElement(name, 3, sizeof(float), normalized){
			type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT;
		}
};
struct VertexBufferLayoutElementFloat4 : public VertexBufferLayoutElement{
	VertexBufferLayoutElementFloat4(std::string name, bool normalized=false) :
		VertexBufferLayoutElement(name, 4, sizeof(float), normalized){
			type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT;
		}
};

struct VertexBufferLayoutElementMat3 : public VertexBufferLayoutElement{
	VertexBufferLayoutElementMat3(std::string name, bool normalized=false) :
		VertexBufferLayoutElement(name, 3*3, sizeof(float), normalized){
			type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT;
		}
};
struct VertexBufferLayoutElementMat4 : public VertexBufferLayoutElement{
	VertexBufferLayoutElementMat4(std::string name, bool normalized=false) :
		VertexBufferLayoutElement(name, 4*4, sizeof(float), normalized){
			type = VERTEX_BUFFER_LAYOUT_ELEMENT_TYPE_FLOAT;
		}
};

class Buffer{
	public:
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