//********************************************************************
//  File:    vec2dTemplate.h
//  Date:    Sun, 19 Nov 2023: 02:23
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VEC_2D_TEMPLATE_H)
#define VEC_2D_TEMPLATE_H

#include <iostream>

namespace muggy::math
{
    template <typename T>
    struct vec2dTemplate
    {
        typedef T vType;
        typedef vec2dTemplate<T> vec2Type;

        // Member variables
        union 
        {
            struct{ 
                vType x, y; 
            };
            struct{ 
                vType a, b; 
            };
            struct{ 
                vType left, top; 
            };
        };
        
        // Constructors
        constexpr vec2dTemplate();
        constexpr vec2dTemplate( const vType& _s );
        constexpr vec2dTemplate( const vType& _x, 
                                 const vType& _y );
        constexpr vec2dTemplate( const vec2Type& _v );
        constexpr vec2dTemplate( const vType (&_arr)[2] );

        // Member functions
        // Implement basic math functions for 2D vectors
        constexpr vec2Type& add( const vec2Type& other );
        constexpr vec2Type& subtract( const vec2Type& other );
        constexpr vec2Type& multiply( const vec2Type& other );
        constexpr vec2Type& divide( const vec2Type& other );

        // Math operators overload
        // NOTE(klek): Simply calls above math functions
        constexpr vec2Type& operator+=( const vec2Type& other );
        constexpr vec2Type& operator-=( const vec2Type& other );
        constexpr vec2Type& operator*=( const vec2Type& other );
        constexpr vec2Type& operator/=( const vec2Type& other );

        // Boolean operators overload
        constexpr bool operator==( const vec2Type other ) const;
        constexpr bool operator!=( const vec2Type other ) const;

        //************************************************************
        // Friend functions, could probably be outside of the struct
        template <typename Y>
        friend constexpr vec2Type operator+( vec2Type left, const vec2dTemplate<Y>& right );
        template <typename Y>
        friend constexpr vec2Type operator-( vec2Type left, const vec2dTemplate<Y>& right );
        template <typename Y>
        friend constexpr vec2Type operator*( vec2Type left, const vec2dTemplate<Y>& right );
        template <typename Y>
        friend constexpr vec2Type operator/( vec2Type left, const vec2dTemplate<Y>& right );

        // Output operators, overloaded
        template <typename Y>
        friend std::ostream& operator<<( std::ostream& stream, const vec2dTemplate<Y>& v );
    };
} // namespace muggy::math

#ifndef USE_MATH_EXTERNAL
#define INCLUDE_VEC2_CPP            1
#include "vec2dTemplate.cpp"
#undef INCLUDE_VEC2_CPP
#endif

#endif