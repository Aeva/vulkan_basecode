#include <iostream>
#include <vector>
#include "errors.h"
#include "vulkan_boilerplate.h"
#include "../demo.h"


GLFWwindow* Window;
namespace GPU
{
	VkInstance Instance;
	VkDevice Device;
	VkQueue GraphicsQueue;
}


void ErrorCallback(int Error, const char* Description)
{
	std::cout << "Error: " << Description << '\n';
	SetHaltAndCatchFire();
}


StatusCode SetupGLFW()
{
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit())
	{
		std::cout << "glfw init failed\n";
		return StatusCode::FAIL;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	Window = glfwCreateWindow(ScreenWidth, ScreenHeight, "meep", nullptr, nullptr);
	if (!Window)
	{
		std::cout << "failed to create glfw window\n";
		glfwTerminate();
		return StatusCode::FAIL;
	}
	//glfwMakeContextCurrent(Window);

	return StatusCode::PASS;
}


StatusCode SetupVulkan()
{
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
			"VK_LAYER_KHRONOS_validation"
		};
		CreateInfo.enabledLayerCount = (uint32_t)ValidationLayers.size();
		CreateInfo.ppEnabledLayerNames = ValidationLayers.data();
#else
		CreateInfo.enabledLayerCount = 0;
#endif
		VkResult InstanceCreated = vkCreateInstance(&CreateInfo, nullptr, &GPU::Instance);
		if (InstanceCreated == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			std::cout << "A required instance extension is missing.\n";
		}

		RETURN_ON_VK_FAIL(InstanceCreated);
	}

	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
	{
		// Now to find the GPU!
		uint32_t DeviceCount = 0;
		vkEnumeratePhysicalDevices(GPU::Instance, &DeviceCount, nullptr);
		if (DeviceCount == 0)
		{
			std::cout << "No compatible Vulkan devices found.\n";
			return StatusCode::FAIL;
		}
		{
			std::vector<VkPhysicalDevice> Devices(DeviceCount);
			vkEnumeratePhysicalDevices(GPU::Instance, &DeviceCount, Devices.data());
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
			std::cout << "No adequate Vulkan devices found.\n";
			return StatusCode::FAIL;
		}
		// I found the GPU!
	}

	{
		float QueuePriority = 1.0f;
		VkDeviceQueueCreateInfo QueueCreateInfo = {};
		QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueueCreateInfo.queueFamilyIndex = 0;
		QueueCreateInfo.queueCount = 1;
		QueueCreateInfo.pQueuePriorities = &QueuePriority;
		{
			uint32_t QueueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilies.data());
			bool bFoundGraphicsQueue = false;
			for (uint32_t i = 0; i < QueueFamilyCount; ++i)
			{
				if ((QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 1)
				{
					QueueCreateInfo.queueFamilyIndex = i;
					bFoundGraphicsQueue = true;
					break;
				}
			}
			if (!bFoundGraphicsQueue)
			{
				std::cout << "Can't find the graphics queue.....????\n";
				return StatusCode::FAIL;
			}
		}

		VkPhysicalDeviceFeatures DeviceFeatures = {};

		VkDeviceCreateInfo DeviceInfo = {};
		DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		DeviceInfo.queueCreateInfoCount = 1;
		DeviceInfo.pQueueCreateInfos = &QueueCreateInfo;
		DeviceInfo.enabledExtensionCount = (uint32_t)RenderingExperiment::DeviceExtensions.size();
		DeviceInfo.ppEnabledExtensionNames = RenderingExperiment::DeviceExtensions.data();
		DeviceInfo.pEnabledFeatures = &DeviceFeatures;

		std::cout << "This application uses the following Vulkan device extensions:\n";
		for (int i = 0; i < RenderingExperiment::DeviceExtensions.size(); ++i)
		{
			std::cout << " - " << RenderingExperiment::DeviceExtensions[i] << "\n";
		}

		VkResult DeviceCreated = vkCreateDevice(PhysicalDevice, &DeviceInfo, nullptr, &GPU::Device);
		if (DeviceCreated == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			std::cout << "A required device extension is missing.\n";
		}

		RETURN_ON_VK_FAIL(DeviceCreated);

		vkGetDeviceQueue(GPU::Device, QueueCreateInfo.queueFamilyIndex, 0, &GPU::GraphicsQueue);
	}

	return StatusCode::PASS;
}


StatusCode DemoSetup()
{
	//glUseProgram(0);
	RETURN_ON_FAIL(RenderingExperiment::Setup());
	return StatusCode::PASS;
}


void DrawFrame()
{
	RenderingExperiment::Render();

	//glfwSwapBuffers(Window);
	glfwPollEvents();
}


#define QUIT_ON_FAIL(Expr) if (Expr == StatusCode::FAIL) return 1;


int main() {
	QUIT_ON_FAIL(SetupGLFW());
	QUIT_ON_FAIL(SetupVulkan());

#if RENDERDOC_CAPTURE_AND_QUIT
	if (rdoc_api != nullptr)
	{
		rdoc_api->StartFrameCapture(NULL, NULL);
#endif
		QUIT_ON_FAIL(DemoSetup());

#if !RENDERDOC_CAPTURE_AND_QUIT
		std::cout << "Starting main loop...\n";
		while (!glfwWindowShouldClose(Window) && !GetHaltAndCatchFire())
#endif
		{
			DrawFrame();
		}
#if RENDERDOC_CAPTURE_AND_QUIT
		rdoc_api->EndFrameCapture(NULL, NULL);
	}
#endif

	vkDestroyDevice(GPU::Device, nullptr);
	vkDestroyInstance(GPU::Instance, nullptr);
	glfwDestroyWindow(Window);
	glfwTerminate();
	return 0;
}
