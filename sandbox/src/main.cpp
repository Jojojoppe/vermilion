#include <vermilion/vermilion.hpp>
#include <vermilion/glm/glm.hpp>
#include <vermilion/glm/gtc/matrix_transform.hpp>
#include <memory>

#include "gui.hpp"

struct UniformBufferObject{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

struct Application{

	std::shared_ptr<VmInstance> vmInstance;

	VmRenderTarget defaultRenderTarget;
	VmRenderTarget textureRenderTarget;

	VmTexture texture1;
	VmTexture texture2;
	VmSampler sampler1;
	VmSampler sampler2;

	VmShader vertexShader;
	VmShader fragmentShader;
	VmShaderProgram shaderProgram;

	VmPipeline pipeline1;
	VmPipeline pipeline2;
	VmBinding binding1;
	VmBinding binding2;

	VmBuffer vertexBuffer;
	VmBuffer indexBuffer;
	VmRenderable object;

	VmBuffer uniformBuffer1;
	VmBuffer uniformBuffer2;

	UniformBufferObject ubo1;
	UniformBufferObject ubo2;

	std::unique_ptr<GUI> gui;

	float time;

	static void resize(VmInstance * instance, void * userPointer){
		Application * app = (Application*) userPointer;
		int width, height;
		instance->window->getFrameBufferSize(&width, &height);
		app->pipeline1->setViewPort(width, height, 0, 0);
		app->pipeline1->setScissor(width, height, 0, 0);
		app->ubo1.proj = glm::perspective(glm::radians(45.0f), width/(float)height, 0.1f, 10.0f);
		app->gui->resize(width, height);
	}

