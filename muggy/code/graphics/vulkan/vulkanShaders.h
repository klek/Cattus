//********************************************************************
//  File:    vulkanShaders.h
//  Date:    Thu, 21 Mar 2024: 16:22
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_SHADERS_H)
#define VULKAN_SHADERS_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan::shaders
{
    struct shader_type
    {
        enum type : uint32_t
        {
            vertex = 0,
            tesselation,
            geometry,
            fragment,

            count
        };

    };

    struct engine_shader
    {
        enum id : uint32_t
        {
            triangleShaderVS = 0,
            triangleShaderFS,
            mandelBrotFS,

            count
        };
    };

    typedef struct vulkan_shader_struct
    {
        const void*     byteCode;
        uint64_t        size;
    } vulkan_shader_struct;

    // What functionality do I want?
    // Read shader from source, the glsl code directly
    bool readShaderFromSource( const char* srcPath );
    // Pass the glsl code directly to shaderc 
    // Compile shader and generate a byte array
    bool compileShaderFromSource( const char* dest, 
                                  std::unique_ptr<uint8_t>& data,
                                  uint64_t size );
    // Read a shader that has been compiled, in the form of a byte
    // array, and then return a byte array
    bool readShaderFromFile( const char* src,
                             std::unique_ptr<uint8_t>& data,
                             uint64_t& size );

    // Create a shader module object from a compiled shader 
    bool createShaderModule( VkDevice device,
                             engine_shader::id id,
                             VkShaderModule& shaderModule );

    bool initialize( void );
    void shutdown( void );
    vulkan_shader_struct getEngineShader( engine_shader::id id );
} // namespace muggy::graphics::vulkan::shaders


#endif