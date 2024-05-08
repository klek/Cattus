//********************************************************************
//  File:    vec4dTemplate.cpp
//  Date:    Sun, 19 Nov 2023: 00:47
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************
#ifdef INCLUDE_VEC4_CPP

#include "vec4dTemplate.h"

namespace muggy::math
{
    // Constructors
    template <typename T>
    constexpr vec4dTemplate<T>::vec4dTemplate( ) 
     : 
        x( T(0) ), 
        y( T(0) ), 
        z( T(0) ), 
        w( T(0) )
    {}

    template <typename T>
    constexpr vec4dTemplate<T>::vec4dTemplate( const T& _s )
     :
        x( _s ),
        y( _s ),
        z( _s ),
        w( _s )
    {}

    template <typename T>
    constexpr vec4dTemplate<T>::vec4dTemplate( const T& _x, 
                                               const T& _y, 
                                               const T& _z, 
                                               const T& _w )
     :
        x( _x ),
        y( _y ),
        z( _z ),
        w( _w )
    {}

    template <typename T>
    constexpr vec4dTemplate<T>::vec4dTemplate( const vec4dTemplate<T>& _v )
     : 
        x( _v.x ),
        y( _v.y ),
        z( _v.z ),
        w( _v.w )
    {}

    template <typename T>
    constexpr vec4dTemplate<T>::vec4dTemplate( const T (&_arr)[4] )
     : 
        x( _arr[0] ),
        y( _arr[1] ),
        z( _arr[2] ),
        w( _arr[3] )
    {}

    // Member functions
    // Implement basic math functions for 4D vectors
    template <typename T>
    constexpr vec4dTemplate<T>& vec4dTemplate<T>::add( const vec4dTemplate<T>& other )
    {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        this->w += other.w;

        return *this;
    }

    template <typename T>
    constexpr vec4dTemplate<T>& vec4dTemplate<T>::subtract( const vec4dTemplate<T>& other )
    {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        this->w -= other.w;

        return *this;
    }

    template <typename T>
    constexpr vec4dTemplate<T>& vec4dTemplate<T>::multiply( const vec4dTemplate<T>& other )
    {
        this->x *= other.x;
        this->y *= other.y;
        this->z *= other.z;
        this->w *= other.w;

        return *this;
    }

    template <typename T>
    constexpr vec4dTemplate<T>& vec4dTemplate<T>::divide( const vec4dTemplate<T>& other )
    {
        this->x /= other.x;
        this->y /= other.y;
        this->z /= other.z;
        this->w /= other.w;

        return *this;
    }

    // Math operators overload
    // NOTE(klek): Simply calls above math functions
    template <typename T>
    constexpr vec4dTemplate<T>& vec4dTemplate<T>::operator+=( const vec4dTemplate<T>& other )
    {
        return this->add( other );
    }

    template <typename T>
    constexpr vec4dTemplate<T>& vec4dTemplate<T>::operator-=( const vec4dTemplate<T>& other )
    {
        return this->subtract( other );
    }

    template <typename T>
    constexpr vec4dTemplate<T>& vec4dTemplate<T>::operator*=( const vec4dTemplate<T>& other )
    {
        return this->multiply( other );
    }

    template <typename T>
    constexpr vec4dTemplate<T>& vec4dTemplate<T>::operator/=( const vec4dTemplate<T>& other )
    {
        return this->divide( other );
    }

    // Boolean operators overload
    template <typename T>
    constexpr bool vec4dTemplate<T>::operator==( const vec4dTemplate<T> other ) const
    {
        return ( this->x == other.x &&
                 this->y == other.y &&
                 this->z == other.z &&
                 this->w == other.w    );
    }

    template <typename T>
    constexpr bool vec4dTemplate<T>::operator!=( const vec4dTemplate<T> other ) const
    {
        return !( *this == other );
    }

    //****************************************************************
    // Friend functions implementation
    template <typename T>
    constexpr vec4dTemplate<T> operator+( vec4dTemplate<T> left, const vec4dTemplate<T>& right )
    {
        return left.add( right );
    }

    template <typename T>
    constexpr vec4dTemplate<T> operator-( vec4dTemplate<T> left, const vec4dTemplate<T>& right )
    {
        return left.subtract( right );
    }

    template <typename T>
    constexpr vec4dTemplate<T> operator*( vec4dTemplate<T> left, const vec4dTemplate<T>& right )
    {
        return left.multiply( right );
    }

    template <typename T>
    constexpr vec4dTemplate<T> operator/( vec4dTemplate<T> left, const vec4dTemplate<T>& right )
    {
        return left.divide( right );
    }

    // Output operators, overloaded
    template <typename T>
    std::ostream& operator<<( std::ostream& stream, const vec4dTemplate<T>& v )
    {
        stream << "vec4d: ( " << v.x << ", " 
                              << v.y << ", " 
                              << v.z << ", " 
                              << v.w << " )";

        return stream;
    }

} // namespace muggy::math

#endif