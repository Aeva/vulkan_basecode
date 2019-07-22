#include <iostream>
#include <vector>
#include <string.h>
#include "errors.h"
#include "vk_boilerplate.h"
#include "../demo.h"


VkDevice Device = VK_NULL_HANDLE;
VkQueue GraphicsQueue = VK_NULL_HANDLE;
VkQueue PresentQueue = VK_NULL_HANDLE;
int GraphicsQueueindex = -1;
int PresentQueueindex = -1;


VkDevice GetDevice()
{
	return Device;
}


VkQueue GetGraphicsQueue()
{
	return GraphicsQueue;
}


VkQueue GetPresentQueue()
{
	return PresentQueue;
}


StatusCode ValidateExtensions()
{
	VkPhysicalDevice PhysicalDevice = GetPhysicalDevice();

	std::vector<VkExtensionProperties> AvailableExtensions;
	{
		uint32_t Count;
		vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &Count, nullptr);
		AvailableExtensions.resize(Count);
		vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &Count, AvailableExtensions.data());
	}

	std::cout << "This application requires the following Vulkan device extensions:\n";

	int MissingCount = 0;
	for (int i = 0; i < RenderingExperiment::DeviceExtensions.size(); ++i)
	{
		bool bFound = false;
		for (int a = 0; a < AvailableExtensions.size(); ++a)
		{
			if (strcmp(RenderingExperiment::DeviceExtensions[i], AvailableExtensions[a].extensionName) == 0)
			{
				bFound = true;
				break;
			}
		}
		if (bFound)
		{
			std::cout << "[ FOUND ] ";
		}
		else
		{
			std::cout << "[MISSING] ";
			++MissingCount;
		}
		std::cout << RenderingExperiment::DeviceExtensions[i] << "\n";
	}

	if (MissingCount > 0)
	{
		std::cout << "A required device extension is missing.\n";
		return StatusCode::FAIL;
	}

	return StatusCode::PASS;
}


void FindQueues(VkSurfaceKHR Surface)
{
	VkPhysicalDevice PhysicalDevice = GetPhysicalDevice();

	GraphicsQueueindex = -1;
	PresentQueueindex = -1;

	uint32_t QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilies.data());
	bool bFoundGraphicsQueue = false;
	bool bFoundPresentQueue = false;
	for (uint32_t i = 0; i < QueueFamilyCount && !bFoundGraphicsQueue && !bFoundPresentQueue; ++i)
	{
		if (!bFoundGraphicsQueue && (QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 1)
		{
			GraphicsQueueindex = i;
			bFoundGraphicsQueue = true;
		}
		VkBool32 bPresentSupported = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &bPresentSupported);
		if (!bFoundPresentQueue && bPresentSupported)
		{
			PresentQueueindex = i;
			bFoundPresentQueue = true;
		}
	}
}


StatusCode SetupDevice(VkSurfaceKHR Surface)
{
	RETURN_ON_FAIL(ValidateExtensions());

	FindQueues(Surface);
	if (GraphicsQueueindex == -1)
	{
		std::cout << "No graphics queue found!\n";
		return StatusCode::FAIL;
	}
	if (PresentQueueindex == -1)
	{
		std::cout << "No present queue found!\n";
		return StatusCode::FAIL;
	}

	const bool bCreateSeparatePresentQueue = GraphicsQueueindex != PresentQueueindex;
	const int QueueCount = bCreateSeparatePresentQueue ? 2 : 1;
	std::vector<VkDeviceQueueCreateInfo> QueueCreateInfo;
	QueueCreateInfo.resize(QueueCount);
	{
		const float QueuePriority = 1.0f;
		QueueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueueCreateInfo[0].queueFamilyIndex = 0;
		QueueCreateInfo[0].queueCount = 1;
		QueueCreateInfo[0].pQueuePriorities = &QueuePriority;
		QueueCreateInfo[0].queueFamilyIndex = GraphicsQueueindex;
	}
	if (bCreateSeparatePresentQueue)
	{
		const float QueuePriority = 0.9f;
		QueueCreateInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueueCreateInfo[1].queueFamilyIndex = 0;
		QueueCreateInfo[1].queueCount = 1;
		QueueCreateInfo[1].pQueuePriorities = &QueuePriority;
		QueueCreateInfo[1].queueFamilyIndex = PresentQueueindex;
	}

	VkPhysicalDeviceFeatures DeviceFeatures = {};

	VkDeviceCreateInfo DeviceInfo = {};
	DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceInfo.queueCreateInfoCount = QueueCount;
	DeviceInfo.pQueueCreateInfos = QueueCreateInfo.data();
	DeviceInfo.enabledExtensionCount = (uint32_t)RenderingExperiment::DeviceExtensions.size();
	DeviceInfo.ppEnabledExtensionNames = RenderingExperiment::DeviceExtensions.data();
	DeviceInfo.pEnabledFeatures = &DeviceFeatures;

	VkResult CreationResult = vkCreateDevice(GetPhysicalDevice(), &DeviceInfo, nullptr, &Device);
	if (CreationResult != VK_SUCCESS)
	{
		std::cout << "Failed to create the Vulkan logical device.\n";
		return StatusCode::FAIL;
	}

	vkGetDeviceQueue(Device, GraphicsQueueindex, 0, &GraphicsQueue);
	vkGetDeviceQueue(Device, PresentQueueindex, 0, &PresentQueue);

	return StatusCode::PASS;
}
