#pragma once
#include "glue/errors.h"
#include "glue/vk_boilerplate.h"
#include <vector>


namespace RenderingExperiment
{
	StatusCode Setup();
	void Render();

	extern const std::vector<const char*> DeviceExtensions;
}
