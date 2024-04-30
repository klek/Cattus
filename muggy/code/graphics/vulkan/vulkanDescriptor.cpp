//********************************************************************
//  File:    vulkanDescriptor.cpp
//  Date:    Mon, 29 Apr 2024: 15:36
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanDescriptor.h"

namespace muggy::graphics::vulkan::descriptor
{
    namespace
    {
        
    } // anonymous namespace

    //****************************************************************
    // vulkan_descriptor_layout_builder implementation
    void vulkan_descriptor_layout_builder::addBinding( uint32_t binding,
                                                       VkDescriptorType type )
    {
        VkDescriptorSetLayoutBinding newBinding { };
        newBinding.binding = binding;
        newBinding.descriptorType = type;
        newBinding.descriptorCount = 1;
        // This is set via addShaderStages
        // newBinding.stageFlags = 0;
        // Currently not in use
        // newBinding.pImmutableSamplers = nullptr;

        m_Bindings.push_back( newBinding );
    }

    void vulkan_descriptor_layout_builder::clear( void )
    {
        m_Bindings.clear();
    }

    // NOTE(klek): Currently we only support the same stage-flags
    //             for all bindings!
    void vulkan_descriptor_layout_builder::addShaderStages( VkShaderStageFlags shaderStages )
    {
        m_ShaderStages = shaderStages;
    }

    bool
    vulkan_descriptor_layout_builder::build( VkDevice device,
                                             VkDescriptorSetLayout& set,
                                             void* pNext /*= nullptr*/,
                                             VkDescriptorSetLayoutCreateFlags flags /*= 0*/ )
    {
        VkResult result { VK_SUCCESS };

        // Set the flags for all bindings
        // NOTE(klek): Currently we only support the same stage-flags
        //             for all bindings!
        for ( VkDescriptorSetLayoutBinding& b : m_Bindings )
        {
            b.stageFlags |= m_ShaderStages;
        }

        VkDescriptorSetLayoutCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.pNext = pNext;
        info.bindingCount = static_cast<uint32_t>( m_Bindings.size( ) );
        info.pBindings = m_Bindings.data( );
        info.flags = flags;

        result = vkCreateDescriptorSetLayout( device, &info, nullptr, &set );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create a descriptor set layout...");
            return false;
        }

        return true;
    }

    //****************************************************************
    // vulkan_descriptor_allocator implementation
    // bool vulkan_descriptor_allocator::initPool( uint32_t maxSets,
    //                                             pool_size_ratio poolRatios
    //                                             /*std::span<pool_size_ratio> poolRatios*/ )
    bool vulkan_descriptor_allocator::initPool( uint32_t maxSets,
                                                utils::vector<pool_size_ratio> poolRatios )
    {
        VkResult result { VK_SUCCESS };
        utils::vector<VkDescriptorPoolSize> poolSizes;
        // TODO(klek): Add span functionality with for-loop here
        for ( pool_size_ratio& ratio : poolRatios )
        {
            poolSizes.push_back( VkDescriptorPoolSize{ 
                ratio.type,
                static_cast<uint32_t>( ratio.ratio * maxSets )
            } );
        }

        VkDescriptorPoolCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.maxSets = maxSets;
        info.poolSizeCount = static_cast<uint32_t>( poolSizes.size() );
        info.pPoolSizes = poolSizes.data();
        
        result = vkCreateDescriptorPool( m_Device, &info, nullptr, &m_Pool );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create a descriptor pool...");
            return false;
        }
        return true;
    }

    bool vulkan_descriptor_allocator::clearDescriptors( void )
    {
        VkResult result { VK_SUCCESS };
        result = vkResetDescriptorPool( m_Device, m_Pool, 0 );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to reset the descriptor pool...");
            return false;
        }
        return true;
    }

    void vulkan_descriptor_allocator::destroyPool( void )
    {
        if( VK_NULL_HANDLE != m_Pool )
        {
            vkDestroyDescriptorPool( m_Device, m_Pool, nullptr );
            m_Pool = VK_NULL_HANDLE;
        }
    }

    bool vulkan_descriptor_allocator::allocate( VkDescriptorSetLayout layout,
                                                VkDescriptorSet& set )
    {
        VkResult result { VK_SUCCESS };
        VkDescriptorSetAllocateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.pNext = nullptr;
        info.descriptorPool = m_Pool;
        info.descriptorSetCount = 1;
        info.pSetLayouts = &layout;

        result = vkAllocateDescriptorSets( m_Device, &info, &set );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to allocate descriptor sets...");
            return false;
        }
        return true;
    }

    //****************************************************************
    // 
    
} // namespace muggy::graphics::vulkan::descriptor
