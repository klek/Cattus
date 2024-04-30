//********************************************************************
//  File:    vulkanContent.cpp
//  Date:    Fri, 26 Apr 2024: 02:25
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanContent.h"
#include "vulkanHelpers.h"
#include "vulkanCore.h"
#include "vulkanPipeline.h"
#include "vulkanShaders.h"
#include "vulkanDescriptor.h"

namespace muggy::graphics::vulkan::content
{
    namespace
    {
        utils::vector<vertex::vulkan_vertex>    vertices;
        utils::vector<uint32_t>                 indices;
        // Maybe this should be a freelist
        //utils::vector<vulkan_object>    gameObjects { };
        struct object_buffer
        {
            vulkan_buffer                       vertices;
            vulkan_buffer                       indices;
        };
        object_buffer                           staging;
        object_buffer                           toRender;

        struct triangle_pipeline
        {
            VkPipeline                          pipeline { VK_NULL_HANDLE };
            VkPipelineLayout                    layout { VK_NULL_HANDLE };
        };
        triangle_pipeline                       triangleMaterial;
        descriptor::vulkan_descriptor_allocator descriptorAllocator;
        VkDescriptorSet                         descriptorSet;
        VkDescriptorSetLayout                   descriptorLayout;
        bool                                    isInitialized { false };
    } // anonymous namespace
    
    // vulkan_object::vulkan_object( const vulkan_mesh& mesh )
    //  : 
    //     m_VertexBuffer( mesh.vertices ),
    //     m_Indices( mesh.indices )
    // {
    //     if ( !createVertexBuffer( ) )
    //     {
    //         release();
    //     }
    // }

    // void vulkan_object::release( void )
    // {
    //     if ( VK_NULL_HANDLE != m_Buffer ||
    //          VK_NULL_HANDLE != m_Memory )
    //     {
    //         vertex::destroyVertexBuffer( m_Buffer, m_Memory );
    //         m_Buffer = VK_NULL_HANDLE;
    //         m_Memory = VK_NULL_HANDLE;
    //     }
    // }

    // bool vulkan_object::createVertexBuffer( void )
    // {
    //     struct stagingBuffer
    //     {
    //         VkDeviceMemory  memory { };
    //         VkBuffer        buffer { };
    //     };

    //     struct 
    //     {
    //         stagingBuffer   vertices { };
    //         stagingBuffer   indices { };
    //     } stagingBuffers;
    //     VkResult result { VK_SUCCESS };

    //     // Calculate size of the vertices buffer
    //     uint32_t vertexBufferSize = static_cast<uint32_t>( m_VertexBuffer.size() ) * sizeof( vertex::vulkan_vertex );
    //     // Calculate the size of the indices buffer
    //     uint32_t indexBufferSize = static_cast<uint32_t>( m_Indices.size() ) * sizeof( uint32_t );

    //     // Memory allocation and requirements structures
    //     VkMemoryAllocateInfo memAllocInfo { };
    //     memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     VkMemoryRequirements memReq { };

    //     // The data pointer we map to the memory we copy into
    //     void* data;

    //     // Copy of vertices to GPU
    //     // Create a staging buffer that is host visible and coherent
    //     // This should also be marked as source for transfer
    //     VkBufferCreateInfo vertexBufferInfo { };
    //     vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    //     vertexBufferInfo.size = vertexBufferSize;
    //     vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    //     result = vkCreateBuffer( core::getLogicalDevice(),
    //                              &vertexBufferInfo,
    //                              nullptr,
    //                              &stagingBuffers.vertices.buffer );
    //     if ( VK_SUCCESS != result )
    //     {
    //         MSG("Failed to create a staging buffer...");
    //         return false;
    //     }
    //     // Get the memory requirements for said buffer
    //     vkGetBufferMemoryRequirements( core::getLogicalDevice(),
    //                                    stagingBuffers.vertices.buffer,
    //                                    &memReq );
    //     memAllocInfo.allocationSize = memReq.size;
    //     bool res = core::findMemoryIndex( memReq.memoryTypeBits,
    //                                       ( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
    //                                         VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ),
    //                                       memAllocInfo.memoryTypeIndex );
    //     assert ( res );
    //     // Allocate memory for said buffer
    //     result = vkAllocateMemory( core::getLogicalDevice(),
    //                                &memAllocInfo,
    //                                nullptr,
    //                                &stagingBuffers.vertices.memory );
    //     // Now we map the memory and copy data to it
    //     result = vkMapMemory( core::getLogicalDevice(),
    //                           stagingBuffers.vertices.memory,
    //                           0,
    //                           memAllocInfo.allocationSize,
    //                           0,
    //                           &data );
    //     memcpy( data, m_VertexBuffer.data(), vertexBufferSize );
    //     vkUnmapMemory( core::getLogicalDevice(), stagingBuffers.vertices.memory );
    //     // Bind buffer
    //     result = vkBindBufferMemory( core::getLogicalDevice(),
    //                                  stagingBuffers.vertices.buffer,
    //                                  stagingBuffers.vertices.memory,
    //                                  0 );

