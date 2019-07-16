#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "errors.h"

#define RETURN_ON_VK_FAIL(Expr) if ((VkResult)Expr != VK_SUCCESS) return StatusCode::FAIL;


const int ScreenWidth = 1024;
const int ScreenHeight = 768;
