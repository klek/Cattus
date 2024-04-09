//********************************************************************
//  File:    vulkanGraphicsPipeline.h
//  Date:    Thu, 28 Mar 2024: 17:13
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_GRAPHICS_PIPELINE_H)
#define VULKAN_GRAPHICS_PIPELINE_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan::pipeline
{
    bool createGraphicsPipeline( VkDevice device, 
                                 VkRenderPass renderPass );
    VkPipeline& getGraphicsPipeline( void );
    bool initialize( void );
    void shutdown( VkDevice device );
} // namespace muggy::graphics::vulkan::pipeline



#endif