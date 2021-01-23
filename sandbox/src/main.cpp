#include <vermilion/vermilion.hpp>
#include <vermilion/utils.hpp>

#include <vermilion/glm/glm.hpp>
#include <vermilion/glm/gtc/matrix_transform.hpp>
#include "imgui/imgui.h"
#include <memory>
#include <vector>
#include <unordered_map>

#include "gui.hpp"

#include <fstream>
#include <cmath>

class RenderObject{
	private:
		std::shared_ptr<VmInstance> instance;

		VmBuffer vbuffer, ibuffer;
		std::vector<VmRenderable> renderables;
		std::vector<std::string> renderableMaterials;

		VmTexture whiteTexture;
		VmSampler whiteTextureSampler;

		std::vector<VmTexture> textures;
		std::vector<VmSampler> samplers;
		std::vector<VmBinding> bindings;
		std::unordered_map<std::string, unsigned int> renderableBindingIndex;

		std::unordered_map<std::string, glm::vec4> uKd;
		std::unordered_map<std::string, glm::vec4> uKa;

	public:
		RenderObject(std::shared_ptr<VmInstance>& instance, const std::string& path, const std::string& objname){
			this->instance = instance;

			std::vector<float> vertices;
			std::vector<unsigned int> indices;
			std::vector<Vermilion::Utils::mesh> meshes;
			std::unordered_map<std::string, Vermilion::Utils::material> materials;

			Vermilion::Utils::loadObj(vertices, indices, meshes, materials, objname, path);

			instance->createBuffer(vbuffer, sizeof(float)*vertices.size(), Vermilion::Core::BufferType::BUFFER_TYPE_VERTEX, Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY, Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_STATIC);
			instance->createBuffer(ibuffer, sizeof(float)*indices.size(), Vermilion::Core::BufferType::BUFFER_TYPE_INDEX, Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY, Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_STATIC);
			vbuffer.setData(vertices.data());
			ibuffer.setData(indices.data());

			// Create meshes
			renderables.resize(meshes.size());
			int i=0;
			for(auto& m : meshes){
				instance->createRenderable(renderables[i], vbuffer, ibuffer, 0, m.offset*3, m.length*3);
				renderableMaterials.push_back(m.materialName);
				i++;
			}

			instance->createTexture(whiteTexture, 1, 1, 4);
			unsigned char white[4] = {255, 255, 255, 255};
			whiteTexture.setData(white);
			instance->createSampler(whiteTextureSampler, whiteTexture);

			// Create materials
			textures.resize(materials.size());
			samplers.resize(materials.size());
			bindings.resize(materials.size());
			i = 0;
			for(auto& m : materials){
				std::vector<VmSampler*> bindingSamplers;
				std::vector<VmBuffer*> bindingBuffers;

				// Check for diffuse texture
				if(m.second.use_tex_d){
					size_t width, height, channels;
					unsigned char * pixels = Vermilion::Core::loadTextureData(m.second.tex_d, &width, &height, &channels);
					instance->createTexture(textures[i], width, height, channels);
					textures[i].setData(pixels);
					Vermilion::Core::freeTextureData(pixels);
					instance->createSampler(samplers[i], textures[i]);
					bindingSamplers.push_back(&samplers[i]);
				}else{
					bindingSamplers.push_back(&whiteTextureSampler);
				}

				uKd.insert(std::pair<std::string, glm::vec4>(m.first, glm::vec4(m.second.Kdr, m.second.Kdg, m.second.Kdb, 1.0f)));
				uKa.insert(std::pair<std::string, glm::vec4>(m.first, glm::vec4(m.second.Kar, m.second.Kag, m.second.Kab, 1.0f)));

				instance->createBinding(bindings[i], bindingBuffers, bindingSamplers);
				renderableBindingIndex.insert(std::pair<std::string, unsigned int>(m.first, i));
				i++;
			}	

		}

		void draw(VmRenderTarget& target, VmPipeline& pipeline){
			for(int i=0; i<renderables.size(); i++){
				unsigned int bindingIndex = renderableBindingIndex[renderableMaterials[i]];
				target.setUniform(pipeline, "uKd", &(uKd[renderableMaterials[i]]));
				target.setUniform(pipeline, "uKa", &(uKa[renderableMaterials[i]]));
				target.draw(pipeline, bindings[bindingIndex], renderables[i], 1, 0);
			}
		}
};

struct Application{

	std::shared_ptr<VmInstance> vmInstance;

	VmRenderTarget defaultRenderTarget;
	VmShader vertexShader, fragmentShader;
	VmShaderProgram shaderProgram;

	VmPipelineLayout pipelineLayout;
	VmPipeline pipeline;

	std::unique_ptr<GUI> gui;

	std::unique_ptr<RenderObject> object1;

	glm::mat4 view, projection;

