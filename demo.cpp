#include "demo.h"

using namespace RenderingExperiment;


const std::vector<const char*> RenderingExperiment::DeviceExtensions = {
	"VK_KHR_swapchain",
	"VK_NV_ray_tracing"
};


StatusCode RenderingExperiment::Setup()
{
	return StatusCode::PASS;
}


void RenderingExperiment::Render()
{
}
