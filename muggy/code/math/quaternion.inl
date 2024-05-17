//********************************************************************
//  File:    quaternion.inl
//  Date:    Thu, 16 May 2024: 01:12
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
    constexpr quaternion<T>::quaternion( void )
     :
        i ( T( 0 ) ),
        j ( T( 0 ) ),
        k ( T( 0 ) ),
        scalar ( T( 0 ) )
    {}

    template <typename T>
    constexpr quaternion<T>::quaternion( const quaternion<T>& _q )
     :
        i ( _q.x ),
        j ( _q.y ),
        k ( _q.z ),
        scalar ( _q.scalar )
    {}

    template <typename T>
    constexpr quaternion<T>::quaternion( const vec4dTemplate<T>& _v )
     :
        i ( _v.x ),
        j ( _v.y ),
        k ( _v.z ),
        scalar ( _v.w )
    {}

    template <typename T>
    constexpr quaternion<T>::quaternion( const T (&_arr)[4] )
     : 
        x( _arr[0] ),
        y( _arr[1] ),
        z( _arr[2] ),
        w( _arr[3] )
    {}

    //****************************************************************
    // Operators

    // Assignment
    template <typename T>
    constexpr inline quaternion<T>& quaternion<T>::operator=( const vec4dTemplate<T>& v )
    {
        // Using vec4dTemplate assignment
        this->vec = v;

        return *this;
    }

    template <typename T>
    constexpr inline quaternion<T>& quaternion<T>::operator=( const quaternion<T>& q )
    {
        this->i = q.i;
        this->j = q.j;
        this->k = q.k;
        this->scalar = q.scalar;

        return *this;
    }

    //****************************************************************
    // Quaternion operations

    // Rotate a vector with a unit quaternion
    template <typename T>
    constexpr inline vec3dTemplate<T> rotateVector3D( vec4dTemplate<T> vec,
                                                      vec4dTemplate<T> quaternion )
    {
        vec4dTemplate<T> V { vec.x, vec.y, vec.z, 0 };
        // Calculate the conjugate to the provided quaternion
        vec4dTemplate<T> Q { quaternionConjugate( quaternion ) };
        // Multiply the conjugate with our vector
        vec4dTemplate<T> result = quaternionMultiply( Q, V );
        // Finally multiply the result of conjugate*Vector with the
        // original quaternion to get the result
        result = quaternionMultiply( result, quaternion );
        return vec3dTemplate<T>( result.x, result.y, result.z );
    }

    // Multiply quaternions
    // Q1 = w + xi + yj + zk => [ x, y, z, w ] in vector form
    // Q2 = w + xi + yj + zk => [ x, y, z, w ] in vector form
    //
    // The following rules apply:
    //
    // i x j = k, j x k = i, k x i = j and
    // i*i = j*j = k*k = -1
    //
    // Now, if we multiply Q2 with Q1, as Q2*Q1 we get:
    //
    // ( Q2.w * Q1.w ) + i( Q2.w * Q1.x ) + j( Q2.w * Q1.y ) + k( Q2.w * Q1.z ) +
    // i( Q2.x * Q1.w ) + ii( Q2.x * Q1.x ) + ij( Q2.x * Q1.y ) + ik( Q2.x * Q1.z ) +
    // j( Q2.y * Q1.w ) + ji( Q2.y * Q1.x ) + jj( Q2.y * Q1.y ) + jk( Q2.y * Q1.z ) +
    // k( Q2.z * Q1.w ) + ki( Q2.z * Q1.x ) + kj( Q2.z * Q1.y ) + kk( Q2.z * Q1.z )
    //
    // If we now sort the elements according to which vector (i, j and
    // k) they belong to. Of course applying the rules above, we get:
    //
    // [ i ] = ( Q2.w * Q1.x ) + ( Q2.x * Q1.w ) + ( Q2.y * Q1.z ) - ( Q2.z * Q1.y )
    // [ j ] = ( Q2.w * Q1.y ) - ( Q2.x * Q1.z ) + ( Q2.y * Q1.w ) + ( Q2.z * Q1.x )
    // [ k ] = ( Q2.w * Q1.z ) + ( Q2.x * Q1.y ) - ( Q2.y * Q1.x ) + ( Q2.z * Q1.w )
    // [ s ] = ( Q2.w * Q1.w ) - ( Q2.x * Q1.x ) - ( Q2.y * Q1.y ) - ( Q2.z * Q1.z )
    //
    // And this is what we are implementing here
    template <typename T>
    constexpr inline vec4dTemplate<T> quaternionMultiply( vec4dTemplate<T> Q1,
                                                          vec4dTemplate<T> Q2 )
    {
        quaternion<T> q;
        q.i = ( Q2.w * Q1.x ) + ( Q2.x * Q1.w ) + ( Q2.y * Q1.z ) - ( Q2.z * Q1.y );
        q.j = ( Q2.w * Q1.y ) - ( Q2.x * Q1.z ) + ( Q2.y * Q1.w ) + ( Q2.z * Q1.x );
        q.k = ( Q2.w * Q1.z ) + ( Q2.x * Q1.y ) - ( Q2.y * Q1.x ) + ( Q2.z * Q1.w );
        q.w = ( Q2.w * Q1.w ) - ( Q2.x * Q1.x ) - ( Q2.y * Q1.y ) - ( Q2.z * Q1.z );

        return q.vec;
    }

    // Quaternion conjugate
    // For a quaternion (Q), the conjugate (Q*) is define as:
    // Q = [ scalar, vector ], Q* = [ scalar, -vector ]
    // This is identical to a complex numbers conjugate which
    // essentially mean that we just flip the signs of the vector
    //
    template <typename T>
    constexpr inline vec4dTemplate<T> quaternionConjugate( vec4dTemplate<T> quaternion )
    {
        vec4dTemplate<T> result;
        result.x = -quaternion.x;
        result.y = -quaternion.y;
        result.z = -quaternion.z;
        result.w = -quaternion.w;

        return result;
    }

    // Quaternion conversion from roll, pith and yaw
    template <typename T>
    constexpr inline quaternion<T> toQuaternionFromEuler( const T& roll,
                                                          const T& pitch,
                                                          const T& yaw )
    {
        // NOTE(klek): This implementation was yoinked from wikipedia
        // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Source_code
        
        // We assert for any type that is not float or double
        static_assert( std::is_floating_point<T>::value, "T must be float or double");

        // Roll, pitch and yaw angles are assumed to be in radian
        quaternion<T> q;

        T cr = maths_cos( roll * 0.5f );
        T sr = maths_sin( roll * 0.5f );
        T cp = maths_cos( pitch * 0.5f );
        T sp = maths_sin( pitch * 0.5f );
        T cy = maths_cos( yaw * 0.5f );
        T sy = maths_sin( yaw * 0.5f );

        q.x = ( sr * cp * cy ) - ( cr * sp * sy );
        q.y = ( cr * sp * cy ) + ( sr * cp * sy );
        q.z = ( cr * cp * sy ) - ( sr * sp * cy );
        q.scalar = ( cr * cp * cy ) + ( sr * sp * sy );

        return q;
    }

    // Quaternion conversion from roll, pith and yaw in 3d-vector form
    template <typename T>
    constexpr inline quaternion<T> toQuaternionFromEuler( const vec3dTemplate<T>& euler )
    {
        // We assert for any type that is not float or double
        static_assert( std::is_floating_point<T>::value, "T must be float or double");

        return toQuaternionFromEuler( euler.x, euler.y, euler.z );
    }

} // namespace muggy::math
