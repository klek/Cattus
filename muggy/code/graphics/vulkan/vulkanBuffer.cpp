//********************************************************************
//  File:    vulkanBuffer.cpp
//  Date:    Fri, 26 Apr 2024: 19:37
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanBuffer.h"
#include "vulkanHelpers.h"
#include "vulkanCore.h"

namespace muggy::graphics::vulkan
{
    VkResult vulkan_buffer::map( VkDeviceSize size /*= VK_WHOLE_SIZE */,
                                 VkDeviceSize offset /* = 0 */ )
    {
        return vkMapMemory( m_Device, m_Memory, offset, size, 0, &m_Mapped );
    }

    void vulkan_buffer::unmap( void )
    {
        if ( m_Mapped )
        {
            vkUnmapMemory( m_Device, m_Memory );
            m_Mapped = nullptr;
        }
    }

    VkResult vulkan_buffer::bind( VkDeviceSize offset /*= 0 */ )
    {
        return vkBindBufferMemory( m_Device, m_Buffer, m_Memory, offset );
    }

    void vulkan_buffer::setupDescriptor( VkDeviceSize size /*= VK_WHOLE_SIZE */,
                                         VkDeviceSize offset /*= 0 */ )
    {
        m_Descriptor.offset = offset;
        m_Descriptor.buffer = m_Buffer;
        m_Descriptor.range = size;
    }

    void vulkan_buffer::copyTo( void* src, VkDeviceSize size )
    {
        assert( m_Mapped );
        memcpy( m_Mapped, src, size );
    }

    VkResult vulkan_buffer::flush( VkDeviceSize size /*= VK_WHOLE_SIZE */,
                                   VkDeviceSize offset /*= 0 */ )
    {
        VkMappedMemoryRange mappedRange { };
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_Memory;
        mappedRange.offset = offset;
        mappedRange.size = size;

        return vkFlushMappedMemoryRanges( m_Device, 1, &mappedRange );
    }

    VkResult vulkan_buffer::invalidate( VkDeviceSize size /*= VK_WHOLE_SIZE */,
                                        VkDeviceSize offset /*= 0 */ )
    {
        VkMappedMemoryRange mappedRange { };
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_Memory;
        mappedRange.offset = offset;
        mappedRange.size = size;

        return vkInvalidateMappedMemoryRanges( m_Device, 1, &mappedRange );
    }

    VkResult vulkan_buffer::create( VkBufferUsageFlags usageFlags,
                                    VkMemoryPropertyFlags memProps,
                                    VkDeviceSize size,
                                    void* data )
    {
        // Must have a valid device
        assert( VK_NULL_HANDLE != m_Device );
        // Size can not be zero
        assert( 0 != size );
        VkResult result { VK_SUCCESS };

        // Create the buffer handle
        VkSharingMode mode { VK_SHARING_MODE_EXCLUSIVE };
        VkBufferCreateInfo bufferInfo { vulkan_utils::bufferCreateInfo( usageFlags, size, mode ) };
        result = vkCreateBuffer( m_Device, &bufferInfo, nullptr, &m_Buffer );
        if ( VK_SUCCESS != result )
        {
            MSG("Could not create buffer...");
            return result;
        }

        // Create the memory backing up the handle
        VkMemoryRequirements memReqs { };
        VkMemoryAllocateInfo memAllocInfo { vulkan_utils::memoryAllocationInfo() };
        vkGetBufferMemoryRequirements( m_Device, m_Buffer, &memReqs );
        memAllocInfo.allocationSize = memReqs.size;
        // Find a memory type index that fits the requested properties
        core::findMemoryIndex( memReqs.memoryTypeBits, memProps, memAllocInfo.memoryTypeIndex );
        // If the buffer has VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT
        // set we also need to enable the appropriate flag during
        // allocation
        VkMemoryAllocateFlagsInfoKHR allocFlagsInfo { };
        if ( usageFlags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT )
        {
            allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
            allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
            memAllocInfo.pNext = &allocFlagsInfo;
        }
        // Allocate memory
        result = vkAllocateMemory( m_Device, &memAllocInfo, nullptr, &m_Memory );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to allocate memory for buffer...");
            return result;
        }

        // Setup internal variables
        m_Alignment = memReqs.alignment;
        m_Size = size;
        m_UsageFlags = usageFlags;
        m_MemProps = memProps;

        // If a valid pointer was submitted for data, we map the current
        // buffer and copy data to it
        if ( nullptr != data )
        {
            result = map( size, 0 );
            if ( VK_SUCCESS != result )
            {
                MSG("Failed to map memory...");
                return result;
            }
            copyTo( data, size );
            // If host coherency has not been requested we need to do a
            // manual flush to make writes visible
            if ( ( memProps & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) == 0 )
            {
                flush( size, 0 );
            }
            unmap( );
        }
        // Initialize a default descriptor that covers the whole buffer
        // size
        setupDescriptor( );

        // Finally bind memory to buffer
        return bind( );
    }

    void vulkan_buffer::destroy( void )
    {
        if ( VK_NULL_HANDLE != m_Buffer )
        {
            vkDestroyBuffer( m_Device, m_Buffer, nullptr );
            m_Buffer = VK_NULL_HANDLE;
        }
        if ( VK_NULL_HANDLE != m_Memory )
        {
            vkFreeMemory( m_Device, m_Memory, nullptr );
            m_Memory = VK_NULL_HANDLE;
        }
    }


} // namespace muggy::graphics::vulkan
