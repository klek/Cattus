//********************************************************************
//  File:    vulkanSwapchain.h
//  Date:    Sun, 21 Apr 2024: 19:05
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_SWAPCHAIN_H)
#define VULKAN_SWAPCHAIN_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan
{
    struct swapchain_details 
    {
        // Surface properties, ie image size/extent
        VkSurfaceCapabilitiesKHR            surfaceCapabilities { };
        // Surface image formats supported, ie RGBA, size of each color
        utils::vector<VkSurfaceFormatKHR>   formats { };
        // How images should be presented to the screen
        utils::vector<VkPresentModeKHR>     presentationModes { };
    };

    struct swapchain_image
    {
        VkImage                             image;
        VkImageView                         imageView;
    };

    class vulkan_swapchain
    {
    public:
        //vulkan_swapchain() = default;
        explicit vulkan_swapchain( const VkSurfaceKHR& surface, VkExtent2D extent )
         : m_Extent( extent )
        {
            createSwapchain( surface );
        }
        vulkan_swapchain( const VkSurfaceKHR& surface, VkExtent2D extent, 
                          std::shared_ptr<vulkan_swapchain> oldSwapchain )
         : 
            m_Extent( extent ),
            m_OldSwapchain( oldSwapchain )
        {
            createSwapchain( surface );
            m_OldSwapchain = nullptr;
        }
        MUGGY_DISABLE_COPY_AND_MOVE( vulkan_swapchain );
        ~vulkan_swapchain() 
        {
            release( );
            assert( VK_NULL_HANDLE == m_Swapchain );
        }

        bool create( VkInstance instance );
        VkResult getNextImageIndex( const VkSemaphore& imageAvailable, 
                                    const VkFence& fence, uint32_t& currentIndex );
        VkImage& getCurrentImage( void );
        VkResult present( const VkQueue& queue, const VkSemaphore& renderFinished );

        // Validation of swapchain
        bool compareSwapchainFormats( const vulkan_swapchain& swapchain ) const
        {
            return swapchain.m_ImageFormat == m_ImageFormat;
        }
        
        // Get functions
        constexpr const VkSwapchainKHR& getSwapchain( void ) const { return m_Swapchain; }
        constexpr VkExtent2D getSwapchainExtent( void ) const { return m_Extent; }
        constexpr VkFormat getSwapchainImageFormat( void ) const { return m_ImageFormat; }
        constexpr uint32_t getCurrentImageIndex( void ) const { return m_CurrentImageIndex; }
        constexpr uint64_t getSwapchainImageCount( void ) const { return m_SwapChainImages.size(); }
        constexpr swapchain_image getSwapchainCurrentImage( void ) const { return m_SwapChainImages[ m_CurrentImageIndex ]; }
        constexpr swapchain_image getSwapchainImage( uint32_t index ) const { return m_SwapChainImages[ index ]; }
        constexpr vulkan_image getSwapchainDepthAttachment( void ) const { return m_DepthAttachment; }
        constexpr uint32_t getImageCount( void ) const { return m_ImageCount; }

    private:
        void release( void );
        bool createSwapchain( const VkSurfaceKHR& surface );


        VkSwapchainKHR                      m_Swapchain { VK_NULL_HANDLE };
        std::shared_ptr<vulkan_swapchain>   m_OldSwapchain { VK_NULL_HANDLE };
        swapchain_details                   m_SwapchainDetails { };
        VkExtent2D                          m_Extent{ };
        VkFormat                            m_ImageFormat { };
        uint32_t                            m_ImageCount { 0 };
        utils::vector<swapchain_image>      m_SwapChainImages { };
        vulkan_image                        m_DepthAttachment { };
        uint32_t                            m_CurrentImageIndex{ 0 };
    };

    class vulkan_surface
    {
    public:
        explicit vulkan_surface( platform::window window )
         : m_Window( window )
        {
            assert( m_Window.handle() );
        }
        MUGGY_DISABLE_COPY_AND_MOVE( vulkan_surface );
        ~vulkan_surface() 
        {
            release( );
            assert( nullptr == m_Swapchain && 
                    VK_NULL_HANDLE == m_Surface );
        }

        bool create( VkInstance instance );
        bool getNextImage( const VkSemaphore& imageAvailable,
                           const VkFence& fence );
        VkImage& getCurrentImage( void );
        bool present( const VkQueue& queue, const VkSemaphore& renderFinshed );
        void beginRenderPass( const VkCommandBuffer& cmdBuffer );
        void endRenderPass( const VkCommandBuffer& cmdBuffer );

        // Set functions
        void setResizeRequested( void ) { m_IsResized = true; }
        void setMinimized( void ) { m_IsMinimized = true; }
        void clearMinimized( void ) { m_IsMinimized = false; }
        void setViewPort( VkViewport viewport );
        void setScissor( VkRect2D scissor );
        // TODO(klek): I want these functions to be constexpr,
        //             so we need to make the math functions into
        //             literal types
        /*constexpr*/ void setRenderArea( math::u32v4d renderArea );
        /*constexpr*/ void setClearColor( math::fv4d clearColor );

        // Get functions
        uint32_t getWidth( void ) const { return m_Extent.width; }
        uint32_t getHeight( void ) const { return m_Extent.height; }
        constexpr const VkFramebuffer& getFrameBuffer( uint32_t index );
        // constexpr const VkRenderPass& getRenderPass( void ) const { return m_Renderpass.renderPass; }

        // Test functions
        constexpr bool isMinimized( void ) const { return m_IsMinimized; }
        constexpr bool isResizeRequested( void ) const { return m_IsResized; }

    private:
        void release( void );
        bool reCreateSwapchain( void );
        bool createSurface( VkInstance instance );
        bool createRenderpass( void );
        bool reCreateFrameBuffers( void );

        platform::window                    m_Window { };
        VkSurfaceKHR                        m_Surface { VK_NULL_HANDLE };
        std::unique_ptr<vulkan_swapchain>   m_Swapchain { nullptr };
        VkExtent2D                          m_Extent{ };
        vulkan_renderpass                   m_Renderpass { };
        utils::vector<vulkan_framebuffer>   m_FrameBuffers { };
        VkViewport                          m_Viewport{ };
        VkRect2D                            m_Scissor{ };
        uint32_t                            m_CurrentImageIndex{ 0 };
        bool                                m_IsMinimized { false };
        bool                                m_IsResized { false };
        bool                                m_IsFrameStarted{ false };
    };

    //****************************************************************
    // Helper functions
    bool getSwapchainDetails( VkPhysicalDevice device,
                              VkSurfaceKHR surface,
                              swapchain_details& details );

} // namespace muggy::graphics::vulkan


#endif