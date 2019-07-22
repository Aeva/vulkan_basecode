#include "demo.h"

using namespace RenderingExperiment;


const std::vector<const char*> RenderingExperiment::DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
	VK_NV_RAY_TRACING_EXTENSION_NAME
};


StatusCode RenderingExperiment::Setup()
{
	return StatusCode::PASS;
}


void RenderingExperiment::Render()
{
}
