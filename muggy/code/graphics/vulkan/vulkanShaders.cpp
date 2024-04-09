//********************************************************************
//  File:    vulkanShaders.cpp
//  Date:    Thu, 21 Mar 2024: 16:23
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanShaders.h"
#include "../../utilities/fileUtils.h"

namespace muggy::graphics::vulkan::shaders
{
    namespace 
    {
        struct engine_shader_info
        {
            const char*         path;
            const char*         function;
            engine_shader::id   id;
            shader_type::type   type;
        };

        // A structure that defines how each compiled shader is stored
        // in the shadersBlob
        typedef struct compiled_shader
        {
            uint64_t        size;
            const uint8_t*  byteCode;
        } const *compiled_shader_ptr;

        // This array holds information about available vulkan 
        // engine shaders
        engine_shader_info shaderInfo[]
        {
            { "triangleShader.vert", "TriangleVS", engine_shader::triangleShaderVS , shader_type::vertex },
            { "triangleShader.frag", "TriangleFS", engine_shader::triangleShaderFS , shader_type::fragment }
        };
        static_assert( _countof( shaderInfo ) == engine_shader::count );
        constexpr const char* shaderSrcFolder{ 
            "../../../muggy/code/graphics/shaders/" 
        };

        // Get the paths for compiled shader location
        // These paths are relative to the engine-binary
        constexpr const char* engineShaderPaths[2]{
            "../resources/vert.spv",
            "../resources/frag.spv"
        };

        // Each element in this array points to an offset within the 
        // shadersBlob
        compiled_shader_ptr engineShaders[ engine_shader::count ]{ };

        // This is a chunk of memory that contains all the compiled
        // engine shaders in the form of a blob. The blob is 
        // essentially an array of size+byteCode (the compiled_shader
        // struct above)
        std::unique_ptr<uint8_t[]>  shadersBlob { };

        bool compiledShadersUpToDate( void )
        {
            // Get path to blob
            const char* shadersBlobPath = getEngineShadersPath();
            // Check that the blob file exists
            if ( !FILE_EXISTS( shadersBlobPath ) )
            {
                return false;
            }
            // Get the blob's last compile time
            auto shaderBlobCompileTime = FILE_GET_WRITE_TIME( shadersBlobPath );

            // Now we need to check this time compared to each shader-
            // source files last write time
            auto path = FILE_CREATE_PATH();
            auto fullPath = FILE_CREATE_PATH();
            for ( uint32_t i { 0 }; i < engine_shader::count; i++ )
            {
                // Grab the first shader source
                const engine_shader_info& info{ shaderInfo[ i ] };
                path = shaderSrcFolder;
                path += info.path;
                //fullPath = FILE_FULL_PATH( path );
                fullPath = path;
                auto shaderFileTime = FILE_GET_WRITE_TIME( fullPath );
                if ( shaderFileTime > shaderBlobCompileTime ) 
                {
                    return false;
                }
            }

            // If we exit the loop then compiled shader blob is up to date
            return true;
        }

        bool saveCompiledShadersToBlob( utils::vector<vulkan_shader_struct>& shaders )
        {
            // Get path to save to
            const char* shaderBlobPath = getEngineShadersPath();
            uint32_t i { 0 };

            // Save the first shader with overwrite to the blob
            if ( !utils::writeShaderToBlob( shaderBlobPath, 
                                            (const uint8_t*)shaders[ i ].byteCode, 
                                            shaders[ i ].size, 
                                            true ) )
            {
                return false;
            }
            i++;
            // Save the rest of the shaders
            for ( ; i < shaders.size(); i++ )
            {
                if ( !utils::writeShaderToBlob( shaderBlobPath,
                                                (const uint8_t*)shaders[ i ].byteCode,
                                                shaders[ i ].size ) )
                {
                    return false;
                }
            }

            // Well we reached end
            return true;
        }

