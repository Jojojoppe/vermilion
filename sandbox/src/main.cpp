#include <vermilion/vermilion.hpp>

#include <vermilion/glm/glm.hpp>
#include <vermilion/glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

struct UniformBufferObject{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

struct Application{

	std::shared_ptr<VmInstance> vmInstance;

	VmTexture texture1;
	VmSampler sampler1;

	VmShader vertexShader, fragmentShader;
	VmShaderProgram shaderProgram;

	float time;

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
		vmInstance.reset(new VmInstance(hintType, hintValue, this, Vermilion::Core::WindowCallbackFunctions{}));

		size_t width, height, channels;
		unsigned char * texture1_pixels = Vermilion::Core::loadTextureData("../assets/texture1.jpg", &width, &height, &channels);
		vmInstance->createTexture(texture1, width, height, 4);
		texture1.setData(texture1_pixels);
		Vermilion::Core::freeTextureData(texture1_pixels);
		vmInstance->createSampler(sampler1, texture1);

		vmInstance->createShader(vertexShader, R"(
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
		vmInstance->createShader(fragmentShader, R"(
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
		vmInstance->createShaderProgram(shaderProgram, {&vertexShader, &fragmentShader});

		time = 0.0f;
	}

	void run(){
		while(vmInstance->shouldClose()){
			vmInstance->startRender();

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