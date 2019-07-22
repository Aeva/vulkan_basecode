#include <string>
#include "demo.h"

using namespace RenderingExperiment;


const std::vector<const char*> RenderingExperiment::DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
	VK_NV_RAY_TRACING_EXTENSION_NAME
};


VkPipelineLayout PipelineLayout;
VkRenderPass RenderPass;
VkPipeline GraphicsPipeline;


StatusCode RenderingExperiment::Setup()
{
	VkShaderModule VertShaderModule;
	VkShaderModule FragShaderModule;
	RETURN_ON_FAIL(CreateShaderModule("blobs\\hello.vs.blob", VertShaderModule));
	RETURN_ON_FAIL(CreateShaderModule("blobs\\hello.fs.blob", FragShaderModule));
	VkPipelineShaderStageCreateInfo ShaderStages[2];
	ShaderStages[0] = BasicShaderStageCreateInfo(VertShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
	ShaderStages[1] = BasicShaderStageCreateInfo(FragShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineVertexInputStateCreateInfo VertexInputInfo = {};
	VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	VertexInputInfo.vertexBindingDescriptionCount = 0;
	VertexInputInfo.pVertexBindingDescriptions = nullptr;
	VertexInputInfo.vertexAttributeDescriptionCount = 0;
	VertexInputInfo.pVertexAttributeDescriptions = nullptr;

	VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo = {};
	InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport Viewport = BasicViewport();
	VkRect2D Scissor = BasicScissor();
	VkPipelineViewportStateCreateInfo ViewportStateCreateInfo = BasicPipelineViewportState(&Viewport, &Scissor);

	VkPipelineRasterizationStateCreateInfo RasterizerCreateInfo = BasicRasterizationStateCreateInfo();
	VkPipelineMultisampleStateCreateInfo MultisampleCreateInfo = BasicMultisampleStateCreateInfo();
	//VkPipelineDepthStencilStateCreateInfo DepthStencilCreateInfo = BasicDepthStencilStateCreateInfo();
	VkPipelineColorBlendAttachmentState BlendAttachmentState = BasicColorBlendAttachmentState();
	VkPipelineColorBlendStateCreateInfo BlendStateCreateInfo = BasicColorBlendStateCreateInfo(&BlendAttachmentState);

	VkAttachmentDescription AttachmentDesc = BasicSwapChainAttacmentDesc();
	VkAttachmentReference AttachmentRef = BasicAttachmentRef();
	VkSubpassDescription SubpassDesc = BasicSubpassDesc(&AttachmentRef);

	VkPipelineDynamicStateCreateInfo DynamicStateInfo = {};
	DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	DynamicStateInfo.pNext = nullptr;
	DynamicStateInfo.flags = 0;
	DynamicStateInfo.dynamicStateCount = 0;
	DynamicStateInfo.pDynamicStates = nullptr;

	VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = BasicLayoutCreateInfo();
	{
		VkResult CreationResult = vkCreatePipelineLayout(GetDevice(), &PipelineLayoutCreateInfo, nullptr, &PipelineLayout);
		if (CreationResult != VK_SUCCESS)
		{
			std::cout << "Failed to create pipeline layout.\n";
			return StatusCode::FAIL;
		}
	}

	VkRenderPassCreateInfo RenderPassCreateInfo = {};
	RenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassCreateInfo.pNext = nullptr;
	RenderPassCreateInfo.flags = 0;
	RenderPassCreateInfo.attachmentCount = 1;
	RenderPassCreateInfo.pAttachments = &AttachmentDesc;
	RenderPassCreateInfo.subpassCount = 1;
	RenderPassCreateInfo.pSubpasses = &SubpassDesc;
	RenderPassCreateInfo.dependencyCount = 0;
	RenderPassCreateInfo.pDependencies = nullptr;
	{
		VkResult CreationResult = vkCreateRenderPass(GetDevice(), &RenderPassCreateInfo, nullptr, &RenderPass);
		if (CreationResult != VK_SUCCESS)
		{
			std::cout << "Failed to create render pass.\n";
			return StatusCode::FAIL;
		}
	}

	VkGraphicsPipelineCreateInfo PipelineCreateInfo = {};
	PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineCreateInfo.pNext = nullptr;
	PipelineCreateInfo.flags = 0;
	PipelineCreateInfo.stageCount = 0;
	PipelineCreateInfo.pStages = ShaderStages;
	PipelineCreateInfo.pVertexInputState = &VertexInputInfo;
	PipelineCreateInfo.pInputAssemblyState = &InputAssemblyInfo;
	PipelineCreateInfo.pTessellationState = nullptr;
	PipelineCreateInfo.pViewportState = &ViewportStateCreateInfo;
	PipelineCreateInfo.pRasterizationState = &RasterizerCreateInfo;
	PipelineCreateInfo.pMultisampleState = &MultisampleCreateInfo;
	PipelineCreateInfo.pDepthStencilState = nullptr;
	PipelineCreateInfo.pColorBlendState = &BlendStateCreateInfo;
	PipelineCreateInfo.pDynamicState = &DynamicStateInfo;
	PipelineCreateInfo.layout = PipelineLayout;
	PipelineCreateInfo.renderPass = RenderPass;
	PipelineCreateInfo.subpass = 0;
	PipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	PipelineCreateInfo.basePipelineIndex = -1;
	{
		VkResult CreationResult = vkCreateGraphicsPipelines(GetDevice(), VK_NULL_HANDLE, 1, &PipelineCreateInfo, nullptr, &GraphicsPipeline);
		if (CreationResult != VK_SUCCESS)
		{
			std::cout << "Could not create the graphics pipeline.\n";
			return StatusCode::FAIL;
		}
	}



	vkDestroyShaderModule(GetDevice(), VertShaderModule, nullptr);
	vkDestroyShaderModule(GetDevice(), FragShaderModule, nullptr);
	return StatusCode::PASS;
}


void RenderingExperiment::Teardown()
{
	vkDestroyPipeline(GetDevice(), GraphicsPipeline, nullptr);
	vkDestroyRenderPass(GetDevice(), RenderPass, nullptr);
	vkDestroyPipelineLayout(GetDevice(), PipelineLayout, nullptr);
	
}


void RenderingExperiment::Render()
{
}
