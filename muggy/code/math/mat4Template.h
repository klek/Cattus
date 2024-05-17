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
#include "vecTemplate.h"
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

        // NOTE(klek): The element array is indexed according to
        // row-major which might not be what the user wants. To convert
        // the element array into column-major, use the transpose
        // functions, to support OpenGL or vulkan.
        //
        // This gives the following matrix with indicies:
        // 
        //    0     1     2     3
        //    4     5     6     7
        //    8     9    10    11
        //   12    13    14    15
        //
        union 
        {
            vType elements[ FOUR_BY_FOUR ];
            vType m[4][4];
            vec4Type rows[ 4 ];
            // vec4Type cols[ 4 ];
        };

        //************************************************************
        // Constructors
        constexpr mat4Template();
        constexpr mat4Template( T diagonal );
        constexpr mat4Template( const mat4Type& mat );
        constexpr mat4Template( T m00, T m01, T m02, T m03,
                                T m10, T m11, T m12, T m13,
                                T m20, T m21, T m22, T m23,
                                T m30, T m31, T m32, T m33 );

        //************************************************************
        // Member functions

        // Support for getting elements
        constexpr inline vec4Type getColumn( int index ) const;
        constexpr inline vec4Type getRow( int index ) const;
        constexpr inline vType getElement( int index ) const;

        // Transpose
        constexpr inline mat4Type transpose( void ) const;
        constexpr inline mat4Type& transposeMe( void );
        
        // Matrix addition
        constexpr inline mat4Type& add( const mat4Type& rhs );
        // Matrix subtraction
        constexpr inline mat4Type& subtract( const mat4Type& rhs );
        // Matrix multiplication
        constexpr inline mat4Type& multiply( const mat4Type& rhs );

        // Matrix addition with scalar
        constexpr inline mat4Type& add( const T& scalar );
        // Matrix subtraction with scalar
        constexpr inline mat4Type& subtract( const T& scalar );
        // Matrix division with scalar
        constexpr inline mat4Type& divide( const T& scalar );
        // Matrix multiplication with scalar
        constexpr inline mat4Type& multiply( const T& scalar );

        //************************************************************
        // Matrix to vector / Vector to matrix multiplication

        // Matrix4x4-Vector3d multiplication
        constexpr inline vec3Type multiplyMatVec( const vec3Type& rhs ) const;
        // Vector3d-Matrix4x4 multiplication
        constexpr inline vec3Type multiplyVecMat( const vec3Type& lhs ) const;
        // Matrix4x4-Vector4d multiplication
        constexpr inline vec4Type multiplyMatVec( const vec4Type& rhs ) const;
        // Vector4d-Matrix4x4 multiplication
        constexpr inline vec4Type multiplyVecMat( const vec4Type& lhs ) const;

        //************************************************************
        // Operators

        // Assignment
        constexpr inline mat4Type& operator=( const mat4Type& mat );
        // Add with other matrix
        constexpr inline mat4Type& operator+=( const mat4Type& other );
        // Subtract with other matrix
        constexpr inline mat4Type& operator-=( const mat4Type& other );
        // Multiply with other matrix
        constexpr inline mat4Type& operator*=( const mat4Type& other );
        // NOTE(klek): When would we divide by another matrix?
        // Divide with other matrix
        //constexpr inline mat4Type& operator/=( const mat4Type& other );
        // Add with scalar
        constexpr inline mat4Type& operator+=( const vType& scalar );
        // Subtract with scalar
        constexpr inline mat4Type& operator-=( const vType& scalar );
        // Multiply with scalar
        constexpr inline mat4Type& operator*=( const vType& scalar );
        // Divide with scalar
        constexpr inline mat4Type& operator/=( const vType& scalar );

        // Output operators, overloaded
        template <typename Y>
        friend std::ostream& operator<<(std::ostream &stream, const mat4Template<Y>& m);
    };

    //****************************************************************
    // General operators

    // Multiplication Matrix*Matrix
    template <typename T>
    constexpr inline mat4Template<T> operator*( const mat4Template<T>& left,
                                                const mat4Template<T>& right );
    // Multiplication Matrix*Vec3
    template <typename T>
    constexpr inline vec3dTemplate<T> operator*( const mat4Template<T>& left, 
                                                 const vec3dTemplate<T>& right );
    // Multiplication Vec3*Matrix
    template <typename T>
    constexpr inline vec3dTemplate<T> operator*( const vec3dTemplate<T>& left, 
                                                 const mat4Template<T>& right );
    // Multiplication Matrix*Vec4
    template <typename T>
    constexpr inline vec4dTemplate<T> operator*( const mat4Template<T>& left, 
                                                 const vec4dTemplate<T>& right );
    // Multiplication Vec4*Matrix
    template <typename T>
    constexpr inline vec4dTemplate<T> operator*( const vec4dTemplate<T>& left, 
                                                 const mat4Template<T>& right );

    //****************************************************************
    // General purpose matrices

    // Identity matrix
    template <typename T>
    constexpr inline mat4Template<T> identity( void );

    //****************************************************************
    // Vulkan matrices
    // Orthographic matrices
    template <typename T>
    constexpr inline mat4Template<T> orthographicVkRHOffCenter( T left,
                                                                T right,
                                                                T bottom,
                                                                T top,
                                                                T near,
                                                                T far );
    template <typename T>
    constexpr inline mat4Template<T> orthographicVkRH( T width,
                                                       T height,
                                                       T near,
                                                       T far );
    // Perspective matrices
    template <typename T>
    constexpr inline mat4Template<T> perspectiveVkRHOffCenter( T left,
                                                               T right,
                                                               T bottom,
                                                               T top,
                                                               T near,
                                                               T far );
    template <typename T>
    constexpr inline mat4Template<T> perspectiveVkRH( T fovY,
                                                      T aspectRatio,
                                                      T near,
                                                      T far );
    // View matrices
    template <typename T>
    constexpr inline mat4Template<T> viewLookAtVkRH( vec3dTemplate<T> pos,
                                                     vec3dTemplate<T> target,
                                                     vec3dTemplate<T> upDir );
    template <typename T>
    constexpr inline mat4Template<T> viewLookToVkRH( vec3dTemplate<T> pos,
                                                     vec3dTemplate<T> dir,
                                                     vec3dTemplate<T> upDir );

    //****************************************************************
    // OpenGL matrices
    // Orthographics matrix for OpenGL
    template <typename T>
    constexpr inline mat4Template<T> orthographicGL( T left, 
                                                     T right, 
                                                     T bottom, 
                                                     T top, 
                                                     T near, 
                                                     T far );
    // Perspective matrix for OpenGL
    template <typename T>
    constexpr inline mat4Template<T> perspectiveGL( T fov, 
                                                    T aspectRatio, 
                                                    T near, 
                                                    T far );
    // Translation matrix for OpenGL
    template <typename T>
    constexpr inline mat4Template<T> translationGL( const vec3dTemplate<T>& translation );
    // Rotation matrix for OpenGL
    template <typename T>
    constexpr inline mat4Template<T> rotationGL( T angle, const vec3dTemplate<T>& axis );
    // Scale matrix for OpenGL
    template <typename T>
    constexpr inline mat4Template<T> scaleGL( const vec3dTemplate<T>& scale );

}

#ifndef USE_MATH_EXTERNAL
#include "mat4Template.inl"
#endif

#endif