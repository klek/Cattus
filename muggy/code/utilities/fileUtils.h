//********************************************************************
//  File:    fileUtils.h
//  Date:    Wed, 20 Mar 2024: 19:12
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(FILE_UTILS_H)
#define FILE_UTILS_H

#include "../common/common.h"
#include <fstream>

#if defined( _WIN64 )
#include <filesystem>
#define FILE_EXISTS( path )         std::filesystem::exists( path )
#define FILE_WRITE_TIME( path )     std::filesystem::last_write_time( path )
#elif defined( __linux__ )
#include <filesystem>
#define FILE_CREATE_PATH( var )     std::filesystem::path( var )
#define FILE_EXISTS( path )         std::filesystem::exists( path )
#define FILE_GET_WRITE_TIME( path ) std::filesystem::last_write_time( path )
#define FILE_FULL_PATH( path )      std::filesystem::absolute( std::filesystem::path( path ) )
#else
#warning "File-utils is not supported on this platform"
#endif

namespace muggy::utils
{
    bool readFileBinary( const std::string& path, std::unique_ptr<uint8_t[]>& data, uint64_t& size );
    bool writeFileBinary( const std::string& path, std::unique_ptr<uint8_t[]>& data, uint64_t& size );

    bool readShaderBlob( const std::filesystem::path& path,
                         std::unique_ptr<uint8_t[]>& data,
                         uint64_t& size );
    bool writeShaderToBlob( const std::filesystem::path& path, 
                            const uint8_t* data, 
                            uint64_t size, 
                            bool overWrite = false);
} // namespace muggy::utils


#endif