	static void resize(VmInstance * instance, void * userPointer, int width, int height){
		Application * app = (Application*) userPointer;
		app->pipeline.setViewport(width, height, 0, 0);
		app->pipeline.setScissor(width, height, 0, 0);
		app->projection = glm::perspective(glm::radians(45.0f), width/(float)height, 0.1f, 10.0f);
		app->gui->resize(width, height);
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
		vmInstance->getDefaultRenderTarget(defaultRenderTarget);

		vmInstance->createShader(vertexShader, R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable
			#extension GL_KHR_vulkan_glsl : enable

			layout(location = 0) in vec4 aPos;
			layout(location = 1) in vec3 aTexCoord;
			layout(location = 2) in vec3 aNorm;
			layout(location = 0) out vec2 fTexCoord;

			#ifdef VULKAN
				layout(push_constant) uniform PC0{
					mat4 uModel;
					mat4 uView;
					mat4 uProjection;
					vec4 uKd;
					vec4 uKa;
				};
			#else
				uniform mat4 uModel;
				uniform mat4 uView;
				uniform mat4 uProjection;
				uniform vec4 uKd;
				uniform vec4 uKa;
			#endif
			
			void main() {
				gl_Position = uProjection * uView * uModel * aPos;
				fTexCoord = aTexCoord.xy;
			}
		)", Vermilion::Core::ShaderType::SHADER_TYPE_VERTEX);
		vmInstance->createShader(fragmentShader, R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable
			#extension GL_KHR_vulkan_glsl : enable

			layout(location = 0) in vec2 fTexCoord;
			layout(location = 0) out vec4 outColor;
			layout(binding = 0) uniform sampler2D s_tex;

			#ifdef VULKAN
				layout(push_constant) uniform PC0{
					mat4 uModel;
					mat4 uView;
					mat4 uProjection;
					vec4 uKd;
					vec4 uKa;
				};
			#else
				uniform mat4 uModel;
				uniform mat4 uView;
				uniform mat4 uProjection;
				uniform vec4 uKd;
				uniform vec4 uKa;
			#endif

			void main() {
				outColor = vec4(texture(s_tex, fTexCoord).xyz, 1.0) * uKd * uKa;
			}
		)", Vermilion::Core::ShaderType::SHADER_TYPE_FRAGMENT);
		vmInstance->createShaderProgram(shaderProgram, {&vertexShader, &fragmentShader});

		vmInstance->createPipelineLayout(pipelineLayout, {
				Vermilion::Core::BufferLayoutElementFloat4("aPos"),
				Vermilion::Core::BufferLayoutElementFloat3("aTexCoord"),
				Vermilion::Core::BufferLayoutElementFloat3("aNorm")
			},{
				Vermilion::Core::PipelineLayoutBinding(Vermilion::Core::PipelineLayoutBindingType::PIPELINE_LAYOUT_BINDING_SAMPLER)
			},{
				Vermilion::Core::PipelineLayoutUniformMat4("uModel"),
				Vermilion::Core::PipelineLayoutUniformMat4("uView"),
				Vermilion::Core::PipelineLayoutUniformMat4("uProjection"),
				Vermilion::Core::PipelineLayoutUniformFloat4("uKd"),
				Vermilion::Core::PipelineLayoutUniformFloat4("uKa"),
		});

		vmInstance->createPipeline(pipeline, defaultRenderTarget, shaderProgram, pipelineLayout, Vermilion::Core::PipelineSettings{
			Vermilion::Core::PipelineSettingsDepthTest::PIPELINE_SETTINGS_DEPTH_TEST_ENABLED,
			Vermilion::Core::PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_NONE,
			Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE
		});

		// Load object
		object1.reset(new RenderObject(vmInstance, "../assets/saloon", "saloon"));
	}
	void run(){

		view = glm::lookAt(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 0, 1));
		projection = glm::perspective(glm::radians(45.0f), (float)400/400, 0.1f, 10.0f);
		glm::mat4 model1 = glm::mat4(1.0);

		while(vmInstance->shouldClose()){
			vmInstance->startRender();
				defaultRenderTarget.start(0.0, 0.0, 0.0, 1.0);

					model1 = glm::rotate(model1, 0.01f, glm::vec3(0,0,1));
					defaultRenderTarget.setUniform(pipeline, "uModel", &model1);
					defaultRenderTarget.setUniform(pipeline, "uView", &view);
					defaultRenderTarget.setUniform(pipeline, "uProjection", &projection);
					object1->draw(defaultRenderTarget, pipeline);

					ImGui::Begin("Settings");
					ImGui::End();

					gui->render();

				defaultRenderTarget.end();
			vmInstance->endRender({});
		}
	}
};

int main(int argc, char ** argv){
	// try{
		Application A;
		A.run();
	// }catch(std::exception& ex){
		// return 1;
	// }
	// return 0;
}