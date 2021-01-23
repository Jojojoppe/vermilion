#include <vermilion/vermilion.hpp>
#include <vermilion/utils.hpp>

#include <vermilion/glm/glm.hpp>
#include <vermilion/glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#include "gui.hpp"

#include <fstream>
#include <cmath>

struct UniformBufferObject{
	glm::mat4 view;
	glm::mat4 proj;
};

struct Application{

	std::shared_ptr<VmInstance> vmInstance;

	VmTexture texture1, texture2;
	VmSampler sampler1, sampler2;

	VmRenderTarget defaultRenderTarget, textureRenderTarget;

	VmShader vertexShader, fragmentShader;
	VmShaderProgram shaderProgram;

	VmBuffer vertexBuffer, indexBuffer, uniformBuffer1, uniformBuffer2;
	VmRenderable object;

	UniformBufferObject ubo1, ubo2;

	VmPipelineLayout pipelineLayout;
	VmPipeline pipeline1, pipeline2;
	VmBinding binding1, binding2;

	std::unique_ptr<GUI> gui;

	static void resize(VmInstance * instance, void * userPointer, int width, int height){
		Application * app = (Application*) userPointer;
		app->pipeline1.setViewport(width, height, 0, 0);
		app->pipeline1.setScissor(width, height, 0, 0);
		app->ubo1.proj = glm::perspective(glm::radians(45.0f), width/(float)height, 0.1f, 10.0f);
		app->uniformBuffer1.setData(&app->ubo1);
		// app->gui->resize(width, height);
	}

	static void mouseButton(VmInstance * instance, void * userPointer, Vermilion::Core::WindowMouseButton btn, Vermilion::Core::WindowMouseAction action){
		Application * app = (Application*) userPointer;
		app->gui->mouseButton(btn, action);
	}

	static void mousePos(VmInstance * instance, void * userPointer, double x, double y){
		Application * app = (Application*) userPointer;
		app->gui->mousePos(x, y);
	}

	static void mouseEnter(VmInstance * instance, void * userPointer, bool enter){
		if(enter){
			instance->logger.log(VMCORE_LOGLEVEL_INFO, "Mouse entered");
		}else{
			instance->logger.log(VMCORE_LOGLEVEL_INFO, "Mouse left");
		}
	}

	static void scroll(VmInstance * instance, void * userPointer, double x, double y){
		Application * app = (Application*) userPointer;
		app->gui->scroll(x, y);
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
			Vermilion::Core::RenderPlatform::RENDER_PLATFORM_OPENGL,
			400,
			400,
			VMCORE_LOGLEVEL_DEBUG,
		0};
		vmInstance.reset(new VmInstance(hintType, hintValue, this, Vermilion::Core::WindowCallbackFunctions{resize, mouseButton, mousePos, mouseEnter, scroll}));

		gui.reset(new GUI(vmInstance, 400, 400));

		size_t width, height, channels;
		unsigned char * texture1_pixels = Vermilion::Core::loadTextureData("../assets/viking_room.png", &width, &height, &channels);
		vmInstance->createTexture(texture1, width, height, 4);
		texture1.setData(texture1_pixels);
		Vermilion::Core::freeTextureData(texture1_pixels);
		vmInstance->createSampler(sampler1, texture1);

		vmInstance->getDefaultRenderTarget(defaultRenderTarget);

