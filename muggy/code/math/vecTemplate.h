//********************************************************************
//  File:    vecTemplate.h
//  Date:    Wed, 15 May 2024: 21:29
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VEC_TEMPLATE_H)
#define VEC_TEMPLATE_H

#include <iostream>

namespace muggy::math
{
    //****************************************************************
    // 2-dimensional vector
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

    //****************************************************************
    // 3-dimensional vector
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

    //****************************************************************
    // 4-dimensional vector
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
    template <typename T>
    struct quaternion
    {
        typedef T vType;
        typedef vec4dTemplate<T> vec4Type;

        // Member variables
        union 
        {
            struct{ 
                vType i, j, k, scalar; 
            };
            struct{
                vType x, y, z, w;
            };
            vec4Type vec;
        };

        //************************************************************
        // Constructors
        constexpr quaternion( void );
        constexpr quaternion( const quaternion<T>& _q );
        constexpr quaternion( const vec4Type& _v );
        constexpr explicit quaternion( const vType (&_arr)[4] );

        //************************************************************
        // Operators
        // Assignment
        constexpr inline quaternion<T>& operator=( const vec4Type& v );
        constexpr inline quaternion<T>& operator=( const quaternion& q );
    };

    //****************************************************************
    // Quaternion operations

    // Rotate a vector with a unit quaternion
    template <typename T>
    constexpr inline vec3dTemplate<T> rotateVector3D( vec4dTemplate<T> vec,
                                                      vec4dTemplate<T> quaternion );

    // Multiply quaternions
    template <typename T>
    constexpr inline vec4dTemplate<T> quaternionMultiply( vec4dTemplate<T> quaternion1,
                                                          vec4dTemplate<T> quaternion2 );

    // Quaternion conjugate
    template <typename T>
    constexpr inline vec4dTemplate<T> quaternionConjugate( vec4dTemplate<T> quaternion );

    // Quaternion conversion from roll, pith and yaw
    // This also supports roll, pitch and yaw in 3d-vector form
    template <typename T>
    constexpr inline quaternion<T> toQuaternionFromEuler( const T& roll,
                                                          const T& pitch,
                                                          const T& yaw );
    template <typename T>
    constexpr inline quaternion<T> toQuaternionFromEuler( const vec3dTemplate<T>& euler );

} // namespace muggy::math

#ifndef USE_MATH_EXTERNAL
#include "vec2dTemplate.inl"
#include "vec3dTemplate.inl"
#include "vec4dTemplate.inl"
#include "quaternion.inl"
#endif

#endif