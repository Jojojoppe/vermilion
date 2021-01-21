#include <stdexcept>
#include <vermilion/instance.hpp>
#include <vermilion/vermilion.hpp>

#include "api.hpp"
#include "Buffer.hpp"
#include "Pipeline.hpp"
#include "Renderable.hpp"
#include "RenderTarget.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

Vermilion::Core::Instance::Instance(int * hintType, int * hintValue, Vermilion::Core::WindowCallbackFunctions windowCallbackFunctions){
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
	this->window.reset(Window::create(platform_window, platform_render, windowCallbackFunctions, this));

	this->window->createWindow(window_width, window_height);

	rendertargets.insert(std::pair<unsigned int, std::shared_ptr<RenderTarget>>(0, api->getDefaultRenderTarget()));
}

Vermilion::Core::Instance::~Instance(){
	this->logger.log(VMCORE_LOGLEVEL_INFO, "Deinitializing Vermilion");
	
	// Reset all vermilion objects
	for(auto& r : this->renderables) r.second.reset();
	for(auto& r : this->rendertargets) r.second.reset();
	for(auto& t : this->textures) t.second.reset();
	for(auto& s : this->shaders) s.second.reset();
	for(auto& s : this->shaderprograms) s.second.reset();
	for(auto& p : this->pipelinelayouts) p.second.reset();
	for(auto& p : this->pipelines) p.second.reset();
	for(auto& s : this->samplers) s.second.reset();
	for(auto& b : this->buffers) b.second.reset();
	for(auto& b : this->bindings) b.second.reset();

	this->api.reset();
	this->window.reset();

	this->logger.log(VMCORE_LOGLEVEL_DEBUG, "Deinitialized Vermilion");
}

void Vermilion::Core::Instance::startRender(){
	this->window->startRender();
}

void Vermilion::Core::Instance::endRender(std::initializer_list<VmRenderTarget*> extraRenderTargets){
	std::vector<std::shared_ptr<RenderTarget>> rdts;
	for(auto r : extraRenderTargets){
		rdts.push_back(this->rendertargets[r->ID]);
	}

	this->window->endRender(rdts);
}

VmRenderTarget Vermilion::Core::Instance::getDefaultRenderTarget(){
	auto defrentar = this->api->getDefaultRenderTarget();
	VmRenderTarget t;
	t.ID = 0;
	t.instance = this;
	return t;
}

VmRenderTarget Vermilion::Core::Instance::createRenderTarget(VmTexture& texture){
	if(texture.ID==0){
		logger.log(VMCORE_LOGLEVEL_FATAL, "Texture not initialized");
		throw std::runtime_error("Vermilion::Core::Instance::createRenderTarget() - Texture not initialized");
	}
	auto tex = this->textures[texture.ID];
	auto rdt = this->api->createRenderTarget(tex);
	this->rendertargets.insert(std::pair<unsigned int, std::shared_ptr<RenderTarget>>(IDcounter, rdt));
	VmRenderTarget t;
	t.ID = IDcounter++;
	t.instance = this;
	return t;
}

VmShader Vermilion::Core::Instance::createShader(const std::string& source, Vermilion::Core::ShaderType type){
	auto shd = this->api->createShader(source, type);
	this->shaders.insert(std::pair<unsigned int, std::shared_ptr<Shader>>(IDcounter, shd));
	VmShader s;
	s.ID = IDcounter++;
	s.instance = this;
	return s;
}

VmShaderProgram Vermilion::Core::Instance::createShaderProgram(std::initializer_list<VmShader*> shaders){
	std::vector<std::shared_ptr<Shader>> shds;
	for(auto& s : shaders){
		shds.push_back(this->shaders[s->ID]);
	}
	auto shp = this->api->createShaderProgram(shds);
	this->shaderprograms.insert(std::pair<unsigned int, std::shared_ptr<ShaderProgram>>(IDcounter, shp));
	VmShaderProgram s;
	s.ID = IDcounter++;
	s.instance = this;
	return s;
}

