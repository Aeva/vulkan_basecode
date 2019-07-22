#include <iostream>
#include <vector>
#include <limits>
#include "errors.h"
#include "vk_boilerplate.h"


const VkSurfaceFormatKHR SurfaceFormatSDR = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
const VkSurfaceFormatKHR SurfaceFormatHDR = { VK_FORMAT_R16G16B16A16_SFLOAT, VK_COLOR_SPACE_HDR10_ST2084_EXT };


VkSwapchainKHR SwapChain;
VkFormat SwapChainFormat;
VkExtent2D SwapChainExtent;
std::vector<VkImage> SwapChainImages;
std::vector<VkImageView> SwapChainImageViews;
uint32_t BackBufferIndex = 0;
VkSemaphore BackBufferReady;


VkSwapchainKHR GetSwapChain()
{
	return SwapChain;
}


VkViewport BasicViewport()
{
	VkViewport Viewport = {};
	Viewport.x = 0.0f;
	Viewport.y = 0.0f;
	Viewport.width = (float)SwapChainExtent.width;
	Viewport.height = (float)SwapChainExtent.height;
	Viewport.minDepth = 0.0f;
	Viewport.maxDepth = 1.0f;
	return Viewport;
}


VkRect2D BasicScissor()
{
	VkRect2D Scissor = {};
	Scissor.offset = { 0, 0 };
	Scissor.extent = SwapChainExtent;
	return Scissor;
}


VkPipelineViewportStateCreateInfo BasicPipelineViewportState(VkViewport* Viewport, VkRect2D* Scissor)
{
	VkPipelineViewportStateCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	CreateInfo.pNext = nullptr;
	CreateInfo.flags = 0;
	CreateInfo.viewportCount = 1;
	CreateInfo.pViewports = Viewport;
	CreateInfo.scissorCount = 1;
	CreateInfo.pScissors = Scissor;
	return CreateInfo;
}


VkAttachmentDescription BasicSwapChainAttacmentDesc()
{
	VkAttachmentDescription Attachment;
	Attachment.format = SwapChainFormat;
	Attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	Attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	Attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	Attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	Attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	Attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	Attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	return Attachment;
}


inline StatusCode ValidateSurfaceFormat(VkSurfaceKHR Surface, VkSurfaceFormatKHR SurfaceFormat)
{
	VkPhysicalDevice PhysicalDevice = GetPhysicalDevice();
	std::vector<VkSurfaceFormatKHR> AvailableSurfaceFormats;
	{
		uint32_t Count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &Count, nullptr);
		AvailableSurfaceFormats.resize(Count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &Count, AvailableSurfaceFormats.data());
	}
	for (int i = 0; i < AvailableSurfaceFormats.size(); ++i)
	{
		if (AvailableSurfaceFormats[i].format == SurfaceFormat.format && AvailableSurfaceFormats[i].colorSpace == SurfaceFormat.colorSpace)
		{
			return StatusCode::PASS;
		}
	}
	std::cout << "Requested surface format not available.\n";
	return StatusCode::FAIL;
}


void AdvanceBackbuffer()
{
	//vkAcquireNextImageKHR(GetDevice(), SwapChain, std::numeric_limits<uint64_t>::max(), BackBufferReady, VK_NULL_HANDLE, &BackBufferIndex);
}


