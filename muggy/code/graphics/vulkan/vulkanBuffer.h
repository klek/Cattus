//********************************************************************
//  File:    vulkanBuffer.h
//  Date:    Fri, 26 Apr 2024: 19:28
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_BUFFER_H)
#define VULKAN_BUFFER_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan
{
    class vulkan_buffer
    {
    public:
        vulkan_buffer( void ) = default;
        explicit vulkan_buffer( const VkDevice& device ) : m_Device( device ) {}
        MUGGY_DISABLE_COPY_AND_MOVE( vulkan_buffer );
        ~vulkan_buffer( void )
        {
            destroy();
        }

        // Functions
        VkResult map( VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0 );
        void unmap( void );

        VkResult bind( VkDeviceSize offset = 0 );
        void setupDescriptor( VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0 );
        void copyTo( void* data, VkDeviceSize size );
        VkResult flush( VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0 );
        VkResult invalidate( VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0 );

        VkResult create( VkBufferUsageFlags usageFlags,
                         VkMemoryPropertyFlags memProps,
                         VkDeviceSize size,
                         void* data );
        void destroy( void );

        // Get functions
        constexpr const VkBuffer& getBuffer( void ) const { return m_Buffer; }
    
    private:

        const VkDevice          m_Device { VK_NULL_HANDLE };
        VkBuffer                m_Buffer { VK_NULL_HANDLE };
        VkDeviceMemory          m_Memory { VK_NULL_HANDLE };
        VkDescriptorBufferInfo  m_Descriptor { };
        VkDeviceSize            m_Size { 0 };
        VkDeviceSize            m_Alignment { 0 };
        void*                   m_Mapped { nullptr };

        VkBufferUsageFlags      m_UsageFlags { 0 };
        VkMemoryPropertyFlags   m_MemProps { 0 };    
    };
} // namespace muggy::graphics::vulkan


#endif