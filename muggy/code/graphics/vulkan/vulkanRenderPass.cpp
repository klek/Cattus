//********************************************************************
//  File:    vulkanRenderPass.cpp
//  Date:    Mon, 18 Mar 2024: 19:47
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanRenderPass.h"
#include "vulkanHelpers.h"

namespace muggy::graphics::vulkan::renderpass
{
    namespace
    {
        // Local helper functions
        VkAttachmentDescription setColorAttachment( VkFormat imageFormat )
        {
            // TODO(klek): This is being hardcoded in, but is likely
            //             something that should be configurable
            VkAttachmentDescription desc{ };
            desc.flags = 0;
            desc.format = imageFormat;
            desc.samples = VK_SAMPLE_COUNT_1_BIT;
            desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            // We do not expect any particular layout before renderpass starts
            desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            // Transitions to a present optimized layout after the renderpass
            desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            return desc;
        }

        VkAttachmentDescription setDepthAttachment( VkFormat depthFormat )
        {
            // TODO(klek): This is being hardcoded in, but is likely
            //             something that should be configurable as we
            //             might not always want a depth attachment
            VkAttachmentDescription desc{ };
            desc.flags = 0;
            desc.format = depthFormat;
            desc.samples = VK_SAMPLE_COUNT_1_BIT;
            desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            desc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            // We do not expect any particular layout before renderpass starts
            desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            // Transitions to a present optimized layout after the renderpass
            desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            return desc;
        }

        VkSubpassDependency setSubpassDependency( )
        {
            VkSubpassDependency dependency { };
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

            return dependency;
        }
    } // namespace

    bool createRenderPass( VkDevice device,
                           VkFormat swapchainImageFormat,
                           VkFormat depthFormat,
                           math::u32v4d renderArea,
                           math::fv4d clearColor,
                           float depth,
                           uint32_t stencil,
                           vulkan_renderpass& renderPass )
    {
        //************************************************************
        // Attachments
        // TODO(klek): There are other possible attachment types that
        //             needs to be configured here...
        //             input, resolve, preserve etc..
        // TODO(klek): These are hardcoded for now, but needs to be
        //             configurable in the future.
        //             They are hardcoded to 2 because of color and
        //             depth attachment
        constexpr uint32_t attachmentDescCount{ 2 };
        VkAttachmentDescription attachmentDesc[ attachmentDescCount ];

        // TODO(klek): Get inputAttachment...

        // Get the color attachment descriptor
        attachmentDesc[ 0 ] = setColorAttachment( swapchainImageFormat );

        // Setup the attachment reference, later used in the subpass
        VkAttachmentReference colorAttachRef;
        // Attachmentdesc[ 0 ] is color attachment
        colorAttachRef.attachment = 0;
        colorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // TODO(klek): Get resolveAttachment...

        // Get the depth attachment descriptor
        // TODO(klek): This is being hardcoded in, but is likely
        //             something that should be configurable as we
        //             might not always want a depth attachment
        attachmentDesc[ 1 ] = setDepthAttachment( depthFormat );

        // Setup the attachment reference, later used in the subpass
        VkAttachmentReference depthAttachRef{ };
        // attachmentDesc[1] is depth attachment
        depthAttachRef.attachment = 1;
        depthAttachRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // TODO(klek): Get preserveAttachment...

        //************************************************************
        // Create the main subpass
        VkSubpassDescription subPass{ };
        subPass.flags = 0;
        subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        // Input from shader
        // TODO(klek): Update this when it has been implemented
        subPass.inputAttachmentCount = 0;
        subPass.pInputAttachments = nullptr;
        // Let subpass know about the color attachment
        subPass.colorAttachmentCount = 1;
        subPass.pColorAttachments = &colorAttachRef;
        // Resolve attachments used for multisampling color attachments
        // not used..
        // TODO(klek): Update this when it has been implemented
        subPass.pResolveAttachments = nullptr;
        // Let subpass know about the depth attachment
        subPass.pDepthStencilAttachment = &depthAttachRef;
        // Preserve attachments are not used
        // TODO(klek): Update this when it has been implemented
        subPass.preserveAttachmentCount = 0;
        subPass.pPreserveAttachments = nullptr;

        //************************************************************
        // Renderpass dependencies
        // TODO(klek): This needs to be configurable
        utils::vector<VkSubpassDependency> dependencies {};
        dependencies.push_back( setSubpassDependency( ) );

        //************************************************************
        // Setup the renderpass create info
        VkRenderPassCreateInfo info{ };
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.pNext = 0;
        info.flags = 0;
        info.attachmentCount = attachmentDescCount;
        info.pAttachments = attachmentDesc;
        info.subpassCount = 1;
        info.pSubpasses = &subPass;
        info.dependencyCount = static_cast<uint32_t>( dependencies.size() );
        info.pDependencies = dependencies.data();

        //************************************************************
        // Setup the renderpass
        renderPass.renderArea = renderArea;
        renderPass.clearColor = clearColor;
        renderPass.depth = depth;
        renderPass.stencil = stencil;

        VkResult result { VK_SUCCESS };
        result = vkCreateRenderPass( device, &info, nullptr, &renderPass.renderPass );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create renderpass...");
            return false;
        }

