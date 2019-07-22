#include <iostream>
#include <vector>
#include "errors.h"
#include "vk_boilerplate.h"


VkInstance Instance = VK_NULL_HANDLE;


VkInstance GetInstance()
{
	return Instance;
}


StatusCode SetupInstance()
{
	// Extensions Required by GLFW
	std::vector<const char*> RequiredExtensions;
	uint32_t GLFWExtensionCount = 0;
	{
		const char** GLFWExtensions;
		GLFWExtensions = glfwGetRequiredInstanceExtensions(&GLFWExtensionCount);
		for (uint32_t i = 0; i < GLFWExtensionCount; ++i)
		{
			RequiredExtensions.push_back(GLFWExtensions[i]);
		}
	}
	std::cout << "This application uses the following Vulkan instance extensions:\n";
	for (int i = 0; i < RequiredExtensions.size(); ++i)
	{
		std::cout << " - " << RequiredExtensions[i] << "\n";
	}

	// Create the Vulkan Instance
	VkApplicationInfo AppInfo = {};
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pApplicationName = "Hail Eris";
	AppInfo.applicationVersion = 0;
	AppInfo.pEngineName = nullptr;
	AppInfo.engineVersion = 0;
	AppInfo.apiVersion = VK_API_VERSION_1_1;
	VkInstanceCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	CreateInfo.pApplicationInfo = &AppInfo;
	CreateInfo.enabledExtensionCount = (uint32_t)RequiredExtensions.size();
	CreateInfo.ppEnabledExtensionNames = RequiredExtensions.data();
#if DEBUG_BUILD
	const std::vector<const char*> ValidationLayers = {	
		"VK_LAYER_KHRONOS_validation",

		//"VK_LAYER_GOOGLE_threading",
		//"VK_LAYER_GOOGLE_unique_objects",
		//"VK_LAYER_LUNARG_api_dump",
		"VK_LAYER_LUNARG_assistant_layer",
		"VK_LAYER_LUNARG_core_validation",
		//"VK_LAYER_LUNARG_device_simulation",
		//"VK_LAYER_LUNARG_monitor",
		"VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_parameter_validation",
		//"VK_LAYER_LUNARG_screenshot",
		//"VK_LAYER_LUNARG_standard_validation",
		//"VK_LAYER_LUNARG_vktrace",
		//"VK_LAYER_NV_nomad_release_public_2019_3_1",
		//"VK_LAYER_NV_optimus",
		//"VK_LAYER_RENDERDOC_Capture",
		//"VK_LAYER_VALVE_steam_fossilize",
		//"VK_LAYER_VALVE_steam_overlay",
	};
	CreateInfo.enabledLayerCount = (uint32_t)ValidationLayers.size();
	CreateInfo.ppEnabledLayerNames = ValidationLayers.data();
#else
	CreateInfo.enabledLayerCount = 0;
#endif
	VkResult CreationResult = vkCreateInstance(&CreateInfo, nullptr, &Instance);
	if (CreationResult == VK_ERROR_EXTENSION_NOT_PRESENT)
	{
		std::cout << "A required instance extension is missing.\n";
		return StatusCode::FAIL;
	}
	if (CreationResult != VK_SUCCESS)
	{
		std::cout << "Failed to create the Vulkan instance.\n";
		return StatusCode::FAIL;
	}

	return StatusCode::PASS;
}