	Application(){
		int hintType[] = {
			Vermilion::Core::HintType::HINT_TYPE_WINDOW_PLATFORM, 
			Vermilion::Core::HintType::HINT_TYPE_RENDER_PLATFORM, 
			Vermilion::Core::HintType::HINT_TYPE_WINDOW_WIDTH,
			Vermilion::Core::HintType::HINT_TYPE_WINDOW_HEIGHT,
			Vermilion::Core::HintType::HINT_TYPE_LOGLEVEL,
		0};
		int hintValue[] = {
			Vermilion::Core::WindowPlatform::WINDOW_PLATFORM_GLFW, 
			Vermilion::Core::RenderPlatform::RENDER_PLATFORM_VULKAN,
			400,
			400,
			VMCORE_LOGLEVEL_DEBUG,
		0};
		vmInstance.reset(new VmInstance(hintType, hintValue));
		vmInstance->window->setUserPointer(this);
		vmInstance->window->setResizedCallback(Application::resize);

		gui.reset(new GUI(vmInstance, vmInstance->window->width, vmInstance->window->height));

		size_t width, height, channels;
		unsigned char * texture1_pixels = Vermilion::Core::loadTextureData("../assets/texture1.jpg", &width, &height, &channels);
		texture1 = vmInstance->createTexture(width, height, 4);
		texture1->setData(texture1_pixels);
		Vermilion::Core::freeTextureData(texture1_pixels);
		sampler1 = vmInstance->createSampler(texture1);

		texture2 = vmInstance->createTexture(512, 512, 4);
		sampler2 = vmInstance->createSampler(texture2);

		defaultRenderTarget = vmInstance->getDefaultRenderTarget();
		textureRenderTarget = vmInstance->createRenderTarget(texture2);

		vertexShader = vmInstance->createShader(R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable
			layout(location = 0) in vec4 aPos;
			layout(location = 1) in vec3 aColor;
			layout(location = 2) in vec2 aTexCoord;
			
			layout(location = 0) out vec3 fColor;
			layout(location = 1) out vec2 fTexCoord;

			layout(binding=0) uniform UniformBufferObject{
				mat4 model;
				mat4 view;
				mat4 proj;
			} ubo;
			
			void main() {
				gl_Position = ubo.proj * ubo.view * ubo.model * aPos;
				fColor = aColor;
				fTexCoord = aTexCoord;
			}
		)", Vermilion::Core::ShaderType::SHADER_TYPE_VERTEX);
		fragmentShader = vmInstance->createShader(R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable

			layout(location = 0) in vec3 fColor;
			layout(location = 1) in vec2 fTexCoord;

			layout(location = 0) out vec4 outColor;

			layout(binding = 1) uniform sampler2D s_tex;

			void main() {
				outColor = texture(s_tex, fTexCoord);
			}
		)", Vermilion::Core::ShaderType::SHADER_TYPE_FRAGMENT);
		shaderProgram = vmInstance->createShaderProgram({vertexShader, fragmentShader});

		pipeline1 = vmInstance->createPipeline(defaultRenderTarget, shaderProgram, {
				Vermilion::Core::PipelineSettingsDepthTest::PIPELINE_SETTINGS_DEPTH_TEST_ENABLED,
				Vermilion::Core::PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_BACK_CC,
				Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE
			},{
				Vermilion::Core::BufferLayoutElementFloat4("aPos"),
				Vermilion::Core::BufferLayoutElementFloat3("aColor"),
				Vermilion::Core::BufferLayoutElementFloat2("aTexCoord")
			}, {
				Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER,
				Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_SAMPLER
		});

		pipeline2 = vmInstance->createPipeline(textureRenderTarget, shaderProgram, {
				Vermilion::Core::PipelineSettingsDepthTest::PIPELINE_SETTINGS_DEPTH_TEST_ENABLED,
				Vermilion::Core::PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_BACK_CC,
				Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE
			},{
				Vermilion::Core::BufferLayoutElementFloat4("aPos"),
				Vermilion::Core::BufferLayoutElementFloat3("aColor"),
				Vermilion::Core::BufferLayoutElementFloat2("aTexCoord")
			}, {
				Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER,
				Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_SAMPLER
		});
		pipeline2->setViewPort(texture2->width, texture2->height, 0, 0);
		pipeline2->setScissor(texture2->width, texture2->height, 0, 0);

		std::vector<float> vertices({
			-1.0,	-1.0,	0.0,	1.0,		1.0,	0.0,	0.0,		1.0, 	0.0,
			1.0	,	-1.0,	0.0,	1.0,		0.0,	1.0,	0.0,		0.0,	0.0,
			1.0,	1.0,	0.0,	1.0,		0.0,	0.0,	1.0,		0.0,	1.0,
			-1.0,	1.0,	0.0,	1.0,		1.0,	1.0,	1.0,		1.0,	1.0,
		});
		std::vector<unsigned int> indices({
			0, 1, 2,
			2, 3, 0,
		});
		vertexBuffer = vmInstance->createBuffer(sizeof(float)*vertices.size(), 
			Vermilion::Core::BufferType::BUFFER_TYPE_VERTEX,
			Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
			Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_STATIC
		);
		indexBuffer = vmInstance->createBuffer(sizeof(unsigned int)*indices.size(),
			Vermilion::Core::BufferType::BUFFER_TYPE_INDEX,
			Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
			Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_STATIC
		);
		vertexBuffer->setData(vertices.data());
		indexBuffer->setData(indices.data());
		object = vmInstance->createRenderable(vertexBuffer, indexBuffer, 0, 0, indices.size());

		uniformBuffer1 = vmInstance->createBuffer(sizeof(UniformBufferObject),
			Vermilion::Core::BufferType::BUFFER_TYPE_UNIFORM,
			Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
			Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_DYNAMIC
		);
		uniformBuffer2 = vmInstance->createBuffer(sizeof(UniformBufferObject),
			Vermilion::Core::BufferType::BUFFER_TYPE_UNIFORM,
			Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
			Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_DYNAMIC
		);

		ubo1.model = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo1.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo1.proj = glm::perspective(glm::radians(45.0f), vmInstance->window->width/(float)vmInstance->window->height, 0.1f, 10.0f);

		ubo2.model = glm::mat4(0.5f);
		ubo2.view = glm::mat4(1.0f);
		ubo2.proj = glm::mat4(1.0f);

		binding1 = vmInstance->createBinding({uniformBuffer1}, {sampler2});
		binding2 = vmInstance->createBinding({uniformBuffer2}, {sampler1});

		time = 0.0f;
	}

	void run(){
		while(vmInstance->window->shouldClose()){
			vmInstance->startRender();

			ubo1.model = glm::rotate(glm::mat4(1.0f), time*glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			uniformBuffer1->setData(&ubo1);
			ubo2.model = glm::rotate(glm::mat4(1.0f), -1*time*glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			uniformBuffer2->setData(&ubo2);

			textureRenderTarget->start(1.0, 0.0, 0.0, 1.0);
			textureRenderTarget->draw(pipeline2, binding2, object);
			textureRenderTarget->end();

			defaultRenderTarget->start(0.05, 0.05, 0.05, 1.0);
			defaultRenderTarget->draw(pipeline1, binding1, object);
			gui->render();
			defaultRenderTarget->end();

			vmInstance->endRender({textureRenderTarget});
			time += 0.01f;
		}
	}
};

int main(int argc, char ** argv){
	try{
		Application A;
		A.run();
	}catch(std::exception& ex){
		return 1;
	}
	return 0;
}