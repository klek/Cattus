//********************************************************************
//  File:    fileUtils.cpp
//  Date:    Wed, 20 Mar 2024: 19:18
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "fileUtils.h"
#include <filesystem>

namespace muggy::utils
{
    bool readFileBinary( const std::string& path, std::unique_ptr<uint8_t[]>& data, uint64_t& size )
    {
        // Check if the path exists
        std::ifstream file( path, std::ios::ate | std::ios::binary );

        if( !file )
        {
            // Failed to open file
            // Close it and return false
            file.close();
            return false;
        }

        // Determine the size of the file
        size = (size_t) file.tellg();

        if ( size == 0 )
        {
            // File is empty
            // Close it and return false
            file.close();
            return false;
        }

        // Allocate memory for the data in the file
        data = std::make_unique<uint8_t[]>( size );
        // Set the read position of the file to start
        file.seekg( 0, std::ios::beg );
        if ( !file.read( (char*)data.get(), size ) )
        {
            // Failed to read file
            // Close it and return
            file.close();
            return false;
        }

        // Data should now be in data
        // Close the file and return true
        file.close();
        return true;
    }

    bool writeFileBinary( const std::string& path, std::unique_ptr<uint8_t[]>& data, uint64_t& size )
    {
        return false;
    }

    bool readShaderBlob( const std::filesystem::path& path,
                         std::unique_ptr<uint8_t[]>& data,
                         uint64_t& size )
    {
        // Check where we are
        auto pathOnSystem = std::filesystem::current_path();
        std::cout << pathOnSystem << std::endl;
        // If file exists we try to read from it
        if ( std::filesystem::exists( path ) )
        {
            // Use readFileBinary
            return readFileBinary( path.c_str(), data, size );
        }
        return false;
    }

    bool writeShaderToBlob( const std::filesystem::path& path, 
                            const uint8_t* data, 
                            uint64_t size, 
                            bool overWrite )
    {
        // If file exists, we append to the existing file
        // If file does not exist, we create a new file
        if ( !path.empty() && data && ( size > 0 ) )
        {
            // Create the potential directory
            std::filesystem::create_directory( path.parent_path() );
            std::ofstream file;

            // Try to open the file
            if ( overWrite || !std::filesystem::exists( path ) )
            {
                // Open in overwrite mode
                file.open( path, std::ios::out | std::ios::binary );
            }
            else 
            {
                // Open in append mode
                file.open( path, std::ios::out | std::ios::binary | std::ios::app );
            }

            if ( !file )
            {
                file.close();
                return false;
            }

            // Write size and data
            file.write( (char*)&size, sizeof( size ) );
            file.write( (char*)data, size );

            // Close file and return
            file.close();

            return true;
        }
        
        // Either path/data is null or the size to write is 0
        return false;
    }
    
} // namespace muggy::utils
