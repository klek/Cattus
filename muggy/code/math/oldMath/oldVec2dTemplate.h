//********************************************************************
//  File:    vec2dTemplate.h
//  Date:    Sun, 19 Nov 2023: 02:23
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(OLD_VEC_2D_TEMPLATE_H)
#define OLD_VEC_2D_TEMPLATE_H

#if USE_OLD_VEC2D

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
        
        //************************************************************
        // Constructors
        constexpr vec2dTemplate();
        constexpr vec2dTemplate( const vType& _s );
        constexpr vec2dTemplate( const vType& _x, 
                                 const vType& _y );
        constexpr vec2dTemplate( const vec2Type& _v );
        constexpr vec2dTemplate( const vType (&_arr)[2] );

        //************************************************************
        // Member functions

        // Vector add
        constexpr inline vec2Type& add( const vec2Type& other );
        // Vector subtract
        constexpr inline vec2Type& subtract( const vec2Type& other );
        // Vector multiply
        constexpr inline vec2Type& multiply( const vec2Type& other );
        // Vector divide
        constexpr inline vec2Type& divide( const vec2Type& other );

        // Vector add with scalar
        constexpr inline vec2Type& add( const vType& scalar );
        // Vector subtract with scalar
        constexpr inline vec2Type& subtract( const vType& scalar );
        // Vector multiply with scalar
        constexpr inline vec2Type& multiply( const vType& scalar );
        // Vector divide with scalar
        constexpr inline vec2Type& divide( const vType& scalar );

        // Dot product
        constexpr inline vType dot( const vec2Type& other ) const;
        constexpr inline vType dot( void ) const;
        // Length function
        constexpr inline vType length( void ) const;
        // Normalize function
        constexpr inline vec2Type& normalize( void );

        //************************************************************
        // Operators
        // Asignment
        constexpr inline vec2Type& operator=( const vec2Type& other );
        // Add with other vector
        constexpr inline vec2Type& operator+=( const vec2Type& other );
        // Subtract with other vector
        constexpr inline vec2Type& operator-=( const vec2Type& other );
        // Multiply with other vector
        constexpr inline vec2Type& operator*=( const vec2Type& other );
        // Divide with other vector
        constexpr inline vec2Type& operator/=( const vec2Type& other );

        // Boolean operators
        constexpr inline bool operator==( const vec2Type other ) const;
        constexpr inline bool operator!=( const vec2Type other ) const;

        // Output operators, overloaded
        template <typename Y>
        friend std::ostream& operator<<( std::ostream& stream, const vec2dTemplate<Y>& v );
    };

    //***************************************************************
    // General operators

    // Add vectors
    template <typename T>
    constexpr inline vec2dTemplate<T> operator+( const vec2dTemplate<T>& left,
                                                 const vec2dTemplate<T>& right );
    // Subtract vectors
    template <typename T>
    constexpr inline vec2dTemplate<T> operator-( const vec2dTemplate<T>& left,
                                                 const vec2dTemplate<T>& right );
    // Multiply vectors
    template <typename T>
    constexpr inline vec2dTemplate<T> operator*( const vec2dTemplate<T>& left,
                                                 const vec2dTemplate<T>& right );
    // Divide vectors
    template <typename T>
    constexpr inline vec2dTemplate<T> operator/( const vec2dTemplate<T>& left,
                                                 const vec2dTemplate<T>& right );


} // namespace muggy::math

#ifndef USE_MATH_EXTERNAL
//#include "vec2dTemplate.inl"
#endif

#endif

#endif