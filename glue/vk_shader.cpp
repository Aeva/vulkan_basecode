#include <iostream>
#include <fstream>
#include <vector>
#include <cstddef>
#include "errors.h"
#include "vk_boilerplate.h"


StatusCode CreateShaderModule(const char* FileName, VkShaderModule& ShaderModule) {
	std::ifstream FileStream(FileName, std::ios::ate | std::ios::binary);
	if (!FileStream.is_open())
	{
		std::cout << "Can't open file: " << FileName << "\n";
		return StatusCode::FAIL;
	}

	size_t FileSize = (size_t)FileStream.tellg();
	std::vector<std::byte> Blob(FileSize);
	FileStream.seekg(0);
	FileStream.read((char*)Blob.data(), FileSize);
	FileStream.close();

	VkShaderModuleCreateInfo CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	CreateInfo.pNext = nullptr;
	CreateInfo.flags = 0;
	CreateInfo.codeSize = FileSize;
	CreateInfo.pCode = (const uint32_t*)Blob.data();

	VkResult CreationResult = vkCreateShaderModule(GetDevice(), &CreateInfo, nullptr, &ShaderModule);
	if (CreationResult != VK_SUCCESS)
	{
		std::cout << "Unable to create shader module for: " << FileName << "\n";
		return StatusCode::FAIL;
	}
	return StatusCode::PASS;
}


VkPipelineShaderStageCreateInfo BasicShaderStageCreateInfo(VkShaderModule ShaderModule, VkShaderStageFlagBits Stage)
{
	VkPipelineShaderStageCreateInfo ShaderStageCreateInfo = {};
	ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	ShaderStageCreateInfo.pNext = nullptr;
	ShaderStageCreateInfo.flags = 0;
	ShaderStageCreateInfo.stage = Stage;
	ShaderStageCreateInfo.module = ShaderModule;
	ShaderStageCreateInfo.pName = "main";
	ShaderStageCreateInfo.pSpecializationInfo = nullptr;
	return ShaderStageCreateInfo;
}
