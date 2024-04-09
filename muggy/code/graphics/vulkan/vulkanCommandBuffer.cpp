//********************************************************************
//  File:    vulkanCommandBuffer.cpp
//  Date:    Mon, 18 Mar 2024: 01:04
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanCommandBuffer.h"

namespace muggy::graphics::vulkan
{
    vulkan_cmd_buffer allocateCmdBuffer( VkDevice device, 
                                         VkCommandPool cmdPool,
                                         bool primary )
    {
        vulkan_cmd_buffer cmdBuffer{ };

        VkCommandBufferAllocateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.commandPool = cmdPool;
        // NOTE(klek): Secondary buffers must be used together with another
        //             buffer (primary), not on its own
        // TODO(klek): Can we check for this?
        info.level = ( primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY );
        info.commandBufferCount = 1;
        info.pNext = nullptr;

        cmdBuffer.cmdState = vulkan_cmd_buffer::CMD_NOT_ALLOCATED;
        VkResult result { VK_SUCCESS };
        result = vkAllocateCommandBuffers( device, &info, &cmdBuffer.cmdBuffer );
        if ( result != VK_SUCCESS )
        {
            MSG("Failed to allocate command buffer...")
            // NOTE(klek): Should we free cmdBuffer.cmdBuffer here?
            //             To make sure it is nullptr?
            return cmdBuffer;
        }

        cmdBuffer.cmdState = vulkan_cmd_buffer::CMD_READY;

        MSG("Command buffer allocated!");

        return cmdBuffer;
    }

    void freeCmdBuffer( VkDevice device, 
                        VkCommandPool cmdPool,
                        vulkan_cmd_buffer& cmdBuffer )
    {
        vkFreeCommandBuffers( device, cmdPool, 1, &cmdBuffer.cmdBuffer );

        cmdBuffer.cmdBuffer = nullptr;
        cmdBuffer.cmdState = vulkan_cmd_buffer::CMD_NOT_ALLOCATED;
    }

    // 
    void beginCmdBuffer( vulkan_cmd_buffer& cmdBuffer,
                         bool singleUse,
                         bool renderpassContinue,
                         bool simultaneousUse )
    {
        VkCommandBufferBeginInfo info { };
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags = 0;

        // NOTE(klek): Single use cannot be used more than once, and
        //             will be reset and recorded again between each
        //             submission
        if ( singleUse )
        {
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        }
        // NOTE(klek): RenderpassContinue indicates it is a secondary 
        //             buffer entirely inside a render pass. It is 
        //             ignored for primary buffers
        if ( renderpassContinue )
        {
            info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        }
        // NOTE(klek): Simultaneous use can be resubmitted to a queue 
        //             while it is in the pending state and recorded
        //             into multiple primary command buffers
        if ( simultaneousUse )
        {
            info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        }

        VkResult result { VK_SUCCESS };
        result = vkBeginCommandBuffer( cmdBuffer.cmdBuffer, &info );
        if ( result != VK_SUCCESS )
        {
            MSG("Failed to begin command buffer...");
            assert( result != VK_SUCCESS );
        }
        cmdBuffer.cmdState = vulkan_cmd_buffer::CMD_RECORDING;
    }

    // Function to end the command buffer
    // Sets the internal recording state to ended
    bool endCmdBuffer( vulkan_cmd_buffer& cmdBuffer )
    {
        VkResult result { VK_SUCCESS };
        // TODO(klek): Check to make sure the buffer can be ended before ending
        result = vkEndCommandBuffer( cmdBuffer.cmdBuffer );
        if ( result != VK_SUCCESS )
        {
            MSG("Failed to end command buffer...");
            return false;
        }

        // Set state to ended
        cmdBuffer.cmdState = vulkan_cmd_buffer::CMD_RECORDING_ENDED;

        return true;
    }

    // Function to set the internal state to submitted
    void updateCmdBufferSubmitted( vulkan_cmd_buffer& cmdBuffer )
    {
        cmdBuffer.cmdState = vulkan_cmd_buffer::CMD_SUBMITTED;
    }

    // Function to set the internal state to ready
    void resetCmdBuffer( vulkan_cmd_buffer& cmdBuffer )
    {
        cmdBuffer.cmdState = vulkan_cmd_buffer::CMD_READY;
    }

    // Function to allocate a single use, primary command buffer
    vulkan_cmd_buffer allocateCmdBufferBeginSingleUse( VkDevice device,
                                                       VkCommandPool cmdPool )
    {
        vulkan_cmd_buffer cmdBuffer { };
        cmdBuffer = allocateCmdBuffer( device, cmdPool, true );
        beginCmdBuffer( cmdBuffer, true, false, false );

        return cmdBuffer;
    }

    // Function to end and submit a single use command buffer
    // NOTE(klek): This does not use a fence...
    void endCmdBufferSingleUse( VkDevice device,
                                VkCommandPool cmdPool,
                                vulkan_cmd_buffer& cmdBuffer,
                                VkQueue queue )
    {
        endCmdBuffer( cmdBuffer );

        VkSubmitInfo info { };
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &cmdBuffer.cmdBuffer;

        // Submit the command buffer to the given queue
        VkResult result { VK_SUCCESS };
        result = vkQueueSubmit( queue, 1, &info, nullptr );
        if ( result != VK_SUCCESS )
        {
            MSG("Failed to submit single use command buffer to queue...");
            assert( result != VK_SUCCESS );
        }

        // Wait for the queue to finish
        result = vkQueueWaitIdle( queue );
        if ( result != VK_SUCCESS )
        {
            MSG("vkQueueWaitIdle failed in endCmdBufferSignleUse...")
            assert( result != VK_SUCCESS );
        }

        // Free the single use command buffer
        freeCmdBuffer( device, cmdPool, cmdBuffer );
    }


} // namespace muggy::graphics::vulkan
