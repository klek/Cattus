//********************************************************************
//  File:    vec2dTemplate.inl
//  Date:    Sun, 19 Nov 2023: 02:28
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vecTemplate.h"

namespace muggy::math
{
    //****************************************************************
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

    //****************************************************************
    // Member functions

    // Vector add
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::add( const vec2dTemplate<T>& other )
    {
        this->x += other.x;
        this->y += other.y;

        return *this;
    }

    // Vector subtract
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::subtract( const vec2dTemplate<T>& other )
    {
        this->x -= other.x;
        this->y -= other.y;

        return *this;
    }

    // Vector multiply
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::multiply( const vec2dTemplate<T>& other )
    {
        this->x *= other.x;
        this->y *= other.y;

        return *this;
    }

    // Vector divide
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::divide( const vec2dTemplate<T>& other )
    {
        this->x /= other.x;
        this->y /= other.y;

        return *this;
    }

    // Vector add with scalar
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::add( const T& scalar )
    {
        this->x += scalar;
        this->y += scalar;

        return *this;
    }

    // Vector subtract with scalar
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::subtract( const T& scalar )
    {
        this->x -= scalar;
        this->y -= scalar;

        return *this;
    }

    // Vector multiply with scalar
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::multiply( const T& scalar )
    {
        this->x *= scalar;
        this->y *= scalar;

        return *this;
    }

    // Vector divide with scalar
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::divide( const T& scalar )
    {
        this->x /= scalar;
        this->y /= scalar;

        return *this;
    }

    // Returns the dot-product of itself with other
    template <typename T>
    constexpr inline T vec2dTemplate<T>::dot( const vec2dTemplate<T>& other ) const
    {
        return T( ( x * other.x ) + ( y * other.y ) );
    }

    // Returns the dot-product of itself
    template <typename T>
    constexpr inline T vec2dTemplate<T>::dot( void ) const
    {
        return T( ( x * x ) + ( y * y ) );
    }

    // Returns the length of itself by calculating the square root of
    // the dot-product of itself
    template <typename T>
    constexpr inline T vec2dTemplate<T>::length( void ) const
    {
        return T( maths_sqrt( dot() ) );
    }

    // Return itself after normalizing
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::normalize( void )
    {
        T n = dot( );
        if ( n > 0 )
        {
            T factor = 1 / length( );
            this->x *= factor;
            this->y *= factor;
        }

        return *this;
    }

    //****************************************************************
    // Operators

    // Assignment
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::operator=( const vec2dTemplate<T>& vec )
    {
        this->x = vec.x;
        this->y = vec.y;

        return *this;
    }

    // Add with other vector
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::operator+=( const vec2dTemplate<T>& other )
    {
        return this->add( other );
    }

    // Subtract with other vector
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::operator-=( const vec2dTemplate<T>& other )
    {
        return this->subtract( other );
    }

    // Multiply with other vector
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::operator*=( const vec2dTemplate<T>& other )
    {
        return this->multiply( other );
    }

    // Divide with other vector
    template <typename T>
    constexpr inline vec2dTemplate<T>& vec2dTemplate<T>::operator/=( const vec2dTemplate<T>& other )
    {
        return this->divide( other );
    }

    // Boolean operators
    template <typename T>
    constexpr inline bool vec2dTemplate<T>::operator==( const vec2dTemplate<T> other ) const
    {
        return ( this->x == other.x &&
                 this->y == other.y    );
    }

    template <typename T>
    constexpr inline bool vec2dTemplate<T>::operator!=( const vec2dTemplate<T> other ) const
    {
        return !( *this == other );
    }

    //****************************************************************
    // General operators

    template <typename T>
    constexpr inline vec2dTemplate<T> operator+( const vec2dTemplate<T>& left, const vec2dTemplate<T>& right )
    {
        vec2dTemplate<T> vec { left };
        return vec.add( right );
    }

    template <typename T>
    constexpr inline vec2dTemplate<T> operator-( const vec2dTemplate<T>& left, const vec2dTemplate<T>& right )
    {
        vec2dTemplate<T> vec { left };
        return vec.subtract( right );
    }

    template <typename T>
    constexpr inline vec2dTemplate<T> operator*( const vec2dTemplate<T>& left, const vec2dTemplate<T>& right )
    {
        vec2dTemplate<T> vec { left };
        return vec.multiply( right );
    }

    template <typename T>
    constexpr inline vec2dTemplate<T> operator/( const vec2dTemplate<T>& left, const vec2dTemplate<T>& right )
    {
        vec2dTemplate<T> vec { left };
        return vec.divide( right );
    }

    //****************************************************************
    // Friend operators

    // Output operators, overloaded
    template <typename T>
    std::ostream& operator<<( std::ostream& stream, const vec2dTemplate<T>& v )
    {
        stream << "vec2d: ( " << v.x << ", " 
                              << v.y << " )";

        return stream;
    }

} // namespace muggy::math
