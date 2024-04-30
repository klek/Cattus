//********************************************************************
//  File:    vulkanVertex.cpp
//  Date:    Sat, 13 Apr 2024: 13:56
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanVertex.h"
#include "vulkanCore.h"
#include "vulkanCommandBuffer.h"

namespace muggy::graphics::vulkan::vertex
{  
    namespace 
    {

    } // anonymous namespace 

    void copyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, 
                     VkDeviceSize size,
                     VkCommandPool cmdPool,
                     VkQueue queue )
    {
        VkCommandBufferAllocateInfo allocInfo { };
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = cmdPool;
        allocInfo.commandBufferCount = 1;

        // NOTE(klek): We are using a single buffer use here to allocate
        //             and start it right away.
        vulkan_cmd_buffer cmdBuffer { };
        allocateCmdBufferBeginSingleUse( core::getLogicalDevice(), cmdPool, cmdBuffer );

        VkBufferCopy copyRegion{ };
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer( cmdBuffer.cmdBuffer, srcBuffer, dstBuffer, 1, &copyRegion );

        // NOTE(klek): endCmdBufferSingleUse should end the cmdbuffer,
        //             submit it, wait for the queue to finish
        //             and finally free the command buffer
        endCmdBufferSingleUse( core::getLogicalDevice(), cmdPool, cmdBuffer, queue );

        assert( vulkan_cmd_buffer::CMD_NOT_ALLOCATED == cmdBuffer.cmdState ); 
        assert( cmdBuffer.cmdBuffer == nullptr );

    }

    uint32_t findMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags prop )
    {
        // Query for info about the types of available memory
        VkPhysicalDeviceMemoryProperties memProp{ };
        vkGetPhysicalDeviceMemoryProperties( core::getPhysicalDevice(), &memProp );

        for ( uint32_t i { 0 }; i < memProp.memoryTypeCount; i++ )
        {
            if ( ( typeFilter & ( 1 << i ) ) && 
                 ( ( memProp.memoryTypes[ i ].propertyFlags & prop ) == prop ) )
            {
                return i;
            }
        }

        assert( false );
    }

    bool createVertexBuffer( utils::vector<vulkan_vertex> vertices, 
                             VkBuffer &buffer, 
                             VkDeviceMemory &bufferMemory,
                             VkCommandPool cmdPool,
                             VkQueue queue )
    {
        VkDeviceSize bufferSize = sizeof( vertices[0] ) * vertices.size();

        VkBuffer stagingBuffer { }; 
        VkDeviceMemory stagingBufferMemory { };
        bool result { false };

        result = createBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                                           ( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ),
                                           stagingBuffer, stagingBufferMemory );

        if ( !result )
        {
            MSG("Failed to create staging buffer...");
            return false;
        }

        void* data;
        // TODO(klek): Do error checking!
        vkMapMemory( core::getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data );
        memcpy( data, vertices.data(), (size_t)bufferSize );
        vkUnmapMemory( core::getLogicalDevice(), stagingBufferMemory );

        result = createBuffer( bufferSize, ( VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
                                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT ),
                                           ( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ),
                                           buffer, bufferMemory );

        assert( result );
        if ( !result )
        {
            MSG("Failed to create buffer...");
            return false;
        }
        
        // TODO(klek): Figure out how to get a commandpool and queue here...
        copyBuffer( stagingBuffer, buffer, bufferSize, cmdPool, queue );

        // Finally destroy the staging buffer
        vkDestroyBuffer( core::getLogicalDevice(), stagingBuffer, nullptr );
        vkFreeMemory( core::getLogicalDevice(), stagingBufferMemory, nullptr );

        return true;
    }

    bool createIndexBuffer( utils::vector<vulkan_index> indices, 
                            VkBuffer &buffer, 
                            VkDeviceMemory &bufferMemory )
    {
        VkDeviceSize bufferSize = sizeof( indices[0] ) * indices.size();

        VkBuffer stagingBuffer { }; 
        VkDeviceMemory stagingBufferMemory { };
        bool result { false };

        result = createBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                                           ( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ),
                                           stagingBuffer, stagingBufferMemory );

        if ( !result )
        {
            MSG("Failed to create staging buffer...");
            return false;
        }

        void* data;
        // TODO(klek): Do error checking!
        vkMapMemory( core::getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data );
        memcpy( data, indices.data(), (size_t)bufferSize );
        vkUnmapMemory( core::getLogicalDevice(), stagingBufferMemory );

        result = createBuffer( bufferSize, ( VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
                                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT ),
                                           ( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ),
                                           buffer, bufferMemory );

        assert( result );
        if ( !result )
        {
            MSG("Failed to create buffer...");
            return false;
        }
        
        // TODO(klek): Figure out how to get a commandpool and queue here...
        //copyBuffer( stagingBuffer, buffer, bufferSize, cmdPool, queue );

        // Finally destroy the staging buffer
        vkDestroyBuffer( core::getLogicalDevice(), stagingBuffer, nullptr );
        vkFreeMemory( core::getLogicalDevice(), stagingBufferMemory, nullptr );

        return true;
    }

    bool createBuffer( VkDeviceSize size, 
                       VkBufferUsageFlags usage, 
                       VkMemoryPropertyFlags prop,
                       VkBuffer &buffer, 
                       VkDeviceMemory &bufferMemory )
    {
        VkBufferCreateInfo bufferInfo{ };
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.flags = 0;

        VkResult result { vkCreateBuffer( core::getLogicalDevice(), &bufferInfo, nullptr, &buffer ) };
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create a buffer...");
            return false;
        }

        MSG("Created a buffer!");

        // Get the memory requirements
        VkMemoryRequirements memReqs{ };
        vkGetBufferMemoryRequirements( core::getLogicalDevice(), buffer, &memReqs );

        VkMemoryAllocateInfo allocInfo{ };
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReqs.size;
        // allocInfo.memoryTypeIndex = findMemoryType( memReqs.memoryTypeBits, 
        //                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
        //                                             VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );
        allocInfo.memoryTypeIndex = findMemoryType( memReqs.memoryTypeBits, prop );

        result = vkAllocateMemory( core::getLogicalDevice(), &allocInfo, nullptr, &bufferMemory );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to allocate buffer memory...");
            return false;
        }

        MSG("Allocated buffer memory!");

        vkBindBufferMemory( core::getLogicalDevice(), buffer, bufferMemory, 0 );

        return true;
    }

    void destroyVertexBuffer( VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory )
    {
        if ( VK_NULL_HANDLE != vertexBuffer )
        {
            vkDestroyBuffer( core::getLogicalDevice(), vertexBuffer, nullptr );
        }
        if ( VK_NULL_HANDLE != vertexBufferMemory )
        {
            vkFreeMemory( core::getLogicalDevice(), vertexBufferMemory, nullptr );
        }
    }
} // namespace muggy::graphics::vulkan::vertex
