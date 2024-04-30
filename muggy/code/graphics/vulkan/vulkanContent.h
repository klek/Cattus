//********************************************************************
//  File:    vulkanContent.h
//  Date:    Fri, 26 Apr 2024: 02:14
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_CONTENT_H)
#define VULKAN_CONTENT_H

#include "vulkanCommon.h"
#include "vulkanVertex.h"
#include "vulkanBuffer.h"

namespace muggy::graphics::vulkan::content
{
    struct vulkan_mesh
    {
        utils::vector<vertex::vulkan_vertex>    vertices { };
        utils::vector<uint32_t>                 indices { };
    };

    struct vulkan_push_constant
    {
        float                              width { };
        float                              heigth { };
        float                              frame { };
    };

    class vulkan_object
    {
    public:
        explicit vulkan_object( const vulkan_mesh& mesh );
        MUGGY_DISABLE_COPY_AND_MOVE( vulkan_object );
        ~vulkan_object( )
        {
            release( );
        }
        
        void release( void );

    private:
        bool createVertexBuffer( void );
        void destroyVertexBuffer( void );

        vulkan_buffer                           m_Buffer;
        utils::vector<vertex::vulkan_vertex>    m_VertexBuffer { };
        utils::vector<uint32_t>                 m_Indices { };
    };
    
    bool init( const VkRenderPass& renderPass );
    void shutdown( void );

    void copyObjects( const VkCommandBuffer& cmdBuffer );
    void drawObjects( const VkCommandBuffer& cmdBuffer, vulkan_push_constant push );
} // namespace muggy::graphics::vulkan::content


#endif