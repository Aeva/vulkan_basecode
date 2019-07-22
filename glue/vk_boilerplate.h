#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "errors.h"

#define RETURN_ON_VK_FAIL(Expr) if ((VkResult)Expr != VK_SUCCESS) return StatusCode::FAIL;


const int ScreenWidth = 1024;
const int ScreenHeight = 768;


extern StatusCode SetupInstance();
extern VkInstance GetInstance();

extern StatusCode SetupPhysicalDevice();
extern VkPhysicalDevice GetPhysicalDevice();

extern StatusCode SetupDevice(VkSurfaceKHR Surface);
extern VkDevice GetDevice();
extern VkQueue GetGraphicsQueue();
extern VkQueue GetPresentQueue();

extern StatusCode SetupSwapChain(VkSurfaceKHR Surface, const bool bOutputHDR = false);
VkSwapchainKHR GetSwapChain();
extern void TeardownSwapChain();
extern void Present();
