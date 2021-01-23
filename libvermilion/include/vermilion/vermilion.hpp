#pragma once

/*
 * Vermilion basic information
 * ---------------------------
 */
#define VERMILION_VERSION_MAJOR 0
#define VERMILION_VERSION_MINOR 1

#define VERMILION_AUTHOR_NAME "Joppe Blondel"
#define VERMILION_AUTHOR_EMAIL "vermilion@joppeb.nl"

#define VERMILION_LICENCE "BSD-2"
/*
 * ---------------------------
 */

#include <string>
#include <cstddef>
#include <vermilion/logger.hpp>

namespace Vermilion{
namespace Core{

class Instance;

enum RenderPlatform{
	RENDER_PLATFORM_NONE = 0,
	RENDER_PLATFORM_OPENGL,
	RENDER_PLATFORM_VULKAN
};

enum ShaderType{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_GEOMETRY,
	SHADER_TYPE_FRAGMENT,
	SHADER_TYPE_COMPUTE
};

enum PipelineLayoutBinding{
	PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER,
	PIPELINE_LAYOUT_BINDING_STORAGE_BUFFER,
	PIPELINE_LAYOUT_BINDING_SAMPLER
};

enum PipelineLayoutUniformType{
	PIPELINE_LAYOUT_UNIFORM_TYPE_NONE,
	PIPELINE_LAYOUT_UNIFORM_TYPE_FLOAT1,
	PIPELINE_LAYOUT_UNIFORM_TYPE_FLOAT2,
	PIPELINE_LAYOUT_UNIFORM_TYPE_FLOAT3,
	PIPELINE_LAYOUT_UNIFORM_TYPE_FLOAT4,
	PIPELINE_LAYOUT_UNIFORM_TYPE_MAT3,
	PIPELINE_LAYOUT_UNIFORM_TYPE_MAT4,
	PIPELINE_LAYOUT_UNIFORM_TYPE_INT1,
	PIPELINE_LAYOUT_UNIFORM_TYPE_INT2,
	PIPELINE_LAYOUT_UNIFORM_TYPE_INT3,
	PIPELINE_LAYOUT_UNIFORM_TYPE_INT4,
};

struct PipelineLayoutUniform{
	std::string name;
	size_t size;
	unsigned int offset;
	PipelineLayoutUniformType type;
	PipelineLayoutUniform(std::string name, size_t size);
};

struct PipelineLayoutUniformFloat1 : public PipelineLayoutUniform{
	PipelineLayoutUniformFloat1(std::string name) :
		PipelineLayoutUniform(name, sizeof(float)){
			type = PIPELINE_LAYOUT_UNIFORM_TYPE_FLOAT1;
		}
};
struct PipelineLayoutUniformFloat2 : public PipelineLayoutUniform{
	PipelineLayoutUniformFloat2(std::string name) :
		PipelineLayoutUniform(name, 2*sizeof(float)){
			type = PIPELINE_LAYOUT_UNIFORM_TYPE_FLOAT2;
		}
};
struct PipelineLayoutUniformFloat3 : public PipelineLayoutUniform{
	PipelineLayoutUniformFloat3(std::string name) :
		PipelineLayoutUniform(name, 3*sizeof(float)){
			type = PIPELINE_LAYOUT_UNIFORM_TYPE_FLOAT3;
		}
};
struct PipelineLayoutUniformFloat4 : public PipelineLayoutUniform{
	PipelineLayoutUniformFloat4(std::string name) :
		PipelineLayoutUniform(name, 4*sizeof(float)){
			type = PIPELINE_LAYOUT_UNIFORM_TYPE_FLOAT4;
		}
};

enum PipelineSettingsDepthTest{
	PIPELINE_SETTINGS_DEPTH_TEST_DISABLED,
	PIPELINE_SETTINGS_DEPTH_TEST_ENABLED
};

enum PipelineSettingsCullMode{
	PIPELINE_SETTINGS_CULL_MODE_NONE,
	PIPELINE_SETTINGS_CULL_MODE_BACK_CC,
	PIPELINE_SETTINGS_CULL_MODE_BACK_C,
};

enum PipelineSettingsPolygonMode{
	PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE,	
    PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE_LINE,
	PIPELINE_SETTINGS_POLYGON_MODE_POINT,
	PIPELINE_SETTINGS_POLYGON_MODE_LINE
};

struct PipelineSettings{
	PipelineSettingsDepthTest depthtest = PIPELINE_SETTINGS_DEPTH_TEST_ENABLED;
	PipelineSettingsCullMode cullmode = PIPELINE_SETTINGS_CULL_MODE_NONE;
	PipelineSettingsPolygonMode poygonmode = PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE;
};

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

enum WindowMouseButton{
	WINDOW_MOUSE_BUTTON_LEFT=0,
	WINDOW_MOUSE_BUTTON_MIDDLE,
	WINDOW_MOUSE_BUTTON_RIGHT
};

enum WindowMouseAction{
	WINDOW_MOUSE_ACTION_PRESS,
	WINDOW_MOUSE_ACTION_RELEASE
};

enum WindowPlatform{
	WINDOW_PLATFORM_NONE = 0,
	WINDOW_PLATFORM_GLFW
};

struct WindowCallbackFunctions{
	void (*resizeCallback)(Vermilion::Core::Instance * instance, void * userPointer, int width, int height) = nullptr;
	void (*mouseButtonCallback)(Vermilion::Core::Instance * instance, void * userPointer, WindowMouseButton button, WindowMouseAction action) = nullptr;
	void (*mousePosCallback)(Vermilion::Core::Instance * instance, void * userPointer, double x, double y) = nullptr;
	void (*mouseEnterCallback)(Vermilion::Core::Instance * instance, void * userPointer, bool entered) = nullptr;
	void (*scrollCallback)(Vermilion::Core::Instance * instance, void * userPointer, double x, double y) = nullptr;
};

enum HintType{
	HINT_TYPE_NONE = 0,
	HINT_TYPE_WINDOW_PLATFORM,
	HINT_TYPE_RENDER_PLATFORM,
	HINT_TYPE_WINDOW_HEIGHT,
	HINT_TYPE_WINDOW_WIDTH,
	HINT_TYPE_LOGLEVEL,
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

typedef Vermilion::Core::Instance VmInstance;

class VmRenderTarget;
class VmShader;
class VmShaderProgram;
class VmPipelineLayout;
class VmPipeline;
class VmBuffer;
class VmRenderable;
class VmTexture;
class VmSampler;
class VmBinding;

class VmObject{
	friend class Vermilion::Core::Instance;
	protected:
        unsigned int ID;
        Vermilion::Core::Instance * instance;
};

class VmRenderTarget : public VmObject{
    public:
        ~VmRenderTarget();
        void start(float r=0.0, float g=0.0, float b=0.0, float a=1.0);
        void end();
        void draw(VmPipeline& pipeline, VmBinding& binding, VmRenderable& renderable, unsigned int instanceCount=1, unsigned int firstInstance=0);
		void setUniform(VmPipeline& pipeline, const std::string& name, void * data);
};

class VmShader : public VmObject{
    public:
        ~VmShader();
        Vermilion::Core::ShaderType type();
};

class VmShaderProgram : public VmObject{
    public:
        ~VmShaderProgram();
};

class VmPipelineLayout : public VmObject{
    public:
        ~VmPipelineLayout();
};

class VmPipeline : public VmObject{
	friend class VmRenderTarget;
    public:
        ~VmPipeline();
        void setViewport(int width, int height, int x, int y);
        void setScissor(int width, int height, int x, int y);
};

class VmBinding : public VmObject{
	friend class VmRenderTarget;
    public:
        ~VmBinding();
};

class VmBuffer : public VmObject{
    public:
        ~VmBuffer();
        Vermilion::Core::BufferType type();
        size_t size();
        void setData(void * data, size_t size=0);
        void getData(void * data, size_t size);
};

class VmRenderable : public VmObject{
	friend class VmRenderTarget;
    public:
        ~VmRenderable();
        unsigned int vertexOffset();
        unsigned int indexOffset();
        unsigned int length();
        void vertexOffset(unsigned int offset);
        void indexOffset(unsigned int offset);
        void length(unsigned int length);
};

class VmTexture : public VmObject{
    public:
        ~VmTexture();
        size_t width();
        size_t height();
        size_t channels();
        void setData(void * data, size_t size=0);
};

class VmSampler : public VmObject{
    public:
        ~VmSampler();
};

namespace Vermilion{
namespace Core{

unsigned char * loadTextureData(const std::string& path, size_t * width, size_t * height, size_t * channels);
void freeTextureData(unsigned char * data);
void flipLoading();

}}

#ifndef __VERMILION_INTERNAL__
#include <vermilion/instance.hpp>
#endif