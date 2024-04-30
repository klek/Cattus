//********************************************************************
//  File:    vulkanValidation.h
//  Date:    Mon, 11 Mar 2024: 18:24
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_VALIDATION_H)
#define VULKAN_VALIDATION_H

#include "vulkanCommon.h"

#if defined( DEBUG_BUILD )
//constexpr bool enableValidationLayers{ true };
#define ENABLE_VALIDATION_LAYERS            true
#else
//constexpr bool enableValidationLayers{ false };
#define ENABLE_VALIDATION_LAYERS            false
#endif


namespace muggy::graphics::vulkan
{
    namespace // Anonymous namespace, ie only for use in this cpp-file
    {
        // VK_LAYER_KHRONOS_validation = All standard validation layers
        const utils::vector<const char*> validationLayers { 1, "VK_LAYER_KHRONOS_validation" };
    } // Anonymous namespace, ie only for use in this cpp-file
    
    bool isValidationLayerSupported( void )
    {
        // List all the available layers
        uint32_t layerCount { 0 };
        vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

        utils::vector<VkLayerProperties> availableLayers( layerCount );
        vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

        // Check if all validationLayers exists in availableLayers
        for ( const auto& layerName : validationLayers )
        {
            bool layerFound { false };

            for ( const auto& layerProperties : availableLayers )
            {
                if ( strcmp( layerName, layerProperties.layerName ) == 0 )
                {
                    layerFound = true;
                    break;
                }
            }

            // Did we find the current layer?
            if ( !layerFound )
            {
                return false;
            }
        }

        // Well, we reached the end, so all layers must have been found
        return true;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL 
    debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                   const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                   [[maybe_unused]] void* userData )
    {
        if ( messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
        {
            // NOTE(klek): Message is important enough to show
            MSG("Validation layer: ");
            MSG( callbackData->pMessage );
        }

        // VK_FALSE indicates that the program should not terminate
        return VK_FALSE;
    }

    VkResult createDebugUtilsMessengerExt( VkInstance instance, 
                                           const VkDebugUtilsMessengerCreateInfoEXT* createInfo, 
                                           const VkAllocationCallbacks* allocator, 
                                           VkDebugUtilsMessengerEXT* debugMessenger )
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr( instance,
                                                                                "vkCreateDebugUtilsMessengerEXT" );
        if ( func != nullptr )
        {
            return func( instance, createInfo, allocator, debugMessenger );
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void destroyDebugUtilsMessengerExt( VkInstance instance, 
                                        VkDebugUtilsMessengerEXT debugMessenger, 
                                        const VkAllocationCallbacks* allocator )
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr( instance, 
                                                                                 "vkDestroyDebugUtilsMessengerEXT" );
        if ( func != nullptr )
        {
            func( instance, debugMessenger, allocator );
        }
    }

    // Sets up the CreateInfo struct for debugging
    void setupDebugCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& info )
    {
        // Return if we are not doing validation layers
        if ( !ENABLE_VALIDATION_LAYERS )
        {
            return;
        }

        info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        info.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        info.pfnUserCallback = debugCallback;
        info.pUserData = nullptr;
    }


} // namespace muggy::graphics::vulkan


#endif