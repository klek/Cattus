//********************************************************************
//  File:    mat4Template.h
//  Date:    Sun, 19 Nov 2023: 03:04
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(MAT_4_TEMPLATE_H)
#define MAT_4_TEMPLATE_H

#include <iostream>
#include "vec3dTemplate.h"
#include "vec4dTemplate.h"
#include "maths_funcs.h"

#define FOUR_BY_FOUR (4 * 4)

namespace muggy::math
{
    template <typename T>
    struct mat4Template
    {
        typedef T vType;
        typedef mat4Template<T> mat4Type;
        typedef vec4dTemplate<T> vec4Type;
        typedef vec3dTemplate<T> vec3Type;

        // NOTE(klek): The element array is indexed according to column-major
        // due to OpenGL. 
        // This gives the following matrix with indicies:
        // 
        //    0     4     8    12
        //    1     5     9    13
        //    2     6    10    14
        //    3     7    11    15
        //
        union 
        {
            vType elements[ FOUR_BY_FOUR ];
            vec4Type cols[ 4 ];
            vec4Type rows[ 4 ];
        };

        // Constructors
        constexpr mat4Template();
        constexpr mat4Template( T diagonal );
        constexpr mat4Template( const mat4Type& m );

        // Member functions
        // Support for getting elements
        constexpr vec4Type getColumn( int index );
        constexpr vec4Type getRow( int index );
        constexpr vType getElement( int index );
        
        // Matrix multiplication
        constexpr mat4Type& multiply( const mat4Type& other );

        // Vector3d-Matrix4x4 multiplication
        constexpr vec3Type multiply( const vec3Type& other ) const;

        // Vector4d-Matrix4x4 multiplication
        constexpr vec4Type multiply( const vec4Type& other ) const;

        // Math operator overload
        template <typename Y>
        friend constexpr mat4Type operator*( mat4Type left, const mat4Template<Y>& right );
        template <typename Y>
        friend constexpr vec3dTemplate<Y> operator*( const mat4Type& left, 
                                                     const vec3dTemplate<Y>& right );
        template <typename Y>
        friend constexpr vec4dTemplate<Y> operator*( const mat4Type& left, 
                                                     const vec4dTemplate<Y>& right );
        
        constexpr mat4Type& operator*=( const mat4Type& other );

        // Identity matrix
        static constexpr mat4Type identity();

        // Orthographics matrix
        static constexpr mat4Type orthographic( vType left, 
                                                vType right, 
                                                vType bottom, 
                                                vType top, 
                                                vType near, 
                                                vType far );

        // Perspective matrix
        static constexpr mat4Type perspective( vType fov, 
                                               vType aspectRatio, 
                                               vType near, 
                                               vType far );

        // Translation matrix
        static constexpr mat4Type translation( const vec3Type& translation );

        // Rotation matrix
        static constexpr mat4Type rotation( vType angle, const vec3Type& axis );

        // Scale matrix
        static constexpr mat4Type scale(const vec3Type& scale);

        // Output operators, overloaded
        template <typename Y>
        friend std::ostream& operator<<(std::ostream &stream, const mat4Template<Y>& m);
    };

}

#ifndef USE_MATH_EXTERNAL
#define INCLUDE_MAT4_CPP            1
#include "mat4Template.cpp"
#undef INCLUDE_MAT4_CPP
#endif

#endif