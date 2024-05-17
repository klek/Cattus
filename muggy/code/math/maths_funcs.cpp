//********************************************************************
//  File:    maths_funcs.cpp
//  Date:    Sat, 18 Nov 2023: 00:22
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************
#ifdef INCLUDE_MATHS_FUNCS_CPP

#include "maths_funcs.h"
#include <cmath>
#include "mathTypes.h"

namespace muggy::math
{
    //
    // Defining sine function and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_sin( T x ) 
    { 
        return sin( x ); 
    }

    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_cos( T x )
    {
        return cos( x ); 
    }

    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_tan( T x )
    {
        return tan( x ); 
    }
    
    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_abs( T x )
    {
        return abs( x );
    }

    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_sqrt( T x )
    {
        return sqrt( x );
    }

    //
    // Defining a toRadians function
    //
    template <typename T>
    constexpr inline T toRadians( T degrees )
    {
//        return degrees * ( M_PI / T( 180.0f ) );
        return degrees * ( MUGGY_PI / T( 180.0f ) );
    }

} // namespace muggy:math

#endif