//********************************************************************
//  File:    vulkanResources.cpp
//  Date:    Mon, 18 Mar 2024: 23:50
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanResources.h"
#include "vulkanCore.h"

namespace muggy::graphics::vulkan
{
    bool createImage( const image_init_info* const initInfo, 
                      vulkan_image& image )
    {
        VkResult result { VK_SUCCESS };
        image.width = initInfo->width;
        image.height = initInfo->height;

        // Create an image
        {
            VkImageCreateInfo info { };
            info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            // TODO(klek): This should be configurable
            info.imageType = VK_IMAGE_TYPE_2D;
            info.extent.width = initInfo->width;
            info.extent.height = initInfo->height;
            // TODO(klek): This should be configurable
            info.extent.depth = 1;
            // TODO(klek): This should be configurable
            info.mipLevels = 1;
            // TODO(klek): This should be configurable
            info.arrayLayers = 1;
            info.format = initInfo->format;
            info.tiling = initInfo->tiling;
            info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            info.usage = initInfo->usageFlags;
            // TODO(klek): This should be configurable
            info.samples = VK_SAMPLE_COUNT_1_BIT;
            // TODO(klek): This should be configurable
            info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            result = vkCreateImage( initInfo->device, &info, nullptr, &image.image );
            if ( result != VK_SUCCESS )
            {
                MSG("Failed to create image...");
                return false;
            }
        }

        // Get memory requirements for image
        VkMemoryRequirements memReqs;
        vkGetImageMemoryRequirements( initInfo->device, image.image, &memReqs );

        int32_t index { core::findMemoryIndex( memReqs.memoryTypeBits, initInfo->memoryFlags ) };
        if ( index == -1 )
        {
            MSG("The required memory type was not found...");
            assert( index == -1 );
        }

        // Allocate memory for image
        {
            VkMemoryAllocateInfo info { };
            info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            info.allocationSize = memReqs.size;
            info.memoryTypeIndex = index;

            result = vkAllocateMemory( initInfo->device, &info, nullptr, &image.memory );
            if ( result != VK_SUCCESS )
            {
                MSG("Failed to allocate memory for image...");
                return false;
            }
        }

        // TODO(klek): Make memory offset configurable, for use in things
        //             like image pooling
        result = vkBindImageMemory( initInfo->device, image.image, image.memory, 0 );
        if ( result != VK_SUCCESS )
        {
            MSG("Failed to bind image memory...");
            return false;
        }

        if ( initInfo->createView )
        {
            image.view = nullptr;
            if ( !createImageView( initInfo->device, initInfo->format, &image, initInfo->viewAspectFlags ) )
            {
                return false;
            }
        }

        return true;
    }

    bool createImageView( VkDevice device, 
                          VkFormat format, 
                          vulkan_image* image,
                          VkImageAspectFlags viewAspectFlags )
    {
        VkImageViewCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image = image->image;
        // TODO(klek): This should be configurable
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = format;
        info.subresourceRange.aspectMask = viewAspectFlags;
        // TODO(klek): This should be configurable
        info.subresourceRange.baseMipLevel = 0;
        // TODO(klek): This should be configurable
        info.subresourceRange.levelCount = 1;
        // TODO(klek): This should be configurable
        info.subresourceRange.baseArrayLayer = 0;
        // TODO(klek): This should be configurable
        info.subresourceRange.layerCount = 1;

        VkResult result { VK_SUCCESS };
        result = vkCreateImageView( device, &info, nullptr, &image->view );
        if ( result != VK_SUCCESS )
        {
            MSG("Failed to create image view...");
            return false;
        }

        return true;
    }

    void destroyImage( VkDevice device, vulkan_image* image )
    {
        if ( image->view )
        {
            vkDestroyImageView( device, image->view, nullptr );
            image->view = nullptr;
        }

        if ( image->memory )
        {
            vkFreeMemory( device, image->memory, nullptr );
            image->memory = nullptr;
        }

        if ( image->image )
        {
            vkDestroyImage( device, image->image, nullptr );
            image->image = nullptr;
        }
    }

    bool createFrameBuffer( VkDevice device,
                            vulkan_renderpass& renderPass,
                            uint32_t width,
                            uint32_t height,
                            uint32_t attachCount,
                            VkImageView* attachments,
                            vulkan_framebuffer& frameBuffer )
    {
        frameBuffer.attachments.resize( attachCount );
        for ( uint32_t i{ 0 }; i < attachCount; i++ )
        {
            frameBuffer.attachments[ i ] = attachments[ i ];
        }
        frameBuffer.renderPass = &renderPass;
        frameBuffer.attachCount = attachCount;

        // 
        VkFramebufferCreateInfo info { };
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = renderPass.renderPass;
        info.attachmentCount = attachCount;
        info.pAttachments = frameBuffer.attachments.data();
        info.width = width;
        info.height = height;
        info.layers = 1;

        VkResult result { VK_SUCCESS };
        result = vkCreateFramebuffer( device, &info, nullptr, &frameBuffer.frameBuffer );
        if ( result != VK_SUCCESS )
        {
            MSG("Failed to create a framebuffer...");
            return false;
        }
        
        MSG("Create a framebuffer!");
        return true;
    }
    
    void destroyFrameBuffer( VkDevice device, 
                             vulkan_framebuffer& frameBuffer )
    {
        vkDestroyFramebuffer( device, frameBuffer.frameBuffer, nullptr );
        if ( frameBuffer.attachments.data() )
        {
            frameBuffer.attachments.clear();
        }

        frameBuffer.frameBuffer = nullptr;
        frameBuffer.renderPass = nullptr;
        frameBuffer.attachCount = 0;

        MSG("Destroyed framebuffer!");
    }

} // namespace muggy::graphics::vulkan
