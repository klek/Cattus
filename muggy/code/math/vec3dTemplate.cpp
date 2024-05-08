//********************************************************************
//  File:    vec3dTemplate.cpp
//  Date:    Sun, 19 Nov 2023: 02:50
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************
#ifdef INCLUDE_VEC3_CPP

#include "vec3dTemplate.h"

namespace muggy::math
{
    // Constructors
    template <typename T>
    constexpr vec3dTemplate<T>::vec3dTemplate( ) 
     : 
        x( T(0) ), 
        y( T(0) ), 
        z( T(0) )
    {}

    template <typename T>
    constexpr vec3dTemplate<T>::vec3dTemplate( const T& _s )
     :
        x( _s ),
        y( _s ),
        z( _s )
    {}

    template <typename T>
    constexpr vec3dTemplate<T>::vec3dTemplate( const T& _x, 
                                               const T& _y, 
                                               const T& _z )
     :
        x( _x ),
        y( _y ),
        z( _z )
    {}

    template <typename T>
    constexpr vec3dTemplate<T>::vec3dTemplate( const vec3dTemplate<T>& _v )
     : 
        x( _v.x ),
        y( _v.y ),
        z( _v.z )
    {}

    template <typename T>
    constexpr vec3dTemplate<T>::vec3dTemplate( const T (&_arr)[3] )
     : 
        x( _arr[0] ),
        y( _arr[1] ),
        z( _arr[2] )
    {}

    // Member functions
    // Implement basic math functions for 4D vectors
    template <typename T>
    constexpr vec3dTemplate<T>& vec3dTemplate<T>::add( const vec3dTemplate<T>& other )
    {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;

        return *this;
    }

    template <typename T>
    constexpr vec3dTemplate<T>& vec3dTemplate<T>::subtract( const vec3dTemplate<T>& other )
    {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;

        return *this;
    }

    template <typename T>
    constexpr vec3dTemplate<T>& vec3dTemplate<T>::multiply( const vec3dTemplate<T>& other )
    {
        this->x *= other.x;
        this->y *= other.y;
        this->z *= other.z;

        return *this;
    }

    template <typename T>
    constexpr vec3dTemplate<T>& vec3dTemplate<T>::divide( const vec3dTemplate<T>& other )
    {
        this->x /= other.x;
        this->y /= other.y;
        this->z /= other.z;

        return *this;
    }

    // Math operators overload
    // NOTE(klek): Simply calls above math functions
    template <typename T>
    constexpr vec3dTemplate<T>& vec3dTemplate<T>::operator+=( const vec3dTemplate<T>& other )
    {
        return this->add( other );
    }

    template <typename T>
    constexpr vec3dTemplate<T>& vec3dTemplate<T>::operator-=( const vec3dTemplate<T>& other )
    {
        return this->subtract( other );
    }

    template <typename T>
    constexpr vec3dTemplate<T>& vec3dTemplate<T>::operator*=( const vec3dTemplate<T>& other )
    {
        return this->multiply( other );
    }

    template <typename T>
    constexpr vec3dTemplate<T>& vec3dTemplate<T>::operator/=( const vec3dTemplate<T>& other )
    {
        return this->divide( other );
    }

    // Boolean operators overload
    template <typename T>
    constexpr bool vec3dTemplate<T>::operator==( const vec3dTemplate<T> other ) const
    {
        return ( this->x == other.x &&
                 this->y == other.y &&
                 this->z == other.z    );
    }

    template <typename T>
    constexpr bool vec3dTemplate<T>::operator!=( const vec3dTemplate<T> other ) const
    {
        return !( *this == other );
    }

    //****************************************************************
    // Friend functions implementation
    template <typename T>
    constexpr vec3dTemplate<T> operator+( vec3dTemplate<T> left, const vec3dTemplate<T>& right )
    {
        return left.add( right );
    }

    template <typename T>
    constexpr vec3dTemplate<T> operator-( vec3dTemplate<T> left, const vec3dTemplate<T>& right )
    {
        return left.subtract( right );
    }

    template <typename T>
    constexpr vec3dTemplate<T> operator*( vec3dTemplate<T> left, const vec3dTemplate<T>& right )
    {
        return left.multiply( right );
    }

    template <typename T>
    constexpr vec3dTemplate<T> operator/( vec3dTemplate<T> left, const vec3dTemplate<T>& right )
    {
        return left.divide( right );
    }

    // Output operators, overloaded
    template <typename T>
    std::ostream& operator<<( std::ostream& stream, const vec3dTemplate<T>& v )
    {
        stream << "vec3d: ( " << v.x << ", " 
                              << v.y << ", " 
                              << v.z << " )";

        return stream;
    }

} // namespace muggy::math

#endif