VmPipelineLayout Vermilion::Core::Instance::createPipelineLayout(std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout, std::initializer_list<Vermilion::Core::PipelineLayoutBinding> bindings){
	auto pll = this->api->createPipelineLayout(vertexLayout, bindings);
	this->pipelinelayouts.insert(std::pair<unsigned int, std::shared_ptr<PipelineLayout>>(IDcounter, pll));
	VmPipelineLayout p;
	p.ID = IDcounter++;
	p.instance = this;
	return p;
}

VmPipeline Vermilion::Core::Instance::createPipeline(VmRenderTarget& renderTarget, VmShaderProgram& shaderProgram, VmPipelineLayout& pipelineLayout, Vermilion::Core::PipelineSettings settings){
	auto rt = this->rendertargets[renderTarget.ID];
	auto sp = this->shaderprograms[shaderProgram.ID];
	auto pll = this->pipelinelayouts[pipelineLayout.ID];
	auto pl = this->api->createPipeline(rt, sp, pll, settings);
	this->pipelines.insert(std::pair<unsigned int, std::shared_ptr<Pipeline>>(IDcounter, pl));
	VmPipeline p;
	p.ID = IDcounter++;
	p.instance = this;
	return p;
}

VmBinding Vermilion::Core::Instance::createBinding(std::initializer_list<VmBuffer*> buffers, std::initializer_list<VmSampler*> samplers){
	std::vector<std::shared_ptr<Buffer>> bfrs;
	std::vector<std::shared_ptr<Sampler>> smps;
	for(auto& b : buffers) bfrs.push_back(this->buffers[b->ID]);
	for(auto& s : samplers) smps.push_back(this->samplers[s->ID]);
	auto bind = this->api->createBinding(bfrs, smps);
	this->bindings.insert(std::pair<unsigned int, std::shared_ptr<Binding>>(IDcounter, bind));
	VmBinding b;
	b.ID = IDcounter++;
	b.instance = this;
	return b;
}

VmBuffer Vermilion::Core::Instance::createBuffer(size_t size, Vermilion::Core::BufferType type, Vermilion::Core::BufferUsage usage, Vermilion::Core::BufferDataUsage dataUsage){
	auto buf = this->api->createBuffer(size, type, usage, dataUsage);
	this->buffers.insert(std::pair<unsigned int, std::shared_ptr<Buffer>>(IDcounter, buf));
	VmBuffer b;
	b.ID = IDcounter++;
	b.instance = this;
	return b;
}

VmRenderable Vermilion::Core::Instance::createRenderable(VmBuffer& vertexBuffer, VmBuffer& indexBuffer, unsigned int vertexOffset, unsigned int indexOffset, unsigned int length){
	if(vertexBuffer.type()!=BUFFER_TYPE_VERTEX || indexBuffer.type()!=BUFFER_TYPE_INDEX){
		this->logger.log(VMCORE_LOGLEVEL_FATAL, "Buffers not of right type");
		throw std::runtime_error("Vermilion::Core::nstance::CreateRenderable() - Buffers not of right type");
	}
	auto vb = this->buffers[vertexBuffer.ID];
	auto ib = this->buffers[indexBuffer.ID];
	auto ren = this->api->createRenderable(vb, ib, vertexOffset, indexOffset, length);
	this->renderables.insert(std::pair<unsigned int, std::shared_ptr<Renderable>>(IDcounter, ren));
	VmRenderable r;
	r.ID = IDcounter++;
	r.instance = this;
	return r;
}

VmTexture Vermilion::Core::Instance::createTexture(size_t width, size_t height, unsigned int channels){
	auto tex = this->api->createTexture(width, height, channels);
	this->textures.insert(std::pair<unsigned int, std::shared_ptr<Texture>>(IDcounter, tex));
	VmTexture t;
	t.ID = IDcounter++;
	t.instance = this;
	tex = textures[t.ID];
	return t;
}

VmSampler Vermilion::Core::Instance::createSampler(VmTexture& texture){
	auto tex = this->textures[texture.ID];
	auto sam = this->api->createSampler(tex);
	this->samplers.insert(std::pair<unsigned int, std::shared_ptr<Sampler>>(IDcounter, sam));
	VmSampler s;
	s.ID = IDcounter++;
	s.instance = this;
	return s;
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

