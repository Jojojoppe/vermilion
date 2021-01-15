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
	VK_FORMAT_R8_UINT
};

Vermilion::Core::Vulkan::Pipeline::Pipeline(Vermilion::Core::Vulkan::API * api, std::shared_ptr<Vermilion::Core::RenderTarget> renderTarget, 
		std::shared_ptr<Vermilion::Core::ShaderProgram> shaderProgram, std::initializer_list<Vermilion::Core::BufferLayoutElement> vertexLayout,
		std::initializer_list<std::shared_ptr<Vermilion::Core::UniformBuffer>> uniformBuffers, std::initializer_list<std::shared_ptr<Vermilion::Core::Sampler>> samplers){
	this->api = api;
	this->instance = api->instance;

	this->renderTarget = renderTarget;
	this->shaderProgram = shaderProgram;
	this->vertexLayout = vertexLayout;
	this->uniformBuffers = uniformBuffers;
	this->samplers = samplers;

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	unsigned int binding=0;

	// Uniform buffer layout
	for(auto& ubuffer: uniformBuffers){
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = binding++;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
		layoutBindings.push_back(uboLayoutBinding);
	};

	// Samplers layout
	for(auto& sampler : samplers){
		VkDescriptorSetLayoutBinding texLayoutBinding = {};
		texLayoutBinding.binding = binding++;
		texLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		texLayoutBinding.descriptorCount = 1;
		texLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		texLayoutBinding.pImmutableSamplers = nullptr; // Optional
		layoutBindings.push_back(texLayoutBinding);
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = binding;
	layoutInfo.pBindings = layoutBindings.data();
	if(vkCreateDescriptorSetLayout(api->vk_device->vk_device, &layoutInfo, nullptr, &vk_descriptorSetLayout)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create descriptor set layout");
		throw std::runtime_error("Vermilion::Core::Vulkan::Pipeline::Pipeline() - Could not create descriptor set layout");
	}
	std::vector<VkDescriptorSetLayout> layouts(api->vk_swapchain->swapChainImages.size(), vk_descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = api->vk_descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(api->vk_swapchain->swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();
	vk_descriptorSets.resize(api->vk_swapchain->swapChainImages.size());
	if(vkAllocateDescriptorSets(api->vk_device->vk_device, &allocInfo, vk_descriptorSets.data())!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not allocate descriptor sets");
		throw std::runtime_error("Vermilion::Core::Vulkan::Pipeline::Pipeline() - Could not allocate descriptor sets");
	}

	for(int i = 0; i<api->vk_swapchain->swapChainImages.size(); i++){
		int offset = 0;
		// Uniform buffer bindings
		for(auto& ubo : uniformBuffers){
			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = std::static_pointer_cast<Vermilion::Core::Vulkan::UniformBuffer>(ubo)->vk_buffer[i];
			bufferInfo.offset = 0;
			bufferInfo.range = ubo->size;
			
			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = vk_descriptorSets[i];
			descriptorWrite.dstBinding = offset++;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;
			vkUpdateDescriptorSets(api->vk_device->vk_device, 1, &descriptorWrite, 0, nullptr);
		}
		// Sampler bindings
		for(auto& sampler : samplers){
			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = std::static_pointer_cast<Vermilion::Core::Vulkan::Sampler>(sampler)->texture->vk_imageView->imageView;
			imageInfo.sampler = std::static_pointer_cast<Vermilion::Core::Vulkan::Sampler>(sampler)->vk_sampler;
			
			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = vk_descriptorSets[i];
			descriptorWrite.dstBinding = offset++;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = nullptr;
			descriptorWrite.pImageInfo = &imageInfo;
			descriptorWrite.pTexelBufferView = nullptr;
			vkUpdateDescriptorSets(api->vk_device->vk_device, 1, &descriptorWrite, 0, nullptr);
		}
	}

	this->create();
}

Vermilion::Core::Vulkan::Pipeline::~Pipeline(){
	vkDestroyDescriptorSetLayout(api->vk_device->vk_device, vk_descriptorSetLayout, nullptr);
	this->destroy();
}

void Vermilion::Core::Vulkan::Pipeline::create(){
	std::static_pointer_cast<Vermilion::Core::Vulkan::ShaderProgram>(shaderProgram)->createModules();
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfo;
	for(const auto& shader : shaderProgram->shaders){
		shaderStageInfo.push_back(std::static_pointer_cast<Vermilion::Core::Vulkan::Shader>(shader)->vk_shaderStageInfo);
	}
	VkPipelineShaderStageCreateInfo * shaderStages = shaderStageInfo.data();

	// Vertex data layout
	// Calculate stride and offset
	std::vector<Vermilion::Core::BufferLayoutElement> elements(vertexLayout);
	unsigned int offset = 0;
	unsigned int stride = 0;
	for(auto& element : elements){
		element.offset = offset;
		offset += element.size;
		stride += element.size;
	}
	// Create descriptors
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = stride;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	int i = 0;
	for(auto& element : elements){
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
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Viewports and scissors
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
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

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
	colorBlendAttachment.blendEnable = VK_FALSE;
	    // alpha blending
	//colorBlendAttachment.blendEnable = VK_TRUE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	//colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	//colorBlending.blendConstants[0] = 0.0f; // Optional
	//colorBlending.blendConstants[1] = 0.0f; // Optional
	//colorBlending.blendConstants[2] = 0.0f; // Optional
	//colorBlending.blendConstants[3] = 0.0f; // Optional

	// Dynamic states
	// std::vector<VkDynamicState> dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT};
	// VkPipelineDynamicStateCreateInfo dynamicCreateInfo = {};
	// dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	// dynamicCreateInfo.pDynamicStates = dynamicStateEnables.data();
	// dynamicCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());

	// Pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &vk_descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
	if(vkCreatePipelineLayout(api->vk_device->vk_device, &pipelineLayoutInfo, nullptr, &this->vk_pipelineLayout)!=VK_SUCCESS){
		this->instance->logger.log(VMCORE_LOGLEVEL_FATAL, "Could not create pipeline layout");
		throw std::runtime_error("Vermilion::Core::Vulkan::Pipeline::Pipeline() - Could not create pipeline layout");
	}

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
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.layout = vk_pipelineLayout;
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
	vkDestroyPipelineLayout(api->vk_device->vk_device, vk_pipelineLayout, nullptr);
}

#endif
