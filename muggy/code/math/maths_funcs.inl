//********************************************************************
//  File:    maths_funcs.inl
//  Date:    Sat, 18 Nov 2023: 00:22
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

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
        return std::sin( x ); 
    }

    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_cos( T x )
    {
        return std::cos( x ); 
    }

    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_tan( T x )
    {
        return std::tan( x ); 
    }
    
    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_abs( T x )
    {
        return std::abs( x );
    }

    //
    // Defining cosine functions and overloads
    //
    // TODO(klek): Consider adding own implementations for these functions
    template <typename T>
    constexpr inline T maths_sqrt( T x )
    {
        return std::sqrt( x );
    }

    //
    // Defining a toRadians function
    //
    template <typename T>
    constexpr inline T toRadians( T degrees )
    {
        return degrees * T( ( MUGGY_PI / 180.0f ) );
    }

    //
    // Defining a toRadians function
    //
    template <typename T>
    constexpr inline T toDegrees( T radians )
    {
        return radians * T( ( 180.0f / MUGGY_PI ) );
    }

    //
    // Checks if value is approximate zero
    //
    template <typename T>
    constexpr inline bool isApproxZero( T val )
    {
        return ( maths_abs( val ) < T( MUGGY_EPSILON ) );
    }

} // namespace muggy:math
