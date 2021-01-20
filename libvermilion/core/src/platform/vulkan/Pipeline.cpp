#ifdef VMCORE_VULKAN
#include "Pipeline.hpp"

#include <vermilion/vermilion.hpp>
#include <vermilion/instance.hpp>
#include "api.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"

#include <string.h>
#include <stdexcept>
#include <cstdint>

VkFormat VulkanBufferLayoutElementTypeToVulkanBaseType[] = {
	VK_FORMAT_UNDEFINED,
	VK_FORMAT_R32_SFLOAT,
	VK_FORMAT_R32G32_SFLOAT,
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R32G32B32A32_SFLOAT,
	VK_FORMAT_R32_UINT,
	VK_FORMAT_R8_UNORM,
	VK_FORMAT_R8G8_UNORM,
	VK_FORMAT_R8G8B8_UNORM,
	VK_FORMAT_R8G8B8A8_UNORM,
};

Vermilion::Core::Vulkan::PipelineLayout::PipelineLayout(Vermilion::Core::Vulkan::API * api, std::initializer_list<Vermilion::Core::BufferLayoutElement> _vertexLayout, std::initializer_list<Vermilion::Core::PipelineLayoutBinding> bindings){
	this->api = api;
	this->instance = api->instance;

	// Vertex data layout
	// Calculate stride and offset
	unsigned int offset = 0;
	for(auto& element : _vertexLayout){
		auto e = element;
		e.offset = offset;
		offset += e.size;
		stride += e.size;
		this->vertexLayout.push_back(e);
	}

	// Create pipeline descriptor layout
	std::vector<VkDescriptorSetLayoutBinding> layoutBindingsVector;
	unsigned int binding=0;
	for(const auto& b : bindings){
		this->bindings.push_back(b);
		switch(b){
			case Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER:{
				VkDescriptorSetLayoutBinding uboLayoutBinding = {};
				uboLayoutBinding.binding = binding++;
				uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				uboLayoutBinding.descriptorCount = 1;
				uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
				uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
				layoutBindingsVector.push_back(uboLayoutBinding);
			} break;

			case Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_STORAGE_BUFFER:{
				VkDescriptorSetLayoutBinding uboLayoutBinding = {};
				uboLayoutBinding.binding = binding++;
				uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				uboLayoutBinding.descriptorCount = 1;
				uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
				uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
				layoutBindingsVector.push_back(uboLayoutBinding);
			} break;

			case Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_SAMPLER:{
				VkDescriptorSetLayoutBinding texLayoutBinding = {};
				texLayoutBinding.binding = binding++;
				texLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				texLayoutBinding.descriptorCount = 1;
				texLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				texLayoutBinding.pImmutableSamplers = nullptr; // Optional
				layoutBindingsVector.push_back(texLayoutBinding);
			} break;

			default:
				this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Unknown pipeline layout binding type");
		}
	}
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = binding;
	layoutInfo.pBindings = layoutBindingsVector.data();
	if(vkCreateDescriptorSetLayout(api->vk_device->vk_device, &layoutInfo, nullptr, &vk_descriptorSetLayout)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create descriptor set layout");
		throw std::runtime_error("Vermilion::Core::Vulkan::PipelineLayout::PipelineLayout() - Could not create descriptor set layout");
	}

	// Pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &vk_descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
	if(vkCreatePipelineLayout(api->vk_device->vk_device, &pipelineLayoutInfo, nullptr, &this->vk_pipelineLayout)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create pipeline layout");
		throw std::runtime_error("Vermilion::Core::Vulkan::PipelineLayout::PipelineLayout() - Could not create pipeline layout");
	}
	
}