    //     // Create device local buffer which we also need to keep

    //     // Copy of indices to GPU
    //     // This is essentially same story as the stuff for vertex

    //     // Destroy staging buffers
    //     vkDestroyBuffer( core::getLogicalDevice(), stagingBuffers.vertices.buffer, nullptr );
    //     vkFreeMemory( core::getLogicalDevice(), stagingBuffers.vertices.memory, nullptr );

    //     return false;
    // }

    // void vulkan_object::destroyVertexBuffer( void )
    // {

    // }

    bool init( const VkRenderPass& renderPass )
    {
        if ( isInitialized )
        {
            return true;
        }
        VkResult result { VK_SUCCESS };
        // Initialize buffers
        const VkDevice& device = core::getLogicalDevice();
        new ( &staging.vertices ) vulkan_buffer( device );
        result = staging.vertices.create( VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                          ( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ),
                                          512, nullptr );
        if ( VK_SUCCESS != result )
        {
            shutdown();
            return false;
        }
        new ( &staging.indices ) vulkan_buffer( device );
        result = staging.indices.create( VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                         ( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ),
                                         512, nullptr );
        if ( VK_SUCCESS != result )
        {
            shutdown();
            return false;
        }

        new ( &toRender.vertices ) vulkan_buffer( device );
        result = toRender.vertices.create( ( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                             VK_BUFFER_USAGE_TRANSFER_DST_BIT ),
                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                           512, nullptr );
        if ( VK_SUCCESS != result )
        {
            shutdown();
            return false;
        }
        
        new ( &toRender.indices ) vulkan_buffer( device );
        result = toRender.indices.create( ( VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT ),
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                          512, nullptr );
        if ( VK_SUCCESS != result )
        {
            shutdown();
            return false;
        }

        // Create a new game object
        // Which is a simple triangle 
        vertices.push_back( { .pos{ -1.0f, -1.0f, 0.0f, 1.0f }, .color{ 1.0f, 0.0f, 0.0f, 1.0f } } );
        vertices.push_back( { .pos{ -1.0f, 1.0f, 0.0f, 1.0f }, .color{ 0.0f, 1.0f, 0.0f, 1.0f } } );
        vertices.push_back( { .pos{ 1.0f, 1.0f, 0.0f, 1.0f }, .color{ 0.0f, 0.0f, 1.0f, 1.0f } } );
        vertices.push_back( { .pos{ 1.0f, -1.0f, 0.0f, 1.0f }, .color{ 0.0f, 0.0f, 0.0f, 1.0f } } );

        // Setup the indices for same triangle
        indices.push_back( 0 );
        indices.push_back( 1 );
        indices.push_back( 2 );
        indices.push_back( 0 );
        indices.push_back( 2 );
        indices.push_back( 3 );

        // Copy values into the staging vertex buffers
        staging.vertices.map( );
        staging.vertices.copyTo( vertices.data(),
                                 vertices.size() * sizeof( vertex::vulkan_vertex ) );
        staging.vertices.unmap( );

        // Copy values info the staging index buffers
        staging.indices.map( );
        staging.indices.copyTo( indices.data(),
                                indices.size() * sizeof( uint32_t ) );
        staging.vertices.unmap( );

        // Initialize the material for the triangle
        // Create descriptors
        // utils::vector<descriptor::vulkan_descriptor_allocator::pool_size_ratio> poolSizes
        // {
        //     1, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 }
        // };
        // // First initialize the descriptor allocator
        // new ( &descriptorAllocator ) descriptor::vulkan_descriptor_allocator( device );
        // // Then initialize the pool with 10 sets
        // descriptorAllocator.initPool( 10, poolSizes );
        // // Make the descriptor set layout
        // {
        //     descriptor::vulkan_descriptor_layout_builder builder;
        //     builder.addBinding( 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER );
        //     if ( !builder.build( device, descriptorLayout ) )
        //     {
        //         shutdown( );
        //         return false;
        //     }
        // }
        // With the layout, we can now allocate in the pool
        // if ( !descriptorAllocator.allocate( descriptorLayout, descriptorSet ) )
        // {
        //     shutdown( );
        //     return false;
        // }
        // Configure the actual descriptor
        // VkDescriptorBufferInfo bufInfo { };
        // VkWriteDescriptorSet writeInfo { };
        // writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        // writeInfo.pNext = nullptr;
        // writeInfo.dstBinding = 0;
        // writeInfo.dstSet = descriptorSet;
        // writeInfo.descriptorCount = 1;
        // writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        // writeInfo.pBufferInfo = &bufInfo;
        // vkUpdateDescriptorSets( device, 1, &writeInfo, 0, nullptr );

        // Create push constants
        VkPushConstantRange pushConstantRange { };
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof( vulkan_push_constant );

        // Create a pipeline layout
        VkPipelineLayoutCreateInfo layoutInfo { vulkan_utils::defaultPipelineLayoutInfo() };
        layoutInfo.pushConstantRangeCount = 1;
        layoutInfo.pPushConstantRanges = &pushConstantRange;
        // layoutInfo.setLayoutCount = 1;
        // layoutInfo.pSetLayouts = &descriptorLayout;
        result = vkCreatePipelineLayout( device, &layoutInfo, nullptr, &triangleMaterial.layout );
        if ( VK_SUCCESS != result )
        {
            shutdown();
            return false;
        }
        // Create a pipeline via pipeline builder
        pipeline::vulkan_pipeline_builder builder;
        // Create shader modules
        VkShaderModule vertShader { };
        VkShaderModule fragShader { };
        shaders::createShaderModule( device,
                                     shaders::engine_shader::triangleShaderVS,
                                     vertShader );
        shaders::createShaderModule( device,
                                     shaders::engine_shader::juliaSetFS,
                                     fragShader );
        builder.setShaderStages( vertShader, fragShader );
        // Set binding and attribute descriptions
        builder.setVertexBindings( vertex::vulkan_vertex::getBindingDescriptions( 0 ) );
        builder.setVertexAttributes( vertex::vulkan_vertex::getAttributeDescriptions( 0 ) );
        // Set layout
        builder.setLayout( triangleMaterial.layout );
        // Set renderpass
        builder.setRenderPass( renderPass, 0 );
        // Set viewport and scissor
        builder.setDynamicViewPort( );
        builder.setDynamicScissor( );
        // Set color blend info
        builder.disableBlending( );
        // Set topology
        builder.setInputTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST );
        // Setup rasterization
        builder.setPolygonMode( VK_POLYGON_MODE_FILL );
        builder.setCullMode( VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE );
        builder.setLineWidth( );
        builder.disableMultiSampling( );
        builder.disableDepthTesting( );

