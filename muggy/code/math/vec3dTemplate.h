//********************************************************************
//  File:    vec3dTemplate.h
//  Date:    Sun, 19 Nov 2023: 02:46
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VEC_3D_TEMPLATE_H)
#define VEC_3D_TEMPLATE_H

#include <iostream>

namespace muggy::math
{
    template <typename T>
    struct vec3dTemplate
    {
        typedef T vType;
        typedef vec3dTemplate<T> vec3Type;

        // Member variables
        union 
        {
            struct{ 
                vType x, y, z; 
            };
            struct{ 
                vType r, g, b;
            };
            struct{
                vType left, top, right;
            };
        };
        
        // Constructors
        constexpr vec3dTemplate();
        constexpr vec3dTemplate( const vType& _s );
        constexpr vec3dTemplate( const vType& _x, 
                                 const vType& _y, 
                                 const vType& _z );
        constexpr vec3dTemplate( const vec3Type& _v );
        constexpr explicit vec3dTemplate( const vType (&_arr)[3] );

        // Member functions
        // Implement basic math functions for 3D vectors
        constexpr vec3Type& add( const vec3Type& other );
        constexpr vec3Type& subtract( const vec3Type& other );
        constexpr vec3Type& multiply( const vec3Type& other );
        constexpr vec3Type& divide( const vec3Type& other );

        // Math operators overload
        // NOTE(klek): Simply calls above math functions
        constexpr vec3Type& operator+=( const vec3Type& other );
        constexpr vec3Type& operator-=( const vec3Type& other );
        constexpr vec3Type& operator*=( const vec3Type& other );
        constexpr vec3Type& operator/=( const vec3Type& other );

        // Boolean operators overload
        constexpr bool operator==( const vec3Type other ) const;
        constexpr bool operator!=( const vec3Type other ) const;

        //************************************************************
        // Friend functions, could probably be outside of the struct
        template <typename Y>
        friend constexpr vec3Type operator+( vec3Type left, const vec3dTemplate<Y>& right );
        template <typename Y>
        friend constexpr vec3Type operator-( vec3Type left, const vec3dTemplate<Y>& right );
        template <typename Y>
        friend constexpr vec3Type operator*( vec3Type left, const vec3dTemplate<Y>& right );
        template <typename Y>
        friend constexpr vec3Type operator/( vec3Type left, const vec3dTemplate<Y>& right );

        // Output operators, overloaded
        template <typename Y>
        friend std::ostream& operator<<( std::ostream& stream, const vec3dTemplate<Y>& v );
    };
} // namespace muggy::math

#ifndef USE_MATH_EXTERNAL
#define INCLUDE_VEC3_CPP            1
#include "vec3dTemplate.cpp"
#undef INCLUDE_VEC3_CPP
#endif

#endif