Vermilion::Core::Vulkan::PipelineLayout::~PipelineLayout(){
	vkDestroyPipelineLayout(api->vk_device->vk_device, vk_pipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(api->vk_device->vk_device, vk_descriptorSetLayout, nullptr);
}

Vermilion::Core::Vulkan::Pipeline::Pipeline(Vermilion::Core::Vulkan::API * api, std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, 
		std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, Vermilion::Core::PipelineSettings settings, std::shared_ptr<Vermilion::Core::PipelineLayout> pipelineLayout){
	this->api = api;
	this->instance = api->instance;
	this->settings = settings;
	this->pipelineLayout = std::static_pointer_cast<Vermilion::Core::Vulkan::PipelineLayout>(pipelineLayout);

	this->renderTarget = renderTarget;
	this->shaderProgram = shaderProgram;

	this->createDescriptorPool();

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = (float) api->vk_swapchain->swapChainExtent.height;
	viewport.width = (float) api->vk_swapchain->swapChainExtent.width;
	viewport.height = -(float) api->vk_swapchain->swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = api->vk_swapchain->swapChainExtent;
	
	this->viewport = viewport;
	this->scissor = scissor;

	this->create();
}

Vermilion::Core::Vulkan::Pipeline::~Pipeline(){
	this->destroy();
	for(auto& pool : this->vk_descriptorPool){
		vkDestroyDescriptorPool(this->api->vk_device->vk_device, pool, nullptr);
	}
}

void Vermilion::Core::Vulkan::Pipeline::createDescriptorPool(){
	// Create descriptor pool for the pipeline
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// poolSizes[0].descriptorCount = static_cast<uint32_t>(api->vk_swapchain->swapChainImages.size());
	poolSizes[0].descriptorCount = 16;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// poolSizes[1].descriptorCount = static_cast<uint32_t>(api->vk_swapchain->swapChainImages.size());
	poolSizes[1].descriptorCount = 16;
	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	// poolInfo.maxSets = static_cast<uint32_t>(api->vk_swapchain->swapChainImages.size());
	poolInfo.maxSets = 16;
	VkDescriptorPool descriptorPool;
	if(vkCreateDescriptorPool(api->vk_device->vk_device, &poolInfo, nullptr, &descriptorPool)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create descriptor pool");
		throw std::runtime_error("Vermilion::Core::Vulkan::Pipeline::Pipeline() - Could not create descriptor pool");
	}
	this->vk_descriptorPool.push_back(descriptorPool);
}

void Vermilion::Core::Vulkan::Pipeline::create(){
	std::static_pointer_cast<Vermilion::Core::Vulkan::ShaderProgram>(shaderProgram)->createModules();
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfo;
	for(const auto& shader : shaderProgram->shaders){
		shaderStageInfo.push_back(std::static_pointer_cast<Vermilion::Core::Vulkan::Shader>(shader)->vk_shaderStageInfo);
	}
	VkPipelineShaderStageCreateInfo * shaderStages = shaderStageInfo.data();

	// Create descriptors
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = this->pipelineLayout->stride;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	int i = 0;
	for(auto& element : this->pipelineLayout->vertexLayout){
		VkVertexInputAttributeDescription desc = {};
		desc.binding = 0;
		desc.location = i++;
		desc.format = VulkanBufferLayoutElementTypeToVulkanBaseType[element.type];
		desc.offset = element.offset;
		attributeDescriptions.push_back(desc);
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();                                                                                                                                                                                                                                                                                                                       

	// Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	if(settings.poygonmode==PIPELINE_SETTINGS_POLYGON_MODE_POINT){
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	}
	if(settings.poygonmode==PIPELINE_SETTINGS_POLYGON_MODE_LINE){
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	}
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	if(settings.poygonmode==PIPELINE_SETTINGS_POLYGON_MODE_TRIANGLE_LINE){
		rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
	}
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	if(settings.cullmode==PIPELINE_SETTINGS_CULL_MODE_BACK_C){
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	}
	if(settings.cullmode==PIPELINE_SETTINGS_CULL_MODE_BACK_CC){
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	}
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	// Depth buffer
	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_FALSE;
	if(this->settings.depthtest==PIPELINE_SETTINGS_DEPTH_TEST_ENABLED){
		depthStencil.depthTestEnable = VK_TRUE;
	}
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = -1.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	// Color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	// Dynamic states
	std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo dynamicCreateInfo = {};
	dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicCreateInfo.pDynamicStates = dynamicStateEnables.data();
	dynamicCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

	// Viewport and Scissors
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// Pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = shaderProgram->shaders.size();
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicCreateInfo; // Optional
	pipelineInfo.layout = this->pipelineLayout->vk_pipelineLayout;
	pipelineInfo.renderPass = std::static_pointer_cast<Vermilion::Core::Vulkan::RenderTarget>(renderTarget)->renderpass->vk_renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if(vkCreateGraphicsPipelines(api->vk_device->vk_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->vk_pipeline)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create pipeline");
		throw std::runtime_error("Vermilion::Core::Vulkan::Pipeline::Pipeline() - Could not create pipeline");
	}
	std::static_pointer_cast<Vermilion::Core::Vulkan::ShaderProgram>(shaderProgram)->destroyModules();
}

void Vermilion::Core::Vulkan::Pipeline::destroy(){
	vkDestroyPipeline(api->vk_device->vk_device, vk_pipeline, nullptr);
}

void Vermilion::Core::Vulkan::Pipeline::setViewPort(unsigned int width, unsigned int height, unsigned int x, unsigned int y){
	this->viewport.width = (float)width;
	this->viewport.height = -1.0f*(float)height;
	this->viewport.x = (float)x;
	this->viewport.y = (float)height-(float)y;
}

void Vermilion::Core::Vulkan::Pipeline::setScissor(unsigned int width, unsigned int height, unsigned int x, unsigned int y){
	this->scissor.extent = {(uint32_t)width, (uint32_t)height};
	this->scissor.offset = {(int32_t)x, (int32_t)(y)};
}

void Vermilion::Core::Vulkan::Pipeline::bind(std::shared_ptr<Vermilion::Core::Binding> binding){
	std::shared_ptr<Vermilion::Core::Vulkan::Binding> vkBinding = std::static_pointer_cast<Vermilion::Core::Vulkan::Binding>(binding);
	if(this->descriptorSets.find(vkBinding)==this->descriptorSets.end()){
		// Descriptor set not yet created
		this->descriptorSets[vkBinding] = std::vector<VkDescriptorSet>();
		std::vector<VkDescriptorSetLayout> layouts(api->vk_swapchain->swapChainImages.size(), this->pipelineLayout->vk_descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = this->vk_descriptorPool[0];
		allocInfo.descriptorSetCount = static_cast<uint32_t>(api->vk_swapchain->swapChainImages.size());
		allocInfo.pSetLayouts = layouts.data();
		this->descriptorSets[vkBinding].resize(api->vk_swapchain->swapChainImages.size());
		VkResult res = vkAllocateDescriptorSets(api->vk_device->vk_device, &allocInfo, this->descriptorSets[vkBinding].data());
		if(res!=VK_SUCCESS){
			this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not allocate descriptor sets: %d", res);
			throw std::runtime_error("Vermilion::Core::Vulkan::Binding::Binding() - Could not allocate descriptor sets");
		}

		for(int i = 0; i<api->vk_swapchain->swapChainImages.size(); i++){
			int bo_i = 0;
			int sam_i = 0;
			int offset = 0;
			for(const auto& b : this->pipelineLayout->bindings){
				switch(b){
					case Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_UNIFORM_BUFFER:{
						std::shared_ptr<Vermilion::Core::Vulkan::Buffer> bo = std::static_pointer_cast<Vermilion::Core::Vulkan::Binding>(binding)->buffers[bo_i++];
						VkDescriptorBufferInfo bufferInfo = {};
						bufferInfo.buffer = bo->vk_buffer;
						bufferInfo.offset = 0;
						bufferInfo.range = bo->size;
						
						VkWriteDescriptorSet descriptorWrite = {};
						descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrite.dstSet = (this->descriptorSets[vkBinding])[i];
						descriptorWrite.dstBinding = offset++;
						descriptorWrite.dstArrayElement = 0;
						descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						if(bo->type!=BUFFER_TYPE_UNIFORM){
							this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Buffer in binding %d is not uniform buffer", offset-1);
							throw std::runtime_error("Vermilion::Core::Vulkan::Binding::Binding() - Buffer is not uniform buffer");
						}
						descriptorWrite.descriptorCount = 1;
						descriptorWrite.pBufferInfo = &bufferInfo;
						descriptorWrite.pImageInfo = nullptr;
						descriptorWrite.pTexelBufferView = nullptr;
						vkUpdateDescriptorSets(api->vk_device->vk_device, 1, &descriptorWrite, 0, nullptr);
					} break;

					case Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_STORAGE_BUFFER:{
						std::shared_ptr<Vermilion::Core::Vulkan::Buffer> bo = std::static_pointer_cast<Vermilion::Core::Vulkan::Binding>(binding)->buffers[bo_i++];
						VkDescriptorBufferInfo bufferInfo = {};
						bufferInfo.buffer = bo->vk_buffer;
						bufferInfo.offset = 0;
						bufferInfo.range = bo->size;
						
						VkWriteDescriptorSet descriptorWrite = {};
						descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrite.dstSet = (this->descriptorSets[vkBinding])[i];
						descriptorWrite.dstBinding = offset++;
						descriptorWrite.dstArrayElement = 0;
						descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
						if(bo->type!=BUFFER_TYPE_STORAGE){
							this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Buffer in binding %d is not storage buffer", offset-1);
							throw std::runtime_error("Vermilion::Core::Vulkan::Binding::Binding() - Buffer is not storage buffer");
						}
						descriptorWrite.descriptorCount = 1;
						descriptorWrite.pBufferInfo = &bufferInfo;
						descriptorWrite.pImageInfo = nullptr;
						descriptorWrite.pTexelBufferView = nullptr;
						vkUpdateDescriptorSets(api->vk_device->vk_device, 1, &descriptorWrite, 0, nullptr);
					} break;

					case Vermilion::Core::PipelineLayoutBinding::PIPELINE_LAYOUT_BINDING_SAMPLER:{
						std::shared_ptr<Vermilion::Core::Vulkan::Sampler> sam = std::static_pointer_cast<Vermilion::Core::Vulkan::Binding>(binding)->samplers[sam_i++];
						VkDescriptorImageInfo imageInfo = {};
						imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						imageInfo.imageView = sam->texture->vk_imageView->imageView;
						imageInfo.sampler = sam->vk_sampler;
						
						VkWriteDescriptorSet descriptorWrite = {};
						descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrite.dstSet = (this->descriptorSets[vkBinding])[i];
						descriptorWrite.dstBinding = offset++;
						descriptorWrite.dstArrayElement = 0;
						descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						descriptorWrite.descriptorCount = 1;
						descriptorWrite.pBufferInfo = nullptr;
						descriptorWrite.pImageInfo = &imageInfo;
						descriptorWrite.pTexelBufferView = nullptr;
						vkUpdateDescriptorSets(api->vk_device->vk_device, 1, &descriptorWrite, 0, nullptr);
					} break;

					default:
						this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Unknown pipeline layout binding type: %d", b);
				}
			}
		}
	}
}

Vermilion::Core::Vulkan::Binding::Binding(Vermilion::Core::Vulkan::API* api, std::initializer_list<std::shared_ptr<Vermilion::Core::Buffer>> buffers, std::initializer_list<std::shared_ptr<Vermilion::Core::Sampler>> samplers){
	this->api = api;
	this->instance = api->instance;
	for(const auto& u : buffers){
		this->buffers.push_back(std::static_pointer_cast<Vermilion::Core::Vulkan::Buffer>(u));
	}
	for(const auto& s : samplers){
		this->samplers.push_back(std::static_pointer_cast<Vermilion::Core::Vulkan::Sampler>(s));
	}
}

Vermilion::Core::Vulkan::Binding::~Binding(){
}

#endif
