#include <vermilion/instance.hpp>

Vermilion::Core::Instance::Instance(int * hintType, int * hintValue){
	this->logger.log(VMCORE_LOGLEVEL_INFO, "Initializing Vermilion");

	// Parse hints
	platform_render = parseHintType_RENDER_PLATFORM(hintType, hintValue);
	this->logger.log(VMCORE_LOGLEVEL_INFO, "RENDER_PLATFORM: %s", Vermilion::Core::RenderPlatformString[platform_render].c_str());
	platform_window = parseHintType_WINDOW_PLATFORM(hintType, hintValue);
	this->logger.log(VMCORE_LOGLEVEL_INFO, "WINDOW_PLATFORM: %s", Vermilion::Core::WindowPlatformString[platform_window].c_str());
	int window_height = parseHintType_WINDOW_HEIGHT(hintType, hintValue);
	int window_width = parseHintType_WINDOW_WIDTH(hintType, hintValue);
	int loglevel = parseHintType_LOGLEVEL(hintType, hintValue);
	this->logger.setLevel(loglevel);

	// Create API
	this->api.reset(API::create(platform_render, this));
	// Create Window
	this->window.reset(Window::create(platform_window, platform_render, this));

	this->window->createWindow(window_width, window_height);
}

Vermilion::Core::Instance::~Instance(){
	this->logger.log(VMCORE_LOGLEVEL_INFO, "Deinitializing Vermilion");

	this->api.reset();
	this->window.reset();

	this->logger.log(VMCORE_LOGLEVEL_DEBUG, "Deinitialized Vermilion");
}

void Vermilion::Core::Instance::startRender(){
	this->window->startRender();
}

void Vermilion::Core::Instance::endRender(std::initializer_list<std::shared_ptr<Vermilion::Core::RenderTarget>> extraRenderTargets){
	this->window->endRender(extraRenderTargets);
}

std::shared_ptr<Vermilion::Core::RenderTarget> Vermilion::Core::Instance::getDefaultRenderTarget(){
	return this->api->getDefaultRenderTarget();
}

std::shared_ptr<Vermilion::Core::RenderTarget> Vermilion::Core::Instance::createRenderTarget(std::shared_ptr<Vermilion::Core::Texture> texture){
	return this->api->createRenderTarget(texture);
}

std::shared_ptr<Vermilion::Core::Shader> Vermilion::Core::Instance::createShader(const std::string& source, Vermilion::Core::ShaderType type){
	return this->api->createShader(source, type);
}

std::shared_ptr<Vermilion::Core::ShaderProgram> Vermilion::Core::Instance::createShaderProgram(std::initializer_list<std::shared_ptr<Vermilion::Core::Shader>> shaders){
	return this->api->createShaderProgram(shaders);
}

std::shared_ptr<Vermilion::Core::Pipeline> Vermilion::Core::Instance::createPipeline(std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, 
		std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, Vermilion::Core::PipelineSettings settings, std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout,
		std::initializer_list<Vermilion::Core::PipelineLayoutBinding> layoutBindings){
	return this->api->createPipeline(renderTarget, shaderProgram, settings, vertexLayout, layoutBindings);
}

std::shared_ptr<Vermilion::Core::Binding> Vermilion::Core::Instance::createBinding(std::initializer_list<std::shared_ptr<Vermilion::Core::UniformBuffer>> uniformBuffers, std::initializer_list<std::shared_ptr<Vermilion::Core::Sampler>> samplers){
	return this->api->createBinding(uniformBuffers, samplers);
}

std::shared_ptr<Vermilion::Core::VertexBuffer> Vermilion::Core::Instance::createVertexBuffer(size_t size, Vermilion::Core::BufferType type){
	return this->api->createVertexBuffer(size, type);
}

std::shared_ptr<Vermilion::Core::IndexBuffer> Vermilion::Core::Instance::createIndexBuffer(size_t size, Vermilion::Core::BufferType type){
	return this->api->createIndexBuffer(size, type);
}

std::shared_ptr<Vermilion::Core::UniformBuffer> Vermilion::Core::Instance::createUniformBuffer(size_t size, Vermilion::Core::BufferType type){
	return this->api->createUniformBuffer(size, type);
}

