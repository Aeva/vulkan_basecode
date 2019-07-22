#include "vk_boilerplate.h"


VkPipelineRasterizationStateCreateInfo BasicRasterizationStateCreateInfo()
{
	VkPipelineRasterizationStateCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	CreateInfo.pNext = nullptr;
	CreateInfo.flags = 0;
	CreateInfo.depthClampEnable = VK_FALSE;
	CreateInfo.rasterizerDiscardEnable = VK_TRUE;
	CreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	CreateInfo.cullMode = VK_CULL_MODE_NONE;
	CreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	CreateInfo.depthBiasEnable = VK_FALSE;
	CreateInfo.depthBiasConstantFactor = 0.0;
	CreateInfo.depthBiasClamp = 0.0;
	CreateInfo.depthBiasSlopeFactor = 0.0;
	CreateInfo.lineWidth = 1.0;
	CreateInfo.depthClampEnable = VK_FALSE;
	return CreateInfo;
};


VkPipelineMultisampleStateCreateInfo BasicMultisampleStateCreateInfo()
{
	VkPipelineMultisampleStateCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	CreateInfo.flags = 0;
	CreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	CreateInfo.sampleShadingEnable = VK_FALSE;
	CreateInfo.minSampleShading = 1.0;
	CreateInfo.pSampleMask = nullptr;
	CreateInfo.alphaToCoverageEnable = VK_FALSE;
	CreateInfo.alphaToOneEnable = VK_FALSE;
	return CreateInfo;
}


VkPipelineDepthStencilStateCreateInfo BasicDepthStencilStateCreateInfo()
{
	VkPipelineDepthStencilStateCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	CreateInfo.pNext = nullptr;
	CreateInfo.flags = 0;
	CreateInfo.depthTestEnable = VK_FALSE;
	CreateInfo.depthWriteEnable = VK_FALSE;
	CreateInfo.depthCompareOp = VK_COMPARE_OP_NEVER;
	CreateInfo.depthBoundsTestEnable = VK_FALSE;
	CreateInfo.stencilTestEnable = VK_FALSE;
	CreateInfo.front = {};
	CreateInfo.back = {};
	CreateInfo.minDepthBounds = 0.0;
	CreateInfo.maxDepthBounds = 0.0;
	return CreateInfo;
}


VkPipelineColorBlendAttachmentState BasicColorBlendAttachmentState()
{
	VkPipelineColorBlendAttachmentState State;
	State.blendEnable = VK_FALSE;
	State.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	State.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	State.colorBlendOp = VK_BLEND_OP_ADD;
	State.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	State.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	State.alphaBlendOp = VK_BLEND_OP_ADD;
	State.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	return State;
}


VkPipelineColorBlendStateCreateInfo BasicColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState* Attachments)
{
	VkPipelineColorBlendStateCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	CreateInfo.pNext = nullptr;
	CreateInfo.flags = 0;
	CreateInfo.logicOpEnable = VK_FALSE;
	CreateInfo.logicOp = VK_LOGIC_OP_COPY;
	CreateInfo.attachmentCount = 1;
	CreateInfo.pAttachments = Attachments;
	CreateInfo.blendConstants[0] = 0.0;
	CreateInfo.blendConstants[1] = 0.0;
	CreateInfo.blendConstants[2] = 0.0;
	CreateInfo.blendConstants[3] = 0.0;
	return CreateInfo;
}


VkPipelineLayoutCreateInfo BasicLayoutCreateInfo()
{
	VkPipelineLayoutCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	CreateInfo.pNext = nullptr;
	CreateInfo.flags = 0;
	CreateInfo.setLayoutCount = 0;
	CreateInfo.pSetLayouts = nullptr;
	CreateInfo.pushConstantRangeCount = 0;
	CreateInfo.pPushConstantRanges = nullptr;
	return CreateInfo;
}


VkAttachmentReference BasicAttachmentRef()
{
	VkAttachmentReference Ref = {};
	Ref.attachment = 0;
	Ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	return Ref;
}


VkSubpassDescription BasicSubpassDesc(VkAttachmentReference* AttachmentRef)
{
	VkSubpassDescription Desc;
	Desc.flags = 0;
	Desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // VK_PIPELINE_BIND_POINT_RAY_TRACING_NV !
	Desc.inputAttachmentCount = 0;
	Desc.pInputAttachments = nullptr;
	Desc.colorAttachmentCount = 1;
	Desc.pColorAttachments = AttachmentRef;
	Desc.pResolveAttachments = nullptr;
	Desc.pDepthStencilAttachment = nullptr;
	Desc.preserveAttachmentCount = 0;
	Desc.pPreserveAttachments = nullptr;
	return Desc;
}
