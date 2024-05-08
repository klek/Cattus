//********************************************************************
//  File:    vec4dtemplate.h
//  Date:    Sun, 19 Nov 2023: 00:23
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VEC_4D_TEMPLATE_H)
#define VEC_4D_TEMPLATE_H

#include <iostream>

namespace muggy::math
{
    template <typename T>
    struct vec4dTemplate
    {
        typedef T vType;
        typedef vec4dTemplate<T> vec4Type;

        // Member variables
        union 
        {
            struct{ 
                vType x, y, z, w; 
            };
            struct{ 
                vType r, g, b, a; 
            };
            struct{
                vType left, top, right, bottom;
            };
            struct{
                vType xPos, yPos, width, height;
            };
        };
        
        // Constructors
        constexpr vec4dTemplate();
        constexpr vec4dTemplate( const vType& _s );
        constexpr vec4dTemplate( const vType& _x, 
                                 const vType& _y, 
                                 const vType& _z, 
                                 const vType& _w );
        constexpr vec4dTemplate( const vec4Type& _v );
        constexpr explicit vec4dTemplate( const vType (&_arr)[4] );

        // Member functions
        // Implement basic math functions for 4D vectors
        constexpr vec4Type& add( const vec4Type& other );
        constexpr vec4Type& subtract( const vec4Type& other );
        constexpr vec4Type& multiply( const vec4Type& other );
        constexpr vec4Type& divide( const vec4Type& other );

        // Math operators overload
        // NOTE(klek): Simply calls above math functions
        constexpr vec4Type& operator+=( const vec4Type& other );
        constexpr vec4Type& operator-=( const vec4Type& other );
        constexpr vec4Type& operator*=( const vec4Type& other );
        constexpr vec4Type& operator/=( const vec4Type& other );

        // Boolean operators overload
        constexpr bool operator==( const vec4Type other ) const;
        constexpr bool operator!=( const vec4Type other ) const;

        //************************************************************
        // Friend functions, could probably be outside of the struct
        template <typename Y>
        friend constexpr vec4Type operator+( vec4Type left, const vec4dTemplate<Y>& right );
        template <typename Y>
        friend constexpr vec4Type operator-( vec4Type left, const vec4dTemplate<Y>& right );
        template <typename Y>
        friend constexpr vec4Type operator*( vec4Type left, const vec4dTemplate<Y>& right );
        template <typename Y>
        friend constexpr vec4Type operator/( vec4Type left, const vec4dTemplate<Y>& right );

        // Output operators, overloaded
        template <typename Y>
        friend std::ostream& operator<<( std::ostream& stream, const vec4dTemplate<Y>& v );
    };
} // namespace muggy::math

#ifndef USE_MATH_EXTERNAL
#define INCLUDE_VEC4_CPP            1
#include "vec4dTemplate.cpp"
#undef INCLUDE_VEC4_CPP
#endif

#endif