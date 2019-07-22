#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "errors.h"

#define RETURN_ON_VK_FAIL(Expr) if ((VkResult)Expr != VK_SUCCESS) return StatusCode::FAIL;


const int ScreenWidth = 1024;
const int ScreenHeight = 768;


StatusCode SetupInstance();
VkInstance GetInstance();

StatusCode SetupPhysicalDevice();
VkPhysicalDevice GetPhysicalDevice();

StatusCode SetupDevice(VkSurfaceKHR Surface);
VkDevice GetDevice();
VkQueue GetGraphicsQueue();
VkQueue GetPresentQueue();

StatusCode SetupSwapChain(VkSurfaceKHR Surface, const bool bOutputHDR = false);
VkSwapchainKHR GetSwapChain();
VkViewport BasicViewport();
VkRect2D BasicScissor();
VkPipelineViewportStateCreateInfo BasicPipelineViewportState(VkViewport* Viewport, VkRect2D* Scissor);
VkAttachmentDescription BasicSwapChainAttacmentDesc();
void TeardownSwapChain();
void Present();

StatusCode CreateShaderModule(const char* FileName, VkShaderModule& ShaderModule);
VkPipelineShaderStageCreateInfo BasicShaderStageCreateInfo(VkShaderModule ShaderModule, VkShaderStageFlagBits Stage);

VkPipelineRasterizationStateCreateInfo BasicRasterizationStateCreateInfo();
VkPipelineMultisampleStateCreateInfo BasicMultisampleStateCreateInfo();
VkPipelineDepthStencilStateCreateInfo BasicDepthStencilStateCreateInfo();
VkPipelineColorBlendAttachmentState BasicColorBlendAttachmentState();
VkPipelineColorBlendStateCreateInfo BasicColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState* Attachments);
VkPipelineLayoutCreateInfo BasicLayoutCreateInfo();
VkAttachmentReference BasicAttachmentRef();
VkSubpassDescription BasicSubpassDesc(VkAttachmentReference* AttachmentRef);
