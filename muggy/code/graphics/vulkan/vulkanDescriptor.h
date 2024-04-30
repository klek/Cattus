//********************************************************************
//  File:    vulkanDescriptor.h
//  Date:    Sun, 28 Apr 2024: 16:43
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_DESCRIPTOR_H)
#define VULKAN_DESCRIPTOR_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan::descriptor
{
    // TODO(klek): Find a better name!
    class vulkan_descriptor_layout_builder
    {
    public:
        vulkan_descriptor_layout_builder( void ) { clear(); }

        void addBinding( uint32_t binding, VkDescriptorType type );
        void addShaderStages( VkShaderStageFlags shaderStages );
        void clear( void );

        bool build( VkDevice device,
                    VkDescriptorSetLayout& set,
                    void* pNext = nullptr,
                    VkDescriptorSetLayoutCreateFlags flags = 0 );

    private:
        utils::vector<VkDescriptorSetLayoutBinding>     m_Bindings;
        VkShaderStageFlags                              m_ShaderStages { };
    };

    class vulkan_descriptor_allocator
    {
    public:
        struct pool_size_ratio
        {
            VkDescriptorType    type;
            float               ratio;
        };

        vulkan_descriptor_allocator( void ) = default;
        constexpr explicit vulkan_descriptor_allocator( VkDevice device )
        : m_Device( device ) { }
        MUGGY_DISABLE_COPY_AND_MOVE( vulkan_descriptor_allocator );

        bool initPool( uint32_t maxSets,
                       utils::vector<pool_size_ratio> poolRatios );
        // bool initPool( uint32_t maxSets,
        //                pool_size_ratio poolRatios
        //                /*std::span<pool_size_ratio> poolRatios*/ );
        bool clearDescriptors( void );
        void destroyPool( void );

        bool allocate( VkDescriptorSetLayout layout,
                       VkDescriptorSet& set );
        
    private:
        const VkDevice          m_Device { VK_NULL_HANDLE };
        VkDescriptorPool        m_Pool { VK_NULL_HANDLE };
    };
} // namespace muggy::graphics::vulkan::descriptor


#endif