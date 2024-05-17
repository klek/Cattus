//********************************************************************
//  File:    math.h
//  Date:    Sat, 21 Oct 2023: 22:03
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(MATH_H)
#define MATH_H

#include "maths_funcs.h"
#include "vecTemplate.h"
#include "mat4Template.h"

namespace muggy::math
{
    //****************************************************************
    // Typedefines for standard 4x4 matrices
    // NOTE(klek): It does not make much sense to have a matrix of 
    //             other datatypes than float, due to the calculations
    //             done in member functions
    typedef mat4Template<float>         fmat4;
    //****************************************************************

    //****************************************************************
    // Typedefines for standard 4D vectors
    typedef vec4dTemplate<float>        fv4d;

    // Unsigned integers, 8-bit to 64-bit
    typedef vec4dTemplate<uint8_t>      u8v4d;
    typedef vec4dTemplate<uint16_t>     u16v4d;
    typedef vec4dTemplate<uint32_t>     u32v4d;
    typedef vec4dTemplate<uint64_t>     u64v4d;

    // Signed integers, 8-bit to 64-bit
    typedef vec4dTemplate<int8_t>       i8v4d;
    typedef vec4dTemplate<int16_t>      i16v4d;
    typedef vec4dTemplate<int32_t>      i32v4d;
    typedef vec4dTemplate<int64_t>      i64v4d;
    //****************************************************************

    //****************************************************************
    // Typedefines for standard 3D vectors
    typedef vec3dTemplate<float>        fv3d;

    // Unsigned integers, 8-bit to 64-bit
    typedef vec3dTemplate<uint8_t>      u8v3d;
    typedef vec3dTemplate<uint16_t>     u16v3d;
    typedef vec3dTemplate<uint32_t>     u32v3d;
    typedef vec3dTemplate<uint64_t>     u64v3d;

    // Signed integers, 8-bit to 64-bit
    typedef vec3dTemplate<int8_t>       i8v3d;
    typedef vec3dTemplate<int16_t>      i16v3d;
    typedef vec3dTemplate<int32_t>      i32v3d;
    typedef vec3dTemplate<int64_t>      i64v3d;
    //****************************************************************

    //****************************************************************
    // Typedefines for standard 2D vectors
    typedef vec2dTemplate<float>        fv2d;

    // Unsigned integers, 8-bit to 64-bit
    typedef vec2dTemplate<uint8_t>      u8v2d;
    typedef vec2dTemplate<uint16_t>     u16v2d;
    typedef vec2dTemplate<uint32_t>     u32v2d;
    typedef vec2dTemplate<uint64_t>     u64v2d;

    // Signed integers, 8-bit to 64-bit
    typedef vec2dTemplate<int8_t>       i8v2d;
    typedef vec2dTemplate<int16_t>      i16v2d;
    typedef vec2dTemplate<int32_t>      i32v2d;
    typedef vec2dTemplate<int64_t>      i64v2d;
    //****************************************************************

    //****************************************************************
    // Generic objects
    typedef i32v4d                      RECT;
    typedef i32v3d                      TRIANGLE;
    typedef i32v2d                      POINT;

    // Floating point objects
    typedef quaternion<float>           fquat;
    
} // namespace muggy::math


#endif