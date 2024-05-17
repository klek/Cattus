//********************************************************************
//  File:    vec4dtemplate.h
//  Date:    Sun, 19 Nov 2023: 00:23
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(OLD_VEC_4D_TEMPLATE_H)
#define OLD_VEC_4D_TEMPLATE_H

#if USE_OLD_VEC4D

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

        //************************************************************
        // Constructors
        constexpr vec4dTemplate();
        constexpr vec4dTemplate( const vType& _s );
        constexpr vec4dTemplate( const vType& _x, 
                                 const vType& _y, 
                                 const vType& _z, 
                                 const vType& _w );
        constexpr vec4dTemplate( const vec4Type& _v );
        constexpr explicit vec4dTemplate( const vType (&_arr)[4] );

        //************************************************************
        // Member functions

        // Vector add
        constexpr inline vec4Type& add( const vec4Type& other );
        // Vector subtract
        constexpr inline vec4Type& subtract( const vec4Type& other );
        // Vector multiply
        constexpr inline vec4Type& multiply( const vec4Type& other );
        // Vector divide
        constexpr inline vec4Type& divide( const vec4Type& other );

        // Vector add with scalar
        constexpr inline vec4Type& add( const vType& scalar );
        // Vector subtract with scalar
        constexpr inline vec4Type& subtract( const vType& scalar );
        // Vector multiply with scalar
        constexpr inline vec4Type& multiply( const vType& scalar );
        // Vector divide with scalar
        constexpr inline vec4Type& divide( const vType& scalar );

        // Dot product
        constexpr inline vType dot( const vec4Type& other ) const;
        constexpr inline vType dot( void ) const;
        // Cross product
        constexpr inline vec4Type cross( const vec4Type& other ) const;
        // Length function
        constexpr inline vType length( void ) const;
        // Normalize function
        constexpr inline vec4Type& normalize( void );

        //************************************************************
        // Operators
        // Assignment
        constexpr inline vec4Type& operator=( const vec4Type& vec );
        // Add with other vector
        constexpr inline vec4Type& operator+=( const vec4Type& other );
        // Subtract with other vector
        constexpr inline vec4Type& operator-=( const vec4Type& other );
        // Multiply with other vector
        constexpr inline vec4Type& operator*=( const vec4Type& other );
        // Divide with other vector
        constexpr inline vec4Type& operator/=( const vec4Type& other );

        // Boolean operators
        constexpr inline bool operator==( const vec4Type other ) const;
        constexpr inline bool operator!=( const vec4Type other ) const;

        // Output operators, overloaded
        template <typename Y>
        friend std::ostream& operator<<( std::ostream& stream, const vec4dTemplate<Y>& v );
    };

    //***************************************************************
    // General operators

    // Add vectors
    template <typename T>
    constexpr inline vec4dTemplate<T> operator+( const vec4dTemplate<T>& left,
                                                 const vec4dTemplate<T>& right );
    // Subtract vectors
    template <typename T>
    constexpr inline vec4dTemplate<T> operator-( const vec4dTemplate<T>& left,
                                                 const vec4dTemplate<T>& right );
    // Multiply vectors
    template <typename T>
    constexpr inline vec4dTemplate<T> operator*( const vec4dTemplate<T>& left,
                                                 const vec4dTemplate<T>& right );
    // Divide vectors
    template <typename T>
    constexpr inline vec4dTemplate<T> operator/( const vec4dTemplate<T>& left,
                                                 const vec4dTemplate<T>& right );

    //****************************************************************
    // Special operations

    // Rotate a vector with a unit quaternion
    template <typename T>
    constexpr inline vec4dTemplate<T> rotateVector( vec4dTemplate<T> vec,
                                                    vec4dTemplate<T> quaternion );
} // namespace muggy::math

#ifndef USE_MATH_EXTERNAL
//#include "vec4dTemplate.inl"
#endif

#endif

#endif