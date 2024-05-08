//********************************************************************
//  File:    vec2dTemplate.cpp
//  Date:    Sun, 19 Nov 2023: 02:28
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************
#ifdef INCLUDE_VEC2_CPP
#include "vec2dTemplate.h"

namespace muggy::math
{
    // Constructors
    template <typename T>
    constexpr vec2dTemplate<T>::vec2dTemplate( ) 
     : 
        x( T(0) ), 
        y( T(0) )
    {}

    template <typename T>
    constexpr vec2dTemplate<T>::vec2dTemplate( const T& _s ) 
     : 
        x( _s ), 
        y( _s )
    {}

    template <typename T>
    constexpr vec2dTemplate<T>::vec2dTemplate( const T& _x, 
                                               const T& _y )
     :
        x( _x ),
        y( _y )
    {}

    template <typename T>
    constexpr vec2dTemplate<T>::vec2dTemplate( const vec2dTemplate<T>& _v )
     : 
        x( _v.x ),
        y( _v.y )
    {}

    template <typename T>
    constexpr vec2dTemplate<T>::vec2dTemplate( const T (&_arr)[2] )
     : 
        x( _arr[0] ),
        y( _arr[1] )
    {}

    // Member functions
    // Implement basic math functions for 2D vectors
    template <typename T>
    constexpr vec2dTemplate<T>& vec2dTemplate<T>::add( const vec2dTemplate<T>& other )
    {
        this->x += other.x;
        this->y += other.y;

        return *this;
    }

    template <typename T>
    constexpr vec2dTemplate<T>& vec2dTemplate<T>::subtract( const vec2dTemplate<T>& other )
    {
        this->x -= other.x;
        this->y -= other.y;

        return *this;
    }

    template <typename T>
    constexpr vec2dTemplate<T>& vec2dTemplate<T>::multiply( const vec2dTemplate<T>& other )
    {
        this->x *= other.x;
        this->y *= other.y;

        return *this;
    }

    template <typename T>
    constexpr vec2dTemplate<T>& vec2dTemplate<T>::divide( const vec2dTemplate<T>& other )
    {
        this->x /= other.x;
        this->y /= other.y;

        return *this;
    }

    // Math operators overload
    // NOTE(klek): Simply calls above math functions
    template <typename T>
    constexpr vec2dTemplate<T>& vec2dTemplate<T>::operator+=( const vec2dTemplate<T>& other )
    {
        return this->add( other );
    }

    template <typename T>
    constexpr vec2dTemplate<T>& vec2dTemplate<T>::operator-=( const vec2dTemplate<T>& other )
    {
        return this->subtract( other );
    }

    template <typename T>
    constexpr vec2dTemplate<T>& vec2dTemplate<T>::operator*=( const vec2dTemplate<T>& other )
    {
        return this->multiply( other );
    }

    template <typename T>
    constexpr vec2dTemplate<T>& vec2dTemplate<T>::operator/=( const vec2dTemplate<T>& other )
    {
        return this->divide( other );
    }

    // Boolean operators overload
    template <typename T>
    constexpr bool vec2dTemplate<T>::operator==( const vec2dTemplate<T> other ) const
    {
        return ( this->x == other.x &&
                 this->y == other.y    );
    }

    template <typename T>
    constexpr bool vec2dTemplate<T>::operator!=( const vec2dTemplate<T> other ) const
    {
        return !( *this == other );
    }

    //****************************************************************
    // Friend functions implementation
    template <typename T>
    constexpr vec2dTemplate<T> operator+( vec2dTemplate<T> left, const vec2dTemplate<T>& right )
    {
        return left.add( right );
    }

    template <typename T>
    constexpr vec2dTemplate<T> operator-( vec2dTemplate<T> left, const vec2dTemplate<T>& right )
    {
        return left.subtract( right );
    }

    template <typename T>
    constexpr vec2dTemplate<T> operator*( vec2dTemplate<T> left, const vec2dTemplate<T>& right )
    {
        return left.multiply( right );
    }

    template <typename T>
    constexpr vec2dTemplate<T> operator/( vec2dTemplate<T> left, const vec2dTemplate<T>& right )
    {
        return left.divide( right );
    }

    // Output operators, overloaded
    template <typename T>
    std::ostream& operator<<( std::ostream& stream, const vec2dTemplate<T>& v )
    {
        stream << "vec2d: ( " << v.x << ", " 
                              << v.y << " )";

        return stream;
    }

} // namespace muggy::math

#endif