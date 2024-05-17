//********************************************************************
//  File:    maths_funcs.h
//  Date:    Sun, 22 Oct 2023: 23:29
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(MATHS_FUNCS_H)
#define MATHS_FUNCS_H

namespace muggy::math
{
    //
    // Defining sine function and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_sin( T x );
//    float maths_sin(float x);
//    long double maths_sin(long double x);

    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_cos( T x );
//    float maths_cos(float x);
//    long double maths_cos(long double x);

    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_tan( T x );
//    float maths_tan(float x);
//    long double maths_tan(long double x);

    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_abs( T x );

    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_sqrt( T x );

    //
    // Defining a toRadians function
    //
    template <typename T>
    constexpr inline T toRadians( T degrees );
}

#ifndef USE_MATH_EXTERNAL
#define INCLUDE_MATHS_FUNCS_CPP            1
#include "maths_funcs.cpp"
#undef INCLUDE_MATHS_FUNCS_CPP
#endif

#endif