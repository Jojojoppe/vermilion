#pragma once

#include <vermilion/logger.hpp>

#include <memory>
#include <vector>

namespace Vermilion{
namespace Core{

enum PipelineLayoutBinding{
	PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER = 1,
	PIPELINE_LAYOUT_BINDING_STORAGE_BUFFER,
	PIPELINE_LAYOUT_BINDING_SAMPLER
};

enum PipelineSettingsDepthTest{
	PIPELINE_SETTINGS_DEPTH_TEST_DISABLED = 0,
	PIPELINE_SETTINGS_DEPTH_TEST_ENABLED
};

enum PipelineSettingsCullMode{
	PIPELINE_SETTINGS_CULL_MODE_NONE = 0,
	PIPELINE_SETTINGS_CULL_MODE_BACK_CC,
	PIPELINE_SETTINGS_CULL_MODE_BACK_C,
};

enum PipelineSettingsPolygonMode{
	PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE = 0,
	PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE_LINE,
	PIPELINE_SETTINGS_POLYGON_MODE_POINT,
	PIPELINE_SETTINGS_POLYGON_MODE_LINE
};

struct PipelineSettings{
	PipelineSettingsDepthTest depthtest = PIPELINE_SETTINGS_DEPTH_TEST_ENABLED;
	PipelineSettingsCullMode cullmode = PIPELINE_SETTINGS_CULL_MODE_NONE;
	PipelineSettingsPolygonMode poygonmode = PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE;
};

class PipelineLayout{
	public:
		~PipelineLayout() = default;
};

class Pipeline{
	public:
		~Pipeline() = default;
		virtual void setViewPort(unsigned int width, unsigned int height, unsigned int x=0, unsigned int y=0){};
		virtual void setScissor(unsigned int width, unsigned int height, unsigned int x=0, unsigned int y=0){};
};

class Binding{
	public:
		~Binding() = default;
};

}}