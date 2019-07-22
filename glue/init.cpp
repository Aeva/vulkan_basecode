#include <iostream>
#include "errors.h"
#include "vk_boilerplate.h"
#include "../demo.h"


GLFWwindow* Window;
VkSurfaceKHR Surface;


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


#define QUIT_ON_FAIL(Expr) if (Expr == StatusCode::FAIL) return 1;


int main() {
	QUIT_ON_FAIL(SetupGLFW());
	QUIT_ON_FAIL(SetupInstance());
	QUIT_ON_FAIL(SetupPhysicalDevice());
	glfwCreateWindowSurface(GetInstance(), Window, nullptr, &Surface);
	QUIT_ON_FAIL(SetupDevice(Surface));
	QUIT_ON_FAIL(SetupSwapChain(Surface));

#if RENDERDOC_CAPTURE_AND_QUIT
	if (rdoc_api != nullptr)
	{
		rdoc_api->StartFrameCapture(NULL, NULL);
#endif
		QUIT_ON_FAIL(RenderingExperiment::Setup());

#if !RENDERDOC_CAPTURE_AND_QUIT
		std::cout << "Starting main loop...\n";
		while (!glfwWindowShouldClose(Window) && !GetHaltAndCatchFire())
#endif
		{
			RenderingExperiment::Render();
			Present();
			glfwPollEvents();
		}
#if RENDERDOC_CAPTURE_AND_QUIT
		rdoc_api->EndFrameCapture(NULL, NULL);
	}
#endif

	RenderingExperiment::Teardown();
	TeardownSwapChain();
	vkDestroySwapchainKHR(GetDevice(), GetSwapChain(), nullptr);
	vkDestroyDevice(GetDevice(), nullptr);
	vkDestroySurfaceKHR(GetInstance(), Surface, nullptr);
	vkDestroyInstance(GetInstance(), nullptr);
	glfwDestroyWindow(Window);
	glfwTerminate();
	return 0;
}