        // Build the pipeline
        if ( !builder.buildPipeline( device, triangleMaterial.pipeline ) )
        {
            shutdown();
            return false;
        }

        // Destroy our shader modules
        vkDestroyShaderModule( device, vertShader, nullptr );
        vkDestroyShaderModule( device, fragShader, nullptr );

        // Set initialized to true
        isInitialized = true;

        return true;
    }

    void shutdown( void )
    {
        VkDevice device = core::getLogicalDevice();
        // Clear the vectors
        vertices.clear();
        indices.clear();

        // Destroy the buffers we have created
        staging.vertices.destroy();
        staging.indices.destroy();
        toRender.vertices.destroy();
        toRender.indices.destroy();


        // Destroy pipeline and layout
        if ( VK_NULL_HANDLE != triangleMaterial.pipeline )
        {
            vkDestroyPipeline( device, triangleMaterial.pipeline, nullptr );
            triangleMaterial.pipeline = VK_NULL_HANDLE;
        }
        if ( VK_NULL_HANDLE != triangleMaterial.layout )
        {
            vkDestroyPipelineLayout( device, triangleMaterial.layout, nullptr );
            triangleMaterial.layout = VK_NULL_HANDLE;
        }

        // Destroy descriptor pool and layout
        descriptorAllocator.destroyPool( );
        if ( VK_NULL_HANDLE != descriptorLayout )
        {
            vkDestroyDescriptorSetLayout( device, descriptorLayout, nullptr );
        }

        // Set initialized to false
        isInitialized = false;
    }

    //bool copyObjects( const VkCommandPool& cmdPool )
    void copyObjects( const VkCommandBuffer& cmdBuffer )
    {
        // VkResult result { VK_SUCCESS };
        // // Setup a command buffer for the copy
        // VkCommandBuffer copyCmd { };
        // VkCommandBufferAllocateInfo cmdBufAllocInfo { };
        // cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        // cmdBufAllocInfo.commandPool = cmdPool;
        // cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        // cmdBufAllocInfo.commandBufferCount = 1;
        // result = vkAllocateCommandBuffers( core::getLogicalDevice(),
        //                                    &cmdBufAllocInfo,
        //                                    &copyCmd );
        // if ( VK_SUCCESS != result )
        // {
        //     MSG("Failed to create copy command buffer...");
        //     return false;
        // }

        // // Begin the command buffer
        // VkCommandBufferBeginInfo cmdBufBegin { vulkan_utils::setCmdBufferBeginInfo( ) };
        // result = vkBeginCommandBuffer( copyCmd, &cmdBufBegin );
        // if ( VK_SUCCESS != result )
        // {
        //     MSG("Failed to begin copy command buffer...");
        //     return false;
        // }
        // Copy data from staging buffers into device local
        // This requires a commandbuffer
        VkBufferCopy copyRegion { };
        // Copy vertex buffer
        copyRegion.size = vertices.size() * sizeof( vertex::vulkan_vertex );
        vkCmdCopyBuffer( cmdBuffer,
                         staging.vertices.getBuffer(),
                         toRender.vertices.getBuffer(),
                         1,
                         &copyRegion );
        // Copy index buffer
        copyRegion.size = indices.size() * sizeof( uint32_t );
        vkCmdCopyBuffer( cmdBuffer,
                         staging.indices.getBuffer(),
                         toRender.indices.getBuffer(),
                         1,
                         &copyRegion );
    }

    void drawObjects( const VkCommandBuffer& cmdBuffer, vulkan_push_constant push )
    {
        // NOTE(klek): Updating staging buffers happens outside
        //             of this loop currently!

        // Setup descriptor sets

        // Bind pipelines
        vkCmdBindPipeline( cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, triangleMaterial.pipeline );

        // Bind vertexbuffers
        VkDeviceSize offset [1]{ 0 };
        vkCmdBindVertexBuffers( cmdBuffer,
                                0,
                                1,
                                &toRender.vertices.getBuffer(),
                                offset );

        // Bind the index buffer
        vkCmdBindIndexBuffer( cmdBuffer,
                              toRender.indices.getBuffer(),
                              0,
                              VK_INDEX_TYPE_UINT32 );
        
        // Setup push constants
        vkCmdPushConstants( cmdBuffer,
                            triangleMaterial.layout,
                            ( VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ),
                            0,
                            sizeof( vulkan_push_constant ),
                            &push );
        
        // Finally we draw index
        vkCmdDrawIndexed( cmdBuffer, indices.size(), 1, 0, 0, 1 );
    }

} // namespace muggy::graphics::vulkan::content