        bool loadEngineShadersBlob( std::unique_ptr<uint8_t[]>& blob, uint64_t& blobSize )
        {
            // Does the blob exists on disk? Try to read it
            auto shaderBlobPath = getEngineShadersPath();
            bool result { utils::readShaderBlob( shaderBlobPath, blob, blobSize ) };
            if ( result )
            {
                // Nice, it worked
                return result;
            }

            // Well, the blob did not exist, try to create it from compiled
            // shaders
            // Get engineShadersPaths
            for ( uint32_t i { 0 }; i < _countof( engineShaderPaths ); i++)
            {
                std::unique_ptr<uint8_t[]> shaderData;
                uint64_t size { 0 };
                result = utils::readFileBinary( engineShaderPaths[i], shaderData, size);
                if ( !result )
                {
                    // If we cannot read the binary, should we give up?
                    break;
                }
                // Now we write this data to the shaderBlob
                result = utils::writeShaderToBlob( shaderBlobPath, shaderData.get(), size );
                if ( !result )
                {
                    // If we cannot write the shader to file, should we give up?
                    break;
                }
            }

            if ( !result )
            {
                // TODO(klek): Call compile shaders here
                return false;
            }

            // Now we need to reload the blob
            result = utils::readShaderBlob( shaderBlobPath, blob, blobSize );
            if ( !result )
            {
                // Nice, it worked
                return result;
            }
            return true;
        }

        bool loadEngineShaders( void )
        {
            assert( !shadersBlob );
            uint64_t blobSize { 0 };
            bool result { loadEngineShadersBlob( shadersBlob, blobSize ) };
            assert( shadersBlob && blobSize );

            // Iterate through the blob of shaders and find the offset
            // to each shader. Then set this offset for the 
            // corresponding slot in engineShaders
            uint64_t offset { 0 };
            uint32_t index { 0 };
            while ( offset < blobSize && result )
            {
                assert( index < engine_shader::count );
                // Get a reference to the current slot in engineShaders
                compiled_shader_ptr& shader { engineShaders[ index ] };
                assert( !shader );
                // Check to see that this is an actual slot in 
                // engineShaders
                result &= ( index < engine_shader::count ) && !shader;
                if ( !result )
                {
                    break;
                }
                // Cast the current offset in shadersBlob to the shader
                // effectively storing its offset in engineShaders array
                shader = reinterpret_cast<const compiled_shader_ptr>( &shadersBlob[ offset ] );
                // Calculate offset to next shader in shadersBlob
                offset += sizeof( uint64_t ) + shader->size;
                index++;
            }
            assert( offset == blobSize && index == engine_shader::count );
            return true;
        }
    } // namespace anonymous
    
    bool compileShaders(  )
    {
        // Are compiled shaders up to date?
        // Is the compiled shaders newer than shader source?
        if ( compiledShadersUpToDate() )
        {
            return true;
        }

        // Create a buffer to hold shaders
        utils::vector<vulkan_shader_struct> shaders;

        // Compile shaders and add them to the buffer in the same order
        // they are compiled
        for ( uint32_t i { 0 }; i < engine_shader::count; i++ )
        {
            // TODO(klek): Implement this
        }

        // Save shaders to binary blob
        return saveCompiledShadersToBlob( shaders );
    }

    bool createShaderModule( VkDevice device,
                             VkShaderModule& shaderModule,
                             engine_shader::id id )
    {
        assert( id < engine_shader::count );
        // Get the requested shader from engineShaders
        compiled_shader_ptr& shader { engineShaders[ id ] };

        //utils::vector<uint8_t> data ( shader->size );
        utils::vector<uint8_t> data;
        // Copy shader code into our vector
        for ( uint64_t i { 0 }; i < shader->size; i++ )
        {
            //data[ i ] = shader->byteCode[ i ];
            const uint8_t* testPtr = (const uint8_t*)shader;
            data.emplace_back( testPtr[ i + sizeof( shader->size ) ] );
        }

        // Create the info struct
        VkShaderModuleCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = data.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>( data.data() );

        if ( vkCreateShaderModule( device, &createInfo, nullptr, &shaderModule ) != VK_SUCCESS )
        {
            MSG("Failed to create shader module...");
            return false;
        }

        MSG("Created shader module!");

        return true;
    }

    bool initialize( void )
    {
        return loadEngineShaders();
    }

    void shutdown( void )
    {
        // Clear the offset pointers in engineShaders
        for ( uint32_t i { 0 }; i < engine_shader::count; i++ )
        {
            engineShaders[ i ] = { };
        }
        // Release the memory for the unique pointer
        shadersBlob.reset();
    }

    vulkan_shader_struct getEngineShader( engine_shader::id id )
    {
        assert( id < engine_shader::count );
        const compiled_shader_ptr shader { engineShaders[ id ] };
        assert( shader && shader->size );
        vulkan_shader_struct result { };
        //result.byteCode = &shader->byteCode;
        return { &shader->byteCode, shader->size };
    }
} // namespace muggy::graphics::vulkan::shaders
