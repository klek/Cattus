//********************************************************************
//  File:    vec3dTemplate.inl
//  Date:    Sun, 19 Nov 2023: 02:50
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

    //****************************************************************
    // Member functions

    // Vector add
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::add( const vec3dTemplate<T>& other )
    {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;

        return *this;
    }

    // Vector subtract
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::subtract( const vec3dTemplate<T>& other )
    {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;

        return *this;
    }

    // Vector multiply
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::multiply( const vec3dTemplate<T>& other )
    {
        this->x *= other.x;
        this->y *= other.y;
        this->z *= other.z;

        return *this;
    }

    // Vector divide
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::divide( const vec3dTemplate<T>& other )
    {
        this->x /= other.x;
        this->y /= other.y;
        this->z /= other.z;

        return *this;
    }

    // Vector add with scalar
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::add( const T& scalar )
    {
        this->x += scalar;
        this->y += scalar;
        this->z += scalar;

        return *this;
    }

    // Vector subtract with scalar
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::subtract( const T& scalar )
    {
        this->x -= scalar;
        this->y -= scalar;
        this->z -= scalar;

        return *this;
    }

    // Vector multiply with scalar
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::multiply( const T& scalar )
    {
        this->x *= scalar;
        this->y *= scalar;
        this->z *= scalar;

        return *this;
    }

    // Vector divide with scalar
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::divide( const T& scalar )
    {
        this->x /= scalar;
        this->y /= scalar;
        this->z /= scalar;

        return *this;
    }

    // Returns the dot-product of itself with other
    template <typename T>
    constexpr inline T vec3dTemplate<T>::dot( const vec3dTemplate<T>& other ) const
    {
        return T( ( x * other.x ) + ( y * other.y ) + ( z * other.z ) );
    }

    // Returns the dot-product of itself
    template <typename T>
    constexpr inline T vec3dTemplate<T>::dot( void ) const
    {
        return T( ( x * x ) + ( y * y ) + ( z * z ) );
    }

    // Returns the cross-product of itself cross other
    template <typename T>
    constexpr inline vec3dTemplate<T> vec3dTemplate<T>::cross( const vec3dTemplate<T>& other ) const 
    {
        T a, b, c;
        a = ( this->y * other.z ) - ( this->z * other.y );
        b = ( this->z * other.x ) - ( this->x * other.z );
        c = ( this->x * other.y ) - ( this->y * other.x );
        
        return vec3dTemplate<T>( a, b, c );
    }

    // Returns the length of itself by calculating the square root of
    // the dot-product of itself
    template <typename T>
    constexpr inline T vec3dTemplate<T>::length( void ) const
    {
        return T( maths_sqrt( dot() ) );
    }

    // Return itself after normalizing
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::normalize( void )
    {
        T n = dot( );
        if ( n > 0 )
        {
            T factor = 1 / length( );
            this->x *= factor;
            this->y *= factor;
            this->z *= factor;
        }

        return *this;
    }

    //****************************************************************
    // Operators

    // Assignment
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::operator=( const vec3dTemplate<T>& vec )
    {
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;

        return *this;
    }

    // Add with other vector
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::operator+=( const vec3dTemplate<T>& other )
    {
        return this->add( other );
    }

    // Subtract with other vector
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::operator-=( const vec3dTemplate<T>& other )
    {
        return this->subtract( other );
    }

    // Multiply with other vector
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::operator*=( const vec3dTemplate<T>& other )
    {
        return this->multiply( other );
    }

    // Divide with other vector
    template <typename T>
    constexpr inline vec3dTemplate<T>& vec3dTemplate<T>::operator/=( const vec3dTemplate<T>& other )
    {
        return this->divide( other );
    }

    // Boolean operators
    template <typename T>
    constexpr inline bool vec3dTemplate<T>::operator==( const vec3dTemplate<T> other ) const
    {
        return ( this->x == other.x &&
                 this->y == other.y &&
                 this->z == other.z    );
    }

    template <typename T>
    constexpr inline bool vec3dTemplate<T>::operator!=( const vec3dTemplate<T> other ) const
    {
        return !( *this == other );
    }

    //****************************************************************
    // General operators

    // Add vectors
    template <typename T>
    constexpr inline vec3dTemplate<T> operator+( const vec3dTemplate<T>& left,
                                                 const vec3dTemplate<T>& right )
    {
        vec3dTemplate<T> v { left };
        return v.add( right );
    }

    // Subtract vectors
    template <typename T>
    constexpr inline vec3dTemplate<T> operator-( const vec3dTemplate<T>& left,
                                                 const vec3dTemplate<T>& right )
    {
        vec3dTemplate<T> v { left };
        return v.subtract( right );
    }

    // Multiply vectors
    template <typename T>
    constexpr inline vec3dTemplate<T> operator*( const vec3dTemplate<T>& left,
                                                 const vec3dTemplate<T>& right )
    {
        vec3dTemplate<T> v { left };
        return v.multiply( right );
    }

    // Divide vectors
    template <typename T>
    constexpr inline vec3dTemplate<T> operator/( const vec3dTemplate<T>& left,
                                                 const vec3dTemplate<T>& right )
    {
        vec3dTemplate<T> v { left };
        return v.divide( right );
    }

    //****************************************************************
    // Friend operators

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