		vmInstance->createShader(vertexShader, R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable
			#extension GL_KHR_vulkan_glsl : enable

			layout(location = 0) in vec4 aPos;
			layout(location = 1) in vec3 aTexCoord;
			layout(location = 2) in vec3 aNorm;
			layout(location = 0) out vec2 fTexCoord;
			layout(binding=0) uniform UniformBufferObject{
				mat4 view;
				mat4 proj;
			} ubo;

			#ifdef VULKAN
				layout(push_constant) uniform PC0{
					mat4 uModel;
				};
			#else
				layout(push_constant) uniform mat4 uModel;
			#endif
			
			void main() {
				gl_Position = ubo.proj * ubo.view * uModel * aPos;
				fTexCoord = aTexCoord.xy;
			}
		)", Vermilion::Core::ShaderType::SHADER_TYPE_VERTEX);
		vmInstance->createShader(fragmentShader, R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable
			#extension GL_KHR_vulkan_glsl : enable

			layout(location = 0) in vec2 fTexCoord;
			layout(location = 0) out vec4 outColor;
			layout(binding = 1) uniform sampler2D s_tex;

			#ifdef VULKAN
				layout(push_constant) uniform PC0{
					mat4 uModel;
				};
			#else
				layout(push_constant) uniform mat4 uModel;
			#endif

			void main() {
				outColor = vec4(texture(s_tex, fTexCoord).xyz, 1.0);
			}
		)", Vermilion::Core::ShaderType::SHADER_TYPE_FRAGMENT);
		vmInstance->createShaderProgram(shaderProgram, {&vertexShader, &fragmentShader});

		vmInstance->createPipelineLayout(pipelineLayout, {
				Vermilion::Core::BufferLayoutElementFloat4("aPos"),
				Vermilion::Core::BufferLayoutElementFloat3("aTexCoord"),
				Vermilion::Core::BufferLayoutElementFloat3("aNorm")
			},{
				Vermilion::Core::PipelineLayoutBinding(Vermilion::Core::PipelineLayoutBindingType::PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER, 0, 0),
				Vermilion::Core::PipelineLayoutBinding(Vermilion::Core::PipelineLayoutBindingType::PIPELINE_LAYOUT_BINDING_SAMPLER)
			},{
				Vermilion::Core::PipelineLayoutUniformMat4("uModel"),
		});

		vmInstance->createPipeline(pipeline1, defaultRenderTarget, shaderProgram, pipelineLayout, Vermilion::Core::PipelineSettings{
			Vermilion::Core::PipelineSettingsDepthTest::PIPELINE_SETTINGS_DEPTH_TEST_ENABLED,
			Vermilion::Core::PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_NONE,
			Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE
		});

		std::vector<float> vertices;
		std::vector<unsigned int> indices;

		Vermilion::Utils::loadObj(vertices, indices, "../assets/viking_room.obj");

		vmInstance->createBuffer(vertexBuffer, sizeof(float)*vertices.size(), 
			Vermilion::Core::BufferType::BUFFER_TYPE_VERTEX,
			Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
			Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_STATIC
		);
		vmInstance->createBuffer(indexBuffer, sizeof(unsigned int)*indices.size(),
			Vermilion::Core::BufferType::BUFFER_TYPE_INDEX,
			Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
			Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_STATIC
		);
		vertexBuffer.setData(vertices.data());
		indexBuffer.setData(indices.data());
		vmInstance->createRenderable(object, vertexBuffer, indexBuffer, 0, 0, indices.size());

		vmInstance->createBuffer(uniformBuffer1, sizeof(UniformBufferObject),
			Vermilion::Core::BufferType::BUFFER_TYPE_UNIFORM,
			Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
			Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_DYNAMIC
		);
		
		ubo1.view = glm::lookAt(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 0, 1));
		ubo1.proj = glm::perspective(glm::radians(45.0f), (float)400/400, 0.1f, 10.0f);
		uniformBuffer1.setData(&ubo1);
		
		vmInstance->createBinding(binding1, {&uniformBuffer1}, {&sampler1});

	}
	void run(){

		float uColMul;
		glm::vec3 uPos = glm::vec3(0.0f);
		float time = 0.0f;

		glm::mat4 model1 = glm::mat4(1.0f);

		while(vmInstance->shouldClose()){
			vmInstance->startRender();

			model1 = glm::rotate(model1, 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));

			defaultRenderTarget.start(1.0, 1.0, 1.0, 1.0);
			defaultRenderTarget.setUniform(pipeline1, "uModel", &model1);
			defaultRenderTarget.draw(pipeline1, binding1, object, 1, 0);
			// gui->render();
			defaultRenderTarget.end();

			vmInstance->endRender({});
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