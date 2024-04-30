//********************************************************************
//  File:    vulkanSurface.h
//  Date:    Fri, 15 Mar 2024: 20:33
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_SURFACE_H)
#define VULKAN_SURFACE_H

#define USE_OLD_SURFACE         0

#if USE_OLD_SURFACE
#include "vulkanCommon.h"


#if USE_STL_VECTOR
#define CONSTEXPR               
#else
#define CONSTEXPR               constexpr
#endif

namespace muggy::graphics::vulkan
{
    struct swapchain_details 
    {
        // Surface properties, ie image size/extent
        VkSurfaceCapabilitiesKHR            surfaceCapabilities;
        // Surface image formats supported, ie RGBA, size of each color
        utils::vector<VkSurfaceFormatKHR>   formats;
        // How images should be presented to the screen
        utils::vector<VkPresentModeKHR>     presentationModes;
    };

    struct swapchain_image
    {
        VkImage                             image;
        VkImageView                         imageView;
    };

    struct vulkan_swapchain
    {
        swapchain_details                   details;
        VkSwapchainKHR                      swapchain;
        VkFormat                            imageFormat;
        VkExtent2D                          extent;
        utils::vector<swapchain_image>      images;
        vulkan_image                        depthAttachment;
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
        ~vulkan_surface( )
        {
            release();
        }

        void create( VkInstance instance );
        void present( VkSemaphore imageAvailable, 
                      VkSemaphore renderFinished, 
                      VkFence fence, 
                      VkQueue presentationQueue );
        void resize( );
        bool reCreateSwapchain();
        bool nextImageIndex( VkSemaphore imageAvailable, 
                             VkFence fence, 
                             uint64_t timeout );
        // TODO(klek): I want these functions to be constexpr,
        //             so we need to make the math functions into
        //             literal types
        /*constexpr*/ void setRenderpassRenderArea( math::u32v4d renderArea )
        {
            m_Renderpass.renderArea = renderArea;
        }
        /*constexpr*/ void setRenderPassClearColor( math::fv4d clearColor )
        {
            m_Renderpass.clearColor = clearColor;
        }

        // Get functions for accessing certain member variables
        [[nodiscard]] CONSTEXPR VkFramebuffer& getCurrentFrameBuffer( void )
        {
            return m_FrameBuffers[ m_ImageIndex ].frameBuffer;
        }
        [[nodiscard]] CONSTEXPR vulkan_renderpass& getRenderpass( void )
        {
            return m_Renderpass;
        }
        uint32_t getWidth( void ) const { return m_Window.getWidth(); }
        uint32_t getHeight( void ) const { return m_Window.getHeight(); }
        constexpr uint32_t getCurrentFrame( void ) const { return m_FrameIndex; }
        constexpr VkPipeline& getGraphicsPipeline( void ) { return m_GraphicsPipeline; }

        // Check functions, to know status of swap chain and frame buffer
        constexpr bool isReCreating( void ) const { return m_IsRecreating; }
        constexpr bool isResized( void ) const { return m_IsFrameBufferResized; }
        constexpr bool isMinimized( void ) const { return false; }

    private:
        bool createSurface( VkInstance instance );
        bool createRenderpass( void );
        bool createSwapchain( void );
        bool reCreateFramebuffers( void );
        void cleanSwapchain( void );
        void release( void );

        VkSurfaceKHR                        m_Surface{};
        vulkan_swapchain                    m_Swapchain{};
        vulkan_renderpass                   m_Renderpass{};
        utils::vector<vulkan_framebuffer>   m_FrameBuffers{};
        platform::window                    m_Window{};
        bool                                m_IsFrameBufferResized{ false };
        bool                                m_IsRecreating{ false };
        uint32_t                            m_ImageIndex { 0 };
        uint32_t                            m_FrameIndex { 0 };
        VkPipeline                          m_GraphicsPipeline { 0 };
        VkPipelineLayout                    m_PipelineLayout { 0 };
    };

    bool getSwapchainDetails( VkPhysicalDevice device,
                              VkSurfaceKHR surface,
                              swapchain_details& details );
} // namespace muggy::graphics::vulkan

#undef CONSTEXPR

#endif

#endif