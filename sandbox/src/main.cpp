#include <vermilion/vermilion.hpp>
#include <vermilion/glm/glm.hpp>
#include <vermilion/glm/gtc/matrix_transform.hpp>
#include <memory>

struct UniformBufferObject{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

int main(int argc, char ** argv){
	try{

		// Create Vermilion instance
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
		VmInstance vmInstance(hintType, hintValue);

		// Create render targets
		VmRenderTarget defaultRenderTarget = vmInstance.getDefaultRenderTarget();
		VmTexture renderTargetTexture = vmInstance.createTexture("", 512, 512, 4);
		VmRenderTarget renderTarget = vmInstance.createRenderTarget(renderTargetTexture);
		VmSampler renderTargetSampler = vmInstance.createSampler(renderTargetTexture);

		// Create shaders
		VmShader vertexShader = vmInstance.createShader(
			R"(
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
		VmShader fragmentShader = vmInstance.createShader(
			R"(
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
		VmShaderProgram shaderProgram = vmInstance.createShaderProgram({vertexShader, fragmentShader});

		// Create render pipeline
		VmPipeline pipeline = vmInstance.createPipeline(defaultRenderTarget, shaderProgram, {
			Vermilion::Core::BufferLayoutElementFloat4("aPos"),
			Vermilion::Core::BufferLayoutElementFloat3("aColor"),
			Vermilion::Core::BufferLayoutElementFloat2("aTexCoord")
		},{
			Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER,
			Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_SAMPLER
		});

		// Render object
		std::vector<float> vertices({
			-0.5,	-0.5,	0.0,	1.0,		1.0,	0.0,	0.0,		1.0, 	0.0,
			0.5	,	-0.5,	0.0,	1.0,		0.0,	1.0,	0.0,		0.0,	0.0,
			0.5,	0.5,	0.0,	1.0,		0.0,	0.0,	1.0,		0.0,	1.0,
			-0.5,	0.5,	0.0,	1.0,		1.0,	1.0,	1.0,		1.0,	1.0,

			-0.5,	-0.5,	-0.5,	1.0,		1.0,	0.0,	0.0,		1.0, 	0.0,
			0.5	,	-0.5,	-0.5,	1.0,		0.0,	1.0,	0.0,		0.0,	0.0,
			0.5,	0.5,	-0.5,	1.0,		0.0,	0.0,	1.0,		0.0,	1.0,
			-0.5,	0.5,	-0.5,	1.0,		1.0,	1.0,	1.0,		1.0,	1.0,
		});
		std::vector<unsigned int> indices({
			0, 1, 2,
			2, 3, 0,
			4, 5, 6,
			6, 7, 4,
		});
		// Create vertex and index buffers
		VmVertexBuffer vertexBuffer = vmInstance.createVertexBuffer(vertices);
		VmIndexBuffer indexBuffer = vmInstance.createIndexBuffer(indices);
		VmRenderable renderObject = vmInstance.createRenderable(vertexBuffer, indexBuffer);

		// Create texture
		VmTexture texture = vmInstance.createTexture("/home/joppe/Pictures/texture.jpg");
		VmSampler sampler = vmInstance.createSampler(texture);

		// Create uniform buffer
		VmUniformBuffer uniformBuffer1 = vmInstance.createUniformBuffer(sizeof(UniformBufferObject));
		UniformBufferObject uboData1;
		uboData1.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		uboData1.proj = glm::perspective(glm::radians(45.0f), vmInstance.window->width / (float) vmInstance.window->height, 0.1f, 10.0f); // TODO get window data

		// Create binding (Must be in the format as the pipeline layoutBinding)
		VmBinding binding1 = vmInstance.createBinding({uniformBuffer1}, {sampler});
		VmBinding binding2 = vmInstance.createBinding({uniformBuffer1}, {renderTargetSampler});

		float time = 0.0f;

		while(vmInstance.window->shouldClose()){
			time += 0.01f;

			vmInstance.startRender();

				uboData1.model = glm::rotate(glm::mat4(1.0f), time*glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				vmInstance.streamData(uniformBuffer1, &uboData1);		// TODO move function to uniformBuffer->streamData(&uboData); ....

				renderTarget->start();
					renderTarget->draw(pipeline, binding1, renderObject);
				renderTarget->end();

				defaultRenderTarget->start();
					defaultRenderTarget->draw(pipeline, binding2, renderObject);
				defaultRenderTarget->end();

			vmInstance.endRender({renderTarget});
		}

	}catch(std::exception& ex){
		return 1;
	}
	return 0;
}