StatusCode SetupSwapChain(VkSurfaceKHR Surface, const bool bOutputHDR)
{
	VkSurfaceFormatKHR SurfaceFormat = bOutputHDR ? SurfaceFormatHDR : SurfaceFormatSDR;
	RETURN_ON_FAIL(ValidateSurfaceFormat(Surface, SurfaceFormat));

	// NOTE: VK_PRESENT_MODE_FIFO_KHR is always available.  Other modes would need to be
	// validated against the available modes exposed by vkGetPhysicalDeviceSurfacePresentModesKHR.
	VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;

	VkSurfaceCapabilitiesKHR SurfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GetPhysicalDevice(), Surface, &SurfaceCapabilities);

	VkSwapchainCreateInfoKHR SwapChainCreateInfo;
	SwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapChainCreateInfo.pNext = nullptr;
	SwapChainCreateInfo.flags = 0;
	SwapChainCreateInfo.surface = Surface;
	SwapChainCreateInfo.minImageCount = 2;
	SwapChainCreateInfo.imageFormat = SurfaceFormat.format;
	SwapChainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
	SwapChainCreateInfo.imageExtent = SurfaceCapabilities.currentExtent;
	SwapChainCreateInfo.imageArrayLayers = 1;
	SwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	SwapChainCreateInfo.queueFamilyIndexCount = 0;
	SwapChainCreateInfo.pQueueFamilyIndices = nullptr;
	SwapChainCreateInfo.preTransform = SurfaceCapabilities.currentTransform;
	SwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapChainCreateInfo.presentMode = PresentMode;
	SwapChainCreateInfo.clipped = VK_TRUE;
	SwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	SwapChainFormat = SwapChainCreateInfo.imageFormat;
	SwapChainExtent = SwapChainCreateInfo.imageExtent;

	VkResult CreationResult = vkCreateSwapchainKHR(GetDevice(), &SwapChainCreateInfo, nullptr, &SwapChain);
	if (CreationResult != VK_SUCCESS)
	{
		std::cout << "Swap chain creation failed.\n";
		return StatusCode::FAIL;
	}

	// Grab the swap chain images and create the image views.
	{
		uint32_t ImageCount;
		vkGetSwapchainImagesKHR(GetDevice(), SwapChain, &ImageCount, nullptr);
		SwapChainImages.resize(ImageCount);
		SwapChainImageViews.resize(ImageCount);
		vkGetSwapchainImagesKHR(GetDevice(), SwapChain, &ImageCount, SwapChainImages.data());

		for (uint32_t i = 0; i < ImageCount; ++i)
		{
			VkImageViewCreateInfo ViewCreateInfo = {};
			ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			ViewCreateInfo.pNext = nullptr;
			ViewCreateInfo.flags = 0;
			ViewCreateInfo.image = SwapChainImages[i];
			ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			ViewCreateInfo.format = SwapChainCreateInfo.imageFormat;
			ViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			ViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			ViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			ViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			ViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			ViewCreateInfo.subresourceRange.baseMipLevel = 0;
			ViewCreateInfo.subresourceRange.levelCount = 1;
			ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			ViewCreateInfo.subresourceRange.layerCount = 1;

			VkResult CreationResult = vkCreateImageView(GetDevice(), &ViewCreateInfo, nullptr, &SwapChainImageViews[i]);
			if (CreationResult != VK_SUCCESS)
			{
				std::cout << "Could not create swap chain image views.\n";
				return StatusCode::FAIL;
			}
		}
	}

	// Create some semaphores for synchronizing swap chain image access.
	{
		VkSemaphoreCreateInfo CreateInfo = {};
		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkCreateSemaphore(GetDevice(), &CreateInfo, nullptr, &BackBufferReady);
	}

	AdvanceBackbuffer();

	return StatusCode::PASS;
}


void TeardownSwapChain()
{
	for (int i = 0; i < SwapChainImageViews.size(); ++i)
	{
		vkDestroyImageView(GetDevice(), SwapChainImageViews[i], nullptr);
	}
	vkDestroySemaphore(GetDevice(), BackBufferReady, nullptr);
}


void Present()
{
	/*
	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.waitSemaphoreCount = 0;
	PresentInfo.pWaitSemaphores = nullptr;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &SwapChain;
	PresentInfo.pImageIndices = &BackBufferIndex;
	PresentInfo.pResults = nullptr;

	VkResult PresentResult = vkQueuePresentKHR(GetPresentQueue(), &PresentInfo);
	if (PresentResult == VK_SUCCESS)
	{
		++BackBufferIndex;
		BackBufferIndex %= SwapChainImages.size();
	}
	else
	{
		std::cout << "Present failed!!!\n";
		SetHaltAndCatchFire();
	}
	*/
	AdvanceBackbuffer();
}