        return true;
    }

    void destroyRenderPass( VkDevice device, vulkan_renderpass& renderPass )
    {
        if ( renderPass.renderPass ) 
        {
            vkDestroyRenderPass( device, renderPass.renderPass, nullptr );
            renderPass.renderPass = nullptr;
        }
    }

    void beginRenderPass( VkCommandBuffer cmdBuffer,
                          vulkan_cmd_buffer::state& state,
                          vulkan_renderpass& renderPass,
                          VkFramebuffer frameBuffer )
    {
        VkClearValue values[2]{ };
        values[0].color.float32[0] = renderPass.clearColor.x;
        values[0].color.float32[1] = renderPass.clearColor.y;
        values[0].color.float32[2] = renderPass.clearColor.z;
        //values[0].color.float32[3] = renderPass.clearColor.a;
        values[0].color.float32[3] = renderPass.clearColor.z;

        values[1].depthStencil.depth = renderPass.depth;
        values[1].depthStencil.stencil = renderPass.stencil;

        VkRenderPassBeginInfo info { };
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.pNext = nullptr;
        info.renderPass = renderPass.renderPass;
        info.framebuffer = frameBuffer;
        info.renderArea.offset.x = renderPass.renderArea.x;
        info.renderArea.offset.y = renderPass.renderArea.y;
        info.renderArea.extent.width = renderPass.renderArea.width;
        info.renderArea.extent.height = renderPass.renderArea.height;
        info.clearValueCount = 2;
        info.pClearValues = values;

        // Start the render pass
        vkCmdBeginRenderPass( cmdBuffer, &info, VK_SUBPASS_CONTENTS_INLINE );
        state = vulkan_cmd_buffer::CMD_IN_RENDER_PASS;
    }

    void beginRenderPass( const VkCommandBuffer& cmdBuffer, 
                          const vulkan_renderpass& renderPass,
                          const VkFramebuffer& frameBuffer )
    {
        VkClearValue values[2]{ };
        values[0].color.float32[0] = renderPass.clearColor.x;
        values[0].color.float32[1] = renderPass.clearColor.y;
        values[0].color.float32[2] = renderPass.clearColor.z;
        //values[0].color.float32[3] = renderPass.clearColor.a;
        values[0].color.float32[3] = renderPass.clearColor.z;

        values[1].depthStencil.depth = renderPass.depth;
        values[1].depthStencil.stencil = renderPass.stencil;

        VkRect2D renderArea { };
        renderArea.offset.x = renderPass.renderArea.x;
        renderArea.offset.y = renderPass.renderArea.y;
        renderArea.extent.width = renderPass.renderArea.width;
        renderArea.extent.height = renderPass.renderArea.height;
        VkRenderPassBeginInfo info {
            vulkan_utils::setRenderpassBeginInfo( renderPass.renderPass,
                                                  frameBuffer,
                                                  renderArea )
        };
        info.clearValueCount = 2;
        info.pClearValues = values;

        // Start the render pass
        vkCmdBeginRenderPass( cmdBuffer, &info, VK_SUBPASS_CONTENTS_INLINE );
    }

    void endRenderPass( VkCommandBuffer cmdBuffer,
                        vulkan_cmd_buffer::state& state,
                        vulkan_renderpass& renderPass )
    {
        vkCmdEndRenderPass( cmdBuffer );
        state = vulkan_cmd_buffer::CMD_RECORDING;
    }
    
    void endRenderPass( const VkCommandBuffer& cmdBuffer )
    {
        vkCmdEndRenderPass( cmdBuffer );
    }

} // namespace muggy::graphics::vulkan::renderpass
