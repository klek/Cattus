//********************************************************************
//  File:    vec4dTemplate.inl
//  Date:    Sun, 19 Nov 2023: 00:47
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

    //****************************************************************
    // Member functions

    // Vector add
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::add( const vec4dTemplate<T>& other )
    {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        this->w += other.w;

        return *this;
    }

    // Vector subtract
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::subtract( const vec4dTemplate<T>& other )
    {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        this->w -= other.w;

        return *this;
    }

    // Vector multiply
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::multiply( const vec4dTemplate<T>& other )
    {
        this->x *= other.x;
        this->y *= other.y;
        this->z *= other.z;
        this->w *= other.w;

        return *this;
    }

    // Vector divide
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::divide( const vec4dTemplate<T>& other )
    {
        this->x /= other.x;
        this->y /= other.y;
        this->z /= other.z;
        this->w /= other.w;

        return *this;
    }

    // Vector add with scalar
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::add( const T& scalar )
    {
        this->x += scalar;
        this->y += scalar;
        this->z += scalar;
        this->w += scalar;

        return *this;
    }

    // Vector subtract with scalar
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::subtract( const T& scalar )
    {
        this->x -= scalar;
        this->y -= scalar;
        this->z -= scalar;
        this->w -= scalar;

        return *this;
    }

    // Vector multiply with scalar
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::multiply( const T& scalar )
    {
        this->x *= scalar;
        this->y *= scalar;
        this->z *= scalar;
        this->w *= scalar;

        return *this;
    }

    // Vector divide with scalar
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::divide( const T& scalar )
    {
        this->x /= scalar;
        this->y /= scalar;
        this->z /= scalar;
        this->w /= scalar;

        return *this;
    }

    // Returns the dot-product of itself with other
    template <typename T>
    constexpr inline T vec4dTemplate<T>::dot( const vec4dTemplate<T>& other ) const
    {
        return T( ( x * other.x ) + ( y * other.y ) + ( z * other.z ) + ( w * other.w ) );
    }

    // Returns the dot-product of itself
    template <typename T>
    constexpr inline T vec4dTemplate<T>::dot( void ) const
    {
        return T( ( x * x ) + ( y * y ) + ( z * z ) + ( w * w ) );
    }

    // Returns the cross-product of itself cross other
    template <typename T>
    constexpr inline vec4dTemplate<T> vec4dTemplate<T>::cross( const vec4dTemplate<T>& other ) const 
    {
        T a, b, c, d;
        a = ( this->y * other.z ) - ( this->z * other.y );
        b = ( this->z * other.x ) - ( this->x * other.z );
        c = ( this->x * other.y ) - ( this->y * other.x );
        // NOTE(klek): Cross-product in four dimensional space does
        //             not necessary result in an orthogonal vector
        //             to the starting plane. Thus we set the w
        //             component to 0 to indicate that this new vector
        //             belongs to 3 dimensions
        d = 0.0f;
        
        return vec4dTemplate<T>( a, b, c, d );
    }

    // Returns the length of itself by calculating the square root of
    // the dot-product of itself
    template <typename T>
    constexpr inline T vec4dTemplate<T>::length( void ) const
    {
        return T( maths_sqrt( dot() ) );
    }

    // Return itself after normalizing
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::normalize( void )
    {
        T n = dot( );
        if ( n > 0 )
        {
            T factor = 1 / length( );
            this->x *= factor;
            this->y *= factor;
            this->z *= factor;
            // TODO(klek): Verify if we really need to normalize w
            this->w *= factor;
        }

        return *this;
    }

    //****************************************************************
    // Operators

    // Assignment
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::operator=( const vec4dTemplate<T>& vec )
    {
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
        this->w = vec.w;

        return *this;
    }

    // Add with other vector
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::operator+=( const vec4dTemplate<T>& other )
    {
        return this->add( other );
    }

    // Subtract with other vector
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::operator-=( const vec4dTemplate<T>& other )
    {
        return this->subtract( other );
    }

    // Multiply with other vector
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::operator*=( const vec4dTemplate<T>& other )
    {
        return this->multiply( other );
    }

    // Divide with other vector
    template <typename T>
    constexpr inline vec4dTemplate<T>& vec4dTemplate<T>::operator/=( const vec4dTemplate<T>& other )
    {
        return this->divide( other );
    }

    // Boolean operators
    template <typename T>
    constexpr inline bool vec4dTemplate<T>::operator==( const vec4dTemplate<T> other ) const
    {
        return ( this->x == other.x &&
                 this->y == other.y &&
                 this->z == other.z &&
                 this->w == other.w    );
    }

    template <typename T>
    constexpr inline bool vec4dTemplate<T>::operator!=( const vec4dTemplate<T> other ) const
    {
        return !( *this == other );
    }

    //****************************************************************
    // General operators

    // Add vectors
    template <typename T>
    constexpr inline vec4dTemplate<T> operator+( const vec4dTemplate<T>& left,
                                                 const vec4dTemplate<T>& right )
    {
        vec4dTemplate<T> v { left };
        return v.add( right );
    }

    // Subtract vectors
    template <typename T>
    constexpr inline vec4dTemplate<T> operator-( const vec4dTemplate<T>& left,
                                                 const vec4dTemplate<T>& right )
    {
        vec4dTemplate<T> v { left };
        return v.subtract( right );
    }

    // Multiply vectors
    template <typename T>
    constexpr inline vec4dTemplate<T> operator*( const vec4dTemplate<T>& left,
                                                 const vec4dTemplate<T>& right )
    {
        vec4dTemplate<T> v { left };
        return v.multiply( right );
    }

    // Divide vectors
    template <typename T>
    constexpr inline vec4dTemplate<T> operator/( const vec4dTemplate<T>& left,
                                                 const vec4dTemplate<T>& right )
    {
        vec4dTemplate<T> v { left };
        return v.divide( right );
    }

    //****************************************************************
    // Friend operators

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