std::shared_ptr<Vermilion::Core::StorageBuffer> Vermilion::Core::Instance::createStorageBuffer(size_t size, Vermilion::Core::BufferType type){
	return this->api->createStorageBuffer(size, type);
}

std::shared_ptr<Vermilion::Core::Renderable> Vermilion::Core::Instance::createRenderable(std::shared_ptr<Vermilion::Core::VertexBuffer> vertexBuffer, std::shared_ptr<Vermilion::Core::IndexBuffer> indexBuffer, unsigned int vertexOffset, unsigned int indexOffset, unsigned int length){
	return this->api->createRenderable(vertexBuffer, indexBuffer, vertexOffset, indexOffset, length);
}

std::shared_ptr<Vermilion::Core::Texture> Vermilion::Core::Instance::createTexture(const std::string& path, size_t width, size_t height, unsigned int channels){
	return this->api->createTexture(path, width, height, channels);
}
std::shared_ptr<Vermilion::Core::Texture> Vermilion::Core::Instance::createTexture(void * data, size_t width, size_t height, unsigned int channels){
	return this->api->createTexture(data, width, height, channels);
}



std::shared_ptr<Vermilion::Core::Sampler> Vermilion::Core::Instance::createSampler(std::shared_ptr<Vermilion::Core::Texture> texture){
	return this->api->createSampler(texture);
}

int Vermilion::Core::Instance::parseHintType_RENDER_PLATFORM(int * hintType, int * hintValue){
	int API = Vermilion::Core::renderPlatform[0];

	if(!hintType){
		return API;
	}

	while(*hintType){
	
		if((*hintType)==Vermilion::Core::HintType::HINT_TYPE_RENDER_PLATFORM){
			// Check if hint can be applied
			const int * rp = Vermilion::Core::renderPlatform;
			while(*rp){
				if(*rp == *hintValue){
					// hint can be applied
					API = *rp;
					return API;
				}
				rp++;
			}
			// hint cannot be applied, choose first in possible values
			API = rp[0];
			return API;
		}

		hintType++;
		hintValue++;
	}
	return API;
}

int Vermilion::Core::Instance::parseHintType_WINDOW_PLATFORM(int * hintType, int * hintValue){
	int WINDOW = Vermilion::Core::windowPlatform[0];

	if(!hintType){
		return WINDOW;
	}

	while(*hintType){
	
		if((*hintType)==Vermilion::Core::HintType::HINT_TYPE_WINDOW_PLATFORM){
			// Check if hint can be applied
			const int * wp = Vermilion::Core::windowPlatform;
			while(*wp){
				if(*wp == *hintValue){
					// hint can be applied
					WINDOW = *wp;
					return WINDOW;
				}
				wp++;
			}
			// hint cannot be applied, choose first in possible values
			WINDOW = wp[0];
			return WINDOW;
		}

		hintType++;
		hintValue++;
	}
	return WINDOW;
}

int Vermilion::Core::Instance::parseHintType_WINDOW_HEIGHT(int * hintType, int * hintValue){
	int HEIGHT = 800;

	if(!hintType){
		return HEIGHT;
	}

	while(*hintType){
	
		if((*hintType)==Vermilion::Core::HintType::HINT_TYPE_WINDOW_HEIGHT){
			HEIGHT = *hintValue;
			return HEIGHT;
		}

		hintType++;
		hintValue++;
	}
	return HEIGHT;
}

int Vermilion::Core::Instance::parseHintType_WINDOW_WIDTH(int * hintType, int * hintValue){
	int WIDTH = 800;

	if(!hintType){
		return WIDTH;
	}

	while(*hintType){
	
		if((*hintType)==Vermilion::Core::HintType::HINT_TYPE_WINDOW_WIDTH){
			WIDTH = *hintValue;
			return WIDTH;
		}

		hintType++;
		hintValue++;
	}
	return WIDTH;
}

int Vermilion::Core::Instance::parseHintType_LOGLEVEL(int * hintType, int * hintValue){
	int LEVEL = VMCORE_LOGLEVEL_INFO;

	if(!hintType){
		return LEVEL;
	}

	while(*hintType){
	
		if((*hintType)==Vermilion::Core::HintType::HINT_TYPE_LOGLEVEL){
			LEVEL = *hintValue;
			return LEVEL;
		}

		hintType++;
		hintValue++;
	}
	return LEVEL;
}

