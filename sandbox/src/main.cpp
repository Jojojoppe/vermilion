#include <vermilion/vermilion.hpp>

#include <vermilion/instance.hpp>

#include <memory>

int main(int argc, char ** argv){

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
	Vermilion::Core::Instance vmInstance(hintType, hintValue);

	std::shared_ptr<Vermilion::Core::RenderTarget> defaultRenderTarget = vmInstance.getDefaultRenderTarget();

	// Create shaders
	std::shared_ptr<Vermilion::Core::Shader> vertexShader = vmInstance.createShader(
		R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable

			layout(location = 0) out vec3 fragColor;

			vec2 positions[3] = vec2[](
				vec2(0.0, -0.5),
				vec2(0.5, 0.5),
				vec2(-0.5, 0.5)
			);

			vec3 colors[3] = vec3[](
				vec3(1.0, 0.0, 0.0),
				vec3(0.0, 1.0, 0.0),
				vec3(0.0, 0.0, 1.0)
			);

			void main() {
				gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
				fragColor = colors[gl_VertexIndex];
			}
		)", Vermilion::Core::ShaderType::SHADER_TYPE_VERTEX);
	std::shared_ptr<Vermilion::Core::Shader> fragmentShader = vmInstance.createShader(
		R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable

			layout(location = 0) in vec3 fragColor;

			layout(location = 0) out vec4 outColor;

			void main() {
				outColor = vec4(fragColor, 1.0);
			}
		)", Vermilion::Core::ShaderType::SHADER_TYPE_FRAGMENT);
	std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram = vmInstance.createShaderProgram({vertexShader, fragmentShader});

	// Render object
	std::vector<float> vertices({
		-0.8,	-0.8,	0.0,	1.0,		1.0,	0.0,	0.0,
		0.0	,	0.8,	0.0,	1.0,		0.0,	1.0,	0.0,
		0.8,	-0.8,	0.0,	1.0,		0.0,	0.0,	1.0
	});
	std::vector<unsigned int> indices({
		0, 1, 2
	});
	// Create vertex and index buffers
	std::shared_ptr<Vermilion::Core::VertexBuffer> vertexBuffer = vmInstance.createVertexBuffer(vertices.data(), vertices.size()*sizeof(float));
	std::shared_ptr<Vermilion::Core::IndexBuffer> indexBuffer = vmInstance.createIndexBuffer(indices.data(), indices.size()*sizeof(unsigned int));

	// Create render pipeline
	std::shared_ptr<Vermilion::Core::Pipeline> pipeline = vmInstance.createPipeline(defaultRenderTarget, shaderProgram, {
		Vermilion::Core::VertexBufferLayoutElementFloat4("aPos"),
		Vermilion::Core::VertexBufferLayoutElementFloat3("aColor")
	});

	while(vmInstance.window->shouldClose()){
		vmInstance.startRender();

		// Start queueing commands to static queue
		defaultRenderTarget->start();
		defaultRenderTarget->draw(pipeline, 3, 1, 0, 0);
		defaultRenderTarget->end();

		vmInstance.endRender();
	}

	return 0;
}
