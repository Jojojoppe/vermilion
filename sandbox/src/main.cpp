#include <vermilion/vermilion.hpp>

#include <vermilion/glm/glm.hpp>
#include <vermilion/glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#include "gui.hpp"

#include <fstream>
#include <cmath>

struct UniformBufferObject{
	glm::mat4 model;
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
			Vermilion::Core::RenderPlatform::RENDER_PLATFORM_VULKAN,
			400,
			400,
			VMCORE_LOGLEVEL_TRACE,
		0};
		vmInstance.reset(new VmInstance(hintType, hintValue, this, Vermilion::Core::WindowCallbackFunctions{resize, mouseButton, mousePos, mouseEnter, scroll}));

		gui.reset(new GUI(vmInstance, 400, 400));

		size_t width, height, channels;
		unsigned char * texture1_pixels = Vermilion::Core::loadTextureData("../assets/texture1.jpg", &width, &height, &channels);
		vmInstance->createTexture(texture1, width, height, 4);
		texture1.setData(texture1_pixels);
		Vermilion::Core::freeTextureData(texture1_pixels);
		vmInstance->createSampler(sampler1, texture1);

		vmInstance->createTexture(texture2, 512, 512, 4);
		vmInstance->createSampler(sampler2, texture2);

		vmInstance->getDefaultRenderTarget(defaultRenderTarget);
		vmInstance->createRenderTarget(textureRenderTarget, texture2);

		vmInstance->createShader(vertexShader, R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable
			layout(location = 0) in vec3 aPos;
			layout(location = 1) in vec2 aTexCoord;
			layout(location = 2) in vec3 aNorm;
			layout(location = 0) out vec2 fTexCoord;
			layout(binding=0) uniform UniformBufferObject{
				mat4 model;
				mat4 view;
				mat4 proj;
			} ubo;
			
			void main() {
				gl_Position = ubo.proj * ubo.view * ubo.model * vec4(aPos, 1.0);
				fTexCoord = aTexCoord;
			}
		)", Vermilion::Core::ShaderType::SHADER_TYPE_VERTEX);
		vmInstance->createShader(fragmentShader, R"(
			#version 450
			#extension GL_ARB_separate_shader_objects : enable
			layout(location = 0) in vec2 fTexCoord;
			layout(location = 0) out vec4 outColor;
			layout(binding = 1) uniform sampler2D s_tex;
			layout(push_constant) uniform uColMul_u {
				float col;
			} uColMul;

			void main() {
				outColor = vec4(texture(s_tex, fTexCoord).xyz * uColMul.col, 1.0);
			}
		)", Vermilion::Core::ShaderType::SHADER_TYPE_FRAGMENT);
		vmInstance->createShaderProgram(shaderProgram, {&vertexShader, &fragmentShader});

		vmInstance->createPipelineLayout(pipelineLayout, {
				Vermilion::Core::BufferLayoutElementFloat3("aPos"),
				Vermilion::Core::BufferLayoutElementFloat2("aTexCoord"),
				Vermilion::Core::BufferLayoutElementFloat3("aNorm")
			},{
				Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER,
				Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_SAMPLER,
			},{
				Vermilion::Core::PipelineLayoutUniformFloat1("uColMul")
		});

		vmInstance->createPipeline(pipeline1, defaultRenderTarget, shaderProgram, pipelineLayout, Vermilion::Core::PipelineSettings{
			Vermilion::Core::PipelineSettingsDepthTest::PIPELINE_SETTINGS_DEPTH_TEST_ENABLED,
			Vermilion::Core::PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_NONE,
			Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE
		});
		vmInstance->createPipeline(pipeline2, textureRenderTarget, shaderProgram, pipelineLayout, Vermilion::Core::PipelineSettings{
			Vermilion::Core::PipelineSettingsDepthTest::PIPELINE_SETTINGS_DEPTH_TEST_ENABLED,
			Vermilion::Core::PipelineSettingsCullMode::PIPELINE_SETTINGS_CULL_MODE_NONE,
			Vermilion::Core::PipelineSettingsPolygonMode::PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE
		});
		pipeline2.setViewport(texture2.width(), texture2.height(), 0, 0);
		pipeline2.setScissor(texture2.width(), texture2.height(), 0, 0);

		std::vector<float> vertices;
		std::vector<unsigned int> indices;

		{	// LOAD OBJ
			struct vertex_s{
				float x, y, z;
			};
			struct normal_s{
				float x, y, z;
			};
			struct uv_s{
				float u, v;
			};
			struct face_s{
				unsigned int v1, v2, v3, n1, n2, n3, t1, t2, t3;
			};
			std::ifstream in("../assets/cube.obj");
			std::vector<std::string*> lines;
			std::vector<vertex_s*> verts = {nullptr};
			std::vector<normal_s*> norms = {nullptr};
			std::vector<uv_s*> uvs = {nullptr};
			std::vector<face_s*> faces;
			char buf[512];
			while(!in.eof()){
				in.getline(buf, 512);
				lines.push_back(new std::string(buf));
			}

			for(int i=0; i<lines.size(); i++){
				if(lines[i]->c_str()[0]=='#') continue;		// Comment
				if(lines[i]->c_str()[0]=='o') continue;		// Object name
				if(lines[i]->c_str()[0]=='s') continue;		// Smooth shading
				if(lines[i]->c_str()[0]=='v' && lines[i]->c_str()[1]==' '){		// vertex
					float x, y, z;
					sscanf(lines[i]->c_str(), "v %f %f %f", &x, &y, &z);
					verts.push_back(new vertex_s{x, y, z});
				}
				if(lines[i]->c_str()[0]=='v' && lines[i]->c_str()[1]=='n'){		// normal
					float x, y, z;
					sscanf(lines[i]->c_str(), "vn %f %f %f", &x, &y, &z);
					norms.push_back(new normal_s{x, y, z});
				}
				if(lines[i]->c_str()[0]=='v' && lines[i]->c_str()[1]=='t'){		// UV
					float x, y;
					sscanf(lines[i]->c_str(), "vt %f %f", &x, &y);
					uvs.push_back(new uv_s{x, y});
				}
				if(lines[i]->c_str()[0]=='f' && lines[i]->c_str()[1]==' '){		// face
					unsigned int v1, v2, v3, n1, n2, n3, t1, t2, t3;
					sscanf(lines[i]->c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
					faces.push_back(new face_s{v1, v2, v3, n1, n2, n3, t1, t2, t3});
				}
			}
			int i=0;
			for(auto f : faces){
				// Vertex layout: pos UV norm
				vertices.push_back(verts[f->v1]->x);
				vertices.push_back(verts[f->v1]->y);
				vertices.push_back(verts[f->v1]->z);
				vertices.push_back(uvs[f->t1]->u);
				vertices.push_back(uvs[f->t1]->v);
				vertices.push_back(norms[f->n1]->x);
				vertices.push_back(norms[f->n1]->y);
				vertices.push_back(norms[f->n1]->z);

				vertices.push_back(verts[f->v2]->x);
				vertices.push_back(verts[f->v2]->y);
				vertices.push_back(verts[f->v2]->z);
				vertices.push_back(uvs[f->t2]->u);
				vertices.push_back(uvs[f->t2]->v);
				vertices.push_back(norms[f->n2]->x);
				vertices.push_back(norms[f->n2]->y);
				vertices.push_back(norms[f->n2]->z);

				vertices.push_back(verts[f->v3]->x);
				vertices.push_back(verts[f->v3]->y);
				vertices.push_back(verts[f->v3]->z);
				vertices.push_back(uvs[f->t3]->u);
				vertices.push_back(uvs[f->t3]->v);
				vertices.push_back(norms[f->n3]->x);
				vertices.push_back(norms[f->n3]->y);
				vertices.push_back(norms[f->n3]->z);

				indices.push_back(i+0);
				indices.push_back(i+1);
				indices.push_back(i+2);
				i+=3;
			}
			for(int i=0; i<lines.size(); i++) delete lines[i];
			for(int i=0; i<verts.size(); i++) delete verts[i];
			for(int i=0; i<norms.size(); i++) delete norms[i];
			for(int i=0; i<faces.size(); i++) delete faces[i];
			for(int i=0; i<uvs.size(); i++) delete uvs[i];
		}

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
		vmInstance->createBuffer(uniformBuffer2, sizeof(UniformBufferObject),
			Vermilion::Core::BufferType::BUFFER_TYPE_UNIFORM,
			Vermilion::Core::BufferUsage::BUFFER_USAGE_WRITE_ONLY,
			Vermilion::Core::BufferDataUsage::BUFFER_DATA_USAGE_DYNAMIC
		);
		
		ubo1.model = glm::mat4(1.0f);
		ubo1.view = glm::lookAt(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0, 0, 1));
		ubo1.proj = glm::perspective(glm::radians(45.0f), (float)400/400, 0.1f, 10.0f);
		ubo2 = ubo1;
		
		vmInstance->createBinding(binding1, {&uniformBuffer1}, {&sampler2});
		vmInstance->createBinding(binding2, {&uniformBuffer2}, {&sampler1});

	}
	void run(){

		float uColMul;
		float time = 0.0f;

		while(vmInstance->shouldClose()){
			vmInstance->startRender();

			ubo1.model = glm::rotate(ubo1.model, 0.01f, glm::vec3(0.0f, 0.0f, 1.0f));
			ubo2.model = glm::rotate(ubo2.model, -0.02f, glm::vec3(0.0f, 0.0f, 1.0f));
			uniformBuffer1.setData(&ubo1);
			uniformBuffer2.setData(&ubo2);

			uColMul = 1.0f;
			textureRenderTarget.start(1.0, 0.0, 0.0, 1.0);
			textureRenderTarget.setUniform(pipeline2, "uColMul", &uColMul);
			textureRenderTarget.draw(pipeline2, binding2, object, 1, 0);
			textureRenderTarget.end();

			uColMul = std::fmod(time, 2.0f);
			defaultRenderTarget.start(1.0, 1.0, 1.0, 1.0);
			defaultRenderTarget.setUniform(pipeline2, "uColMul", &uColMul);
			defaultRenderTarget.draw(pipeline1, binding1, object, 1, 0);
			gui->render();
			defaultRenderTarget.end();

			vmInstance->endRender({&textureRenderTarget});
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