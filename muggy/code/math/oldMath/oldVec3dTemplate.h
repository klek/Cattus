//********************************************************************
//  File:    vec3dTemplate.h
//  Date:    Sun, 19 Nov 2023: 02:46
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(OLD_VEC_3D_TEMPLATE_H)
#define OLD_VEC_3D_TEMPLATE_H

#if USE_OLD_VEC3D

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

        //************************************************************
        // Constructors
        constexpr vec3dTemplate();
        constexpr vec3dTemplate( const vType& _s );
        constexpr vec3dTemplate( const vType& _x, 
                                 const vType& _y, 
                                 const vType& _z );
        constexpr vec3dTemplate( const vec3Type& _v );
        constexpr explicit vec3dTemplate( const vType (&_arr)[3] );

        //************************************************************
        // Member functions

        // Vector add
        constexpr inline vec3Type& add( const vec3Type& other );
        // Vector subtract
        constexpr inline vec3Type& subtract( const vec3Type& other );
        // Vector multiply
        constexpr inline vec3Type& multiply( const vec3Type& other );
        // Vector divide
        constexpr inline vec3Type& divide( const vec3Type& other );

        // Vector add with scalar
        constexpr inline vec3Type& add( const vType& scalar );
        // Vector subtract with scalar
        constexpr inline vec3Type& subtract( const vType& scalar );
        // Vector multiply with scalar
        constexpr inline vec3Type& multiply( const vType& scalar );
        // Vector divide with scalar
        constexpr inline vec3Type& divide( const vType& scalar );

        // Dot product
        constexpr inline vType dot( const vec3Type& other ) const;
        constexpr inline vType dot( void ) const;
        // Cross product
        constexpr inline vec3Type cross( const vec3Type& other ) const;
        // Length function
        constexpr inline vType length( void ) const;
        // Normalize function
        constexpr inline vec3Type& normalize( void );

        //************************************************************
        // Operators

        // Assignment
        constexpr inline vec3Type& operator=( const vec3Type& vec );
        // Add with other vector
        constexpr inline vec3Type& operator+=( const vec3Type& other );
        // Subtract with other vector
        constexpr inline vec3Type& operator-=( const vec3Type& other );
        // Multiply with other vector
        constexpr inline vec3Type& operator*=( const vec3Type& other );
        // Divide with other vector
        constexpr inline vec3Type& operator/=( const vec3Type& other );

        // Boolean operators
        constexpr inline bool operator==( const vec3Type other ) const;
        constexpr inline bool operator!=( const vec3Type other ) const;

        // Output operators, overloaded
        template <typename Y>
        friend std::ostream& operator<<( std::ostream& stream, const vec3dTemplate<Y>& v );
    };

    //****************************************************************
    // General operators

    // Add vectors
    template <typename T>
    constexpr inline vec3dTemplate<T> operator+( vec3dTemplate<T> left,
                                                 const vec3dTemplate<T>& right );
    // Subtract vectors
    template <typename T>
    constexpr inline vec3dTemplate<T> operator-( vec3dTemplate<T> left,
                                                 const vec3dTemplate<T>& right );
    // Multiply vectors
    template <typename T>
    constexpr inline vec3dTemplate<T> operator*( vec3dTemplate<T> left,
                                                 const vec3dTemplate<T>& right );
    // Divide vectors
    template <typename T>
    constexpr inline vec3dTemplate<T> operator/( vec3dTemplate<T> left,
                                                 const vec3dTemplate<T>& right );

} // namespace muggy::math

#ifndef USE_MATH_EXTERNAL
//#include "vec3dTemplate.inl"
#endif

#endif

#endif