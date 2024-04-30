//********************************************************************
//  File:    vulkanVertex.h
//  Date:    Fri, 12 Apr 2024: 16:21
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_VERTEX_H)
#define VULKAN_VERTEX_H

#include "vulkanCommon.h"
//#include "vulkanCore.h"

namespace muggy::graphics::vulkan::vertex
{
    struct vulkan_vertex
    {
        math::fv4d pos;
        math::fv4d color;

        static utils::vector<VkVertexInputBindingDescription>
        getBindingDescriptions( uint32_t binding )
        {
            utils::vector<VkVertexInputBindingDescription> bindingDescription { };
            VkVertexInputBindingDescription desc { };
            desc.binding = binding;
            desc.stride = sizeof( vulkan_vertex );
            desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            bindingDescription.push_back( desc );

            return bindingDescription;
        }

        static utils::vector<VkVertexInputAttributeDescription>
        getAttributeDescriptions( uint32_t binding )
        {
            utils::vector<VkVertexInputAttributeDescription> attributeDescriptions{ };
            VkVertexInputAttributeDescription desc { };
            // Describing first member of vulkan_vertex struct, which
            // is pos
            desc.location = 0;
            desc.binding = binding;
            desc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
            desc.offset = offsetof( vulkan_vertex, pos );
            attributeDescriptions.push_back( desc );

            // Describing second member of vulkan_vertex struct, which
            // is color
            desc.location = 1;
            desc.binding = binding;
            desc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
            desc.offset = offsetof( vulkan_vertex, color );
            attributeDescriptions.push_back( desc );

            return attributeDescriptions;
        }
    };

    using vulkan_index = uint32_t;
    // struct vulkan_index
    // {
    //     uint32_t index;
    // };

    // TODO(klek): Remove this when we support input from 
    //             content tools
    // Global vertices
    //static utils::vector<vulkan_vertex> vertices{};

    uint32_t findMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags prop );

    bool createVertexBuffer( utils::vector<vulkan_vertex> vertices,
                             VkBuffer &vertexBuffer,
                             VkDeviceMemory &vertexBufferMemory,
                             VkCommandPool cmdPool,
                             VkQueue queue );

    bool createBuffer( VkDeviceSize size,
                       VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags prop,
                       VkBuffer &buffer,
                       VkDeviceMemory &bufferMemory );

    void destroyVertexBuffer( VkBuffer &vertexBuffer, VkDeviceMemory &vertexBufferMemory );

} // namespace muggy::graphics::vulkan::vertex



#endif