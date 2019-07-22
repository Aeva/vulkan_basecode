#include <iostream>
#include <vector>
#include "errors.h"
#include "vk_boilerplate.h"


VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;


VkPhysicalDevice GetPhysicalDevice()
{
	return PhysicalDevice;
}


StatusCode SetupPhysicalDevice()
{
	VkInstance Instance = GetInstance();

	// Now to find the GPU!
	uint32_t DeviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);
	if (DeviceCount == 0)
	{
		std::cout << "No compatible Vulkan devices found.\n";
		return StatusCode::FAIL;
	}
	{
		std::vector<VkPhysicalDevice> Devices(DeviceCount);
		vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices.data());
		for (int i = 0; i < Devices.size(); ++i)
		{
			VkPhysicalDeviceRayTracingPropertiesNV RayTracingProperties;
			RayTracingProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PROPERTIES_NV;
			RayTracingProperties.pNext = nullptr;
			RayTracingProperties.maxInstanceCount = 0;
			VkPhysicalDeviceProperties2 DeviceProperties2;
			DeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			DeviceProperties2.pNext = &RayTracingProperties;
			DeviceProperties2.properties = {};

			vkGetPhysicalDeviceProperties2(Devices[i], &DeviceProperties2);

			if (DeviceProperties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
				RayTracingProperties.maxInstanceCount > 0)
			{
				std::cout << "Using GPU: " << DeviceProperties2.properties.deviceName << "\n";
				PhysicalDevice = Devices[i];
				break;
			}
		}
	}
	if (PhysicalDevice == VK_NULL_HANDLE)
	{
		std::cout << "No adequate Vulkan physical devices found.\n";
		return StatusCode::FAIL;
	}
	// I found the GPU!
	return StatusCode::PASS;
}
