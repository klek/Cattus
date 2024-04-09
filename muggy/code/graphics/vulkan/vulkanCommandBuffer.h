//********************************************************************
//  File:    vulkanCommandBuffer.h
//  Date:    Mon, 18 Mar 2024: 00:57
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_COMMAND_BUFFER_H)
#define VULKAN_COMMAND_BUFFER_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan
{
    vulkan_cmd_buffer allocateCmdBuffer( VkDevice device, 
                                         VkCommandPool cmdPool,
                                         bool primary );
    void freeCmdBuffer( VkDevice device, 
                        VkCommandPool cmdPool,
                        vulkan_cmd_buffer& cmdBuffer );
    void beginCmdBuffer( vulkan_cmd_buffer& cmdBuffer,
                         bool singleUse,
                         bool renderpassContinue,
                         bool simultaneousUse );
    bool endCmdBuffer( vulkan_cmd_buffer& cmdBuffer );
    void updateCmdBufferSubmitted( vulkan_cmd_buffer& cmdBuffer );
    void resetCmdBuffer( vulkan_cmd_buffer& cmdBuffer );

    // vulkan_cmd_buffer allocateCmdBufferBeginSingleUse( VkDevice device,
    //                                                    VkCommandPool cmdPool );
    // void endCmdBufferSingleUse( VkDevice device,
    //                             VkCommandPool cmdPool,
    //                             vulkan_cmd_buffer& cmdBuffer,
    //                             VkQueue queue );

} // namespace muggy::graphics::vulkan


#endif