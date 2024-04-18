//********************************************************************
//  File:    vulkanRenderPass.h
//  Date:    Mon, 18 Mar 2024: 19:39
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_RENDER_PASS_H)
#define VULKAN_RENDER_PASS_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan::renderpass
{
    bool createRenderPass( VkDevice device,
                           VkFormat swapchainFormat,
                           VkFormat depthFormat,
                           math::u32v4d renderArea,
                           math::fv4d clearColor,
                           float depth,
                           uint32_t stencil,
                           vulkan_renderpass& renderPass );
    void destroyRenderPass( VkDevice device, vulkan_renderpass& renderPass );
    void beginRenderPass( VkCommandBuffer cmdBuffer, 
                          vulkan_cmd_buffer::state& state,
                          vulkan_renderpass& renderPass,
                          VkFramebuffer frameBuffer );
    void endRenderPass( VkCommandBuffer cmdBuffer, 
                        vulkan_cmd_buffer::state& state,
                        vulkan_renderpass& renderPass );
} // namespace muggy::graphics::vulkan::renderpass


#endif