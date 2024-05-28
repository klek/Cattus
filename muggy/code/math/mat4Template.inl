//********************************************************************
//  File:    mat4Template.inl
//  Date:    Sun, 19 Nov 2023: 03:11
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "mat4Template.h"

namespace muggy::math
{
    //****************************************************************
    // Constructors

    // Default constructor, initializes all elements to 0
    template <typename T>
    constexpr mat4Template<T>::mat4Template( ) 
    {
        for ( int i = 0; i < FOUR_BY_FOUR; i++ )
        {
            elements[ i ] = T( 0 );
        }
    }

    // Constructor that initialize all elements to zero and then sets
    // the diagonal (top-left to bottom-right) to the provided value
    template <typename T>
    constexpr mat4Template<T>::mat4Template( T diagonal )
    {
        for ( int i = 0; i < FOUR_BY_FOUR; i++ )
        {
            elements[ i ] = T( 0 );
        }

        // Set the diagonal
        m[0][0] = diagonal;
        m[1][1] = diagonal;
        m[2][2] = diagonal;
        m[3][3] = diagonal;
    }

    // Copy-constructor that constructs a matrix by copying from another
    // matrix.
    template <typename T>
    constexpr mat4Template<T>::mat4Template( const mat4Template<T>& mat ) 
    {
        *this = mat;
    }

    // Constructs a new matrix from provided values
    template <typename T>
    constexpr mat4Template<T>::mat4Template( T m00, T m01, T m02, T m03,
                                             T m10, T m11, T m12, T m13,
                                             T m20, T m21, T m22, T m23,
                                             T m30, T m31, T m32, T m33 )
    {
        m[0][0] = m00;
        m[0][1] = m01;
        m[0][2] = m02;
        m[0][3] = m03;

        m[1][0] = m10;
        m[1][1] = m11;
        m[1][2] = m12;
        m[1][3] = m13;

        m[2][0] = m20;
        m[2][1] = m21;
        m[2][2] = m22;
        m[2][3] = m23;

        m[3][0] = m30;
        m[3][1] = m31;
        m[3][2] = m32;
        m[3][3] = m33;
    }

    //****************************************************************
    // Member functions

    // Returns the specified column-index as a vec4Type. If the index
    // is larger than the size of the matrix, an empty vec4Type is
    // returned
    template <typename T>
    constexpr inline vec4dTemplate<T> mat4Template<T>::getColumn( int index ) const
    {
        if ( index < 4 )
        {
            return ( vec4dTemplate<T>( m[0][ index ], 
                                       m[1][ index ], 
                                       m[2][ index ], 
                                       m[3][ index ] ) );
        }

        return vec4dTemplate<T>( );
    }

    // Returns the specified row-indes as a vec4Type. If the index
    // is larger than the size of the matrix, an empty vec4Type is
    // returned
    template <typename T>
    constexpr inline vec4dTemplate<T> mat4Template<T>::getRow( int index ) const
    {
        if ( index < 4 )
        {
            return ( vec4dTemplate<T>( m[ index ][0], 
                                       m[ index ][1], 
                                       m[ index ][2], 
                                       m[ index ][3] ) );
        }

        return vec4dTemplate<T>( );
    }

    // Returns the specified element-index as T-type. If the index
    // is larger that the size of the matrix, an empty T-type is
    // returned
    template <typename T>
    constexpr inline T mat4Template<T>::getElement( int index ) const
    {
        if ( index < FOUR_BY_FOUR )
        {
            return elements[ index ];
        }

        return ( T( 0 ) );
    }

    // Returns a transposed version of the current matrix as a new
    // matrix. The original matrix is not changed
    template <typename T>
    constexpr inline mat4Template<T> mat4Template<T>::transpose( void ) const
    {
        mat4Template<T> result { };

        result.m[0][0] = m[0][0];
        result.m[1][0] = m[0][1];
        result.m[2][0] = m[0][2];
        result.m[3][0] = m[0][3];

        result.m[0][1] = m[1][0];
        result.m[1][1] = m[1][1];
        result.m[2][1] = m[1][2];
        result.m[3][1] = m[1][3];

        result.m[0][2] = m[2][0];
        result.m[1][2] = m[2][1];
        result.m[2][2] = m[2][2];
        result.m[3][2] = m[2][3];

        result.m[0][3] = m[3][0];
        result.m[1][3] = m[3][1];
        result.m[2][3] = m[3][2];
        result.m[3][3] = m[3][3];

        return result;
    }

    // Returns a reference to the transposed version of the current
    // matrix. This function changes the current matrix to its
    // transpose
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::transposeMe( void )
    {
        // Cache the transpose of the current matrix
        mat4Template<T> transposed { transpose() };

        *this = transposed;
        return *this;
    }

    // Returns the inverted version of the current matrix as a new
    // matrix. The original matrix is not changed
    // This matrix inversion has been implemented using Gauss-Jordan
    // elimination, read more on:
    // https://en.wikipedia.org/wiki/Gaussian_elimination#Finding_the_inverse_of_a_matrix
    // or:
    // https://scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/matrix-inverse/matrix-inverse.html
    //
    template <typename T>
    constexpr inline mat4Template<T> mat4Template<T>::inverse( void ) const
    {
        // TODO(klek): Implement this!
        // Local variables
        mat4Template<T> inv { 1.0f };
        mat4Template<T> M{ *this };
        uint32_t row = 0;
        uint32_t col = 0;
        //uint32_t it = 0;
        uint32_t bigRow = 0;
        T pvtVal = 0;

        // Iterate through every column
        for ( col = 0; col < 4; col++ )
        {
            // Find the largest element in the current column, if this
            // element is not in the diagonal spot (pivot spot) we
            // need to swap rows
            // NOTE(klek): We only search below the current pivot spot
            //             in order not to move around elements in the
            //             first column

            // Use this columns pivot spot as the starting value
            bigRow = col;
            pvtVal = M.m[col][col];

            // Loop through the rows
            for ( row = col + 1; row < 4; row++ )
            {
                // Check if the current pivot value is greater than
                // the current rows value
                if ( math::maths_abs( M.m[row][col] ) > math::maths_abs( M.m[bigRow][col] ) )
                {
                    // Found a larger number, store the row number
                    bigRow = row;
                    pvtVal = M.m[row][col];
                }
            }

            // We should now know which row contains the largest number
            // for this column.
            // First check that this value is not zero (otherwise this
            // is a singular matrix)
            if ( isApproxZero( pvtVal ) )
            {
                // Matrix is singular, return the identity matrix
                return mat4Template<T>{ 1.0f };
            }

            // Second swap the row that contains this number into the
            // current pivot spot (ie the diagonal spot)
            if ( bigRow != col )
            {
                T temp;
                // Swap the rows
                for ( uint32_t j = 0; j < 4; j++ )
                {
                    // For the working matrix
                    temp = M.m[col][j];
                    M.m[col][j] = M.m[bigRow][j];
                    M.m[bigRow][j] = temp;
                    // For the inverted matrix
                    temp = inv.m[col][j];
                    inv.m[col][j] = inv.m[bigRow][j];
                    inv.m[bigRow][j] = temp;
                }
            }

            // Now we need to eliminate all numbers below the pivot
            // spot (not the pivot itself), in this column. This means
            // every slot starting from [col+1][col], the diagonal
            // NOTE(klek): We also need to do the same modification
            //             to all column-members of every row, thus
            //             the double loop
            for ( uint32_t i = col + 1; i < 4; i++ )
            {
                // NOTE(klek): i is the current row we are working on
                //             j is the current column we are working
                //             on, in the ith row

                //T coeff = M.m[i][col] / M.m[col][col];
                T coeff = M.m[i][col] / pvtVal;
                for ( uint32_t j = 0; j < 4; j++ )
                {
                    // For the working matrix
                    M.m[i][j] -= coeff * M.m[col][j];
                    // For the inverted matrix
                    inv.m[i][j] -= coeff * inv.m[col][j];
                }
            }

            // Last thing to do before we go to the next column is to
            // divide the current row with its pivot value, which
            // should result in the actual pivot value being 1
            for ( uint32_t j = 0; j < 4; j++ )
            {
                // NOTE(klek): We use col as the row indicator here
                // For the working matrix
                M.m[col][j] /= pvtVal;
                // For the inverted matrix
                inv.m[col][j] /= pvtVal;
            }
        }
        // Here we should now have a matrix in row-echelon form;
        //
        //      1   a   b   c
        //      0   1   d   e
        //      0   0   1   f
        //      0   0   0   1
        //
        // which means that we only have to solve the remaining
        // matrix with back substitution

        // Continue with the upper part of the matrix and solve this
        // by back substitution. Here we iterate by row instead of 
        // column
        // NOTE(klek): The last row is already done (should be 0, 0,
        //             0, 1)
        for ( row = 0; row < 3; row++ )
        {
            // NOTE(klek): Reducing row by row, using the row below
            //             the current one to remove each element
            //             This utilizes the pivot value which is 1,
            //             to guarantee the removal of the desired
            //             element
            // NOTE(klek): The first column is already done, hence we
            //             start with row + 1 to always be above the
            //             pivot spot
            for ( uint32_t j = row + 1; j < 4; j++ )
            {
                // Grab the element we want to reduce
                T coeff = M.m[row][j];
                // Iterate over every column in this row such that
                // we calculate the entire row
                for ( col = 0; col < 4; col++ )
                {
                    // Subtract the coefficient multiplied by the
                    // element on the jth row (essentially the row
                    // below the current one)
                    M.m[row][col] -= coeff * M.m[j][col];
                    inv.m[row][col] -= coeff * inv.m[j][col];
                }
            }
        }

        return inv;
    }

    // Returns a reference to the inverted version of the current
    // matrix. This function changes the current matrix to its
    // inverted version!
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::invertMe( void )
    {
        mat4Template<T> inverted { inverse() };

        *this = inverted;
        return *this;
    }

    // Returns the determinant of the matrix
    //
    //      | a   b   c   d |       | m00 m01 m02 m03 |
    //  M = | e   f   g   h |   =   | m10 m11 m12 m13 |
    //      | i   j   k   l |       | m20 m21 m22 m23 |
    //      | m   n   o   p |       | m30 m31 m32 m33 |
    //
    // det(M) = afkp - afol - agjp + agnl + ahjo - ahnk -
    //          bekp + beol + bgip - bgml - bhio + bhmk +
    //          cejp - cenl - cfip + cfml + chin - chmj -
    //          dejo + denk + dfio - dfmk - dgin + dgmj
    template <typename T>
    constexpr inline T mat4Template<T>::determinant( void ) const
    {
        // TODO(klek): Implement this!
        return ( m[0][0] * m[1][1] * m[2][2] * m[3][3] -    // afkp
                 m[0][0] * m[1][1] * m[3][2] * m[2][3] -    // afpö
                 m[0][0] * m[1][2] * m[2][1] * m[3][3] +    // agjp
                 m[0][0] * m[1][2] * m[3][1] * m[2][3] +    // agnl
                 m[0][0] * m[1][3] * m[2][1] * m[3][2] -    // ahjo
                 m[0][0] * m[1][3] * m[3][1] * m[2][2] -    // ahnk
                 m[0][1] * m[1][0] * m[2][2] * m[3][3] +    // bekp
                 m[0][1] * m[1][0] * m[3][2] * m[2][3] +    // beol
                 m[0][1] * m[1][2] * m[2][0] * m[3][3] -    // bgip
                 m[0][1] * m[1][2] * m[3][0] * m[2][3] -    // bgml
                 m[0][1] * m[1][3] * m[2][0] * m[3][2] +    // bhio
                 m[0][1] * m[1][3] * m[3][0] * m[2][2] +    // bhmk
                 m[0][2] * m[1][0] * m[2][1] * m[3][3] -    // cejp
                 m[0][2] * m[1][0] * m[3][1] * m[2][3] -    // cenl
                 m[0][2] * m[1][1] * m[2][0] * m[3][3] +    // cfip
                 m[0][2] * m[1][1] * m[3][0] * m[2][3] +    // cfml
                 m[0][2] * m[1][3] * m[2][0] * m[3][1] -    // chin
                 m[0][2] * m[1][3] * m[3][0] * m[2][1] -    // chmj
                 m[0][3] * m[1][0] * m[2][1] * m[3][2] +    // dejo
                 m[0][3] * m[1][0] * m[3][1] * m[2][2] +    // denk
                 m[0][3] * m[1][1] * m[2][0] * m[3][2] -    // dfio
                 m[0][3] * m[1][1] * m[3][0] * m[2][2] -    // dfmk
                 m[0][3] * m[1][2] * m[2][0] * m[3][1] +    // dgin
                 m[0][3] * m[1][2] * m[3][0] * m[2][1]  );  // dgmj
    }

    // Matrix addition
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::add( const mat4Template<T>& rhs )
    {
        this->rows[0].add( rhs.rows[0] );
        this->rows[1].add( rhs.rows[1] );
        this->rows[2].add( rhs.rows[2] );
        this->rows[3].add( rhs.rows[3] );

        return *this;
    }

    // Matrix subtraction
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::subtract( const mat4Template<T>& rhs )
    {
        this->rows[0].subtract( rhs.rows[0] );
        this->rows[1].subtract( rhs.rows[1] );
        this->rows[2].subtract( rhs.rows[2] );
        this->rows[3].subtract( rhs.rows[3] );

        return *this;
    }

    // Matrix multiplikation:
    // Row x column is the way we multiply matrices. Thus we calculate
    // each row for the resulting matrix by first caching the data and
    // then using this for each row calculation
    // TODO(klek): Verify that this implementation is actually correct
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::multiply( const mat4Template<T>& rhs ) 
    {
        // Temporary storage
        //mat4Template<T> result { };

        // Cache first row in variables
        T x = m[0][0];
        T y = m[0][1];
        T z = m[0][2];
        T w = m[0][3];

        // Calculate the first row
        m[0][0] = ( x * rhs.m[0][0] ) + ( y * rhs.m[1][0] ) + ( z * rhs.m[2][0] ) + ( w * rhs.m[3][0] );
        m[0][1] = ( x * rhs.m[0][1] ) + ( y * rhs.m[1][1] ) + ( z * rhs.m[2][1] ) + ( w * rhs.m[3][1] );
        m[0][2] = ( x * rhs.m[0][2] ) + ( y * rhs.m[1][2] ) + ( z * rhs.m[2][2] ) + ( w * rhs.m[3][2] );
        m[0][3] = ( x * rhs.m[0][3] ) + ( y * rhs.m[1][3] ) + ( z * rhs.m[2][3] ) + ( w * rhs.m[3][3] );

        // Cache second row in variables
        x = m[1][0];
        y = m[1][1];
        z = m[1][2];
        w = m[1][3];

        // Calculate the second row
        m[1][0] = ( x * rhs.m[0][0] ) + ( y * rhs.m[1][0] ) + ( z * rhs.m[2][0] ) + ( w * rhs.m[3][0] );
        m[1][1] = ( x * rhs.m[0][1] ) + ( y * rhs.m[1][1] ) + ( z * rhs.m[2][1] ) + ( w * rhs.m[3][1] );
        m[1][2] = ( x * rhs.m[0][2] ) + ( y * rhs.m[1][2] ) + ( z * rhs.m[2][2] ) + ( w * rhs.m[3][2] );
        m[1][3] = ( x * rhs.m[0][3] ) + ( y * rhs.m[1][3] ) + ( z * rhs.m[2][3] ) + ( w * rhs.m[3][3] );

        // Cache third row in variables
        x = m[2][0];
        y = m[2][1];
        z = m[2][2];
        w = m[2][3];

        // Calculate the third row
        m[2][0] = ( x * rhs.m[0][0] ) + ( y * rhs.m[1][0] ) + ( z * rhs.m[2][0] ) + ( w * rhs.m[3][0] );
        m[2][1] = ( x * rhs.m[0][1] ) + ( y * rhs.m[1][1] ) + ( z * rhs.m[2][1] ) + ( w * rhs.m[3][1] );
        m[2][2] = ( x * rhs.m[0][2] ) + ( y * rhs.m[1][2] ) + ( z * rhs.m[2][2] ) + ( w * rhs.m[3][2] );
        m[2][3] = ( x * rhs.m[0][3] ) + ( y * rhs.m[1][3] ) + ( z * rhs.m[2][3] ) + ( w * rhs.m[3][3] );

        // Cache fourth row in variables
        x = m[3][0];
        y = m[3][1];
        z = m[3][2];
        w = m[3][3];

        // Calculate the fourth row
        m[3][0] = ( x * rhs.m[0][0] ) + ( y * rhs.m[1][0] ) + ( z * rhs.m[2][0] ) + ( w * rhs.m[3][0] );
        m[3][1] = ( x * rhs.m[0][1] ) + ( y * rhs.m[1][1] ) + ( z * rhs.m[2][1] ) + ( w * rhs.m[3][1] );
        m[3][2] = ( x * rhs.m[0][2] ) + ( y * rhs.m[1][2] ) + ( z * rhs.m[2][2] ) + ( w * rhs.m[3][2] );
        m[3][3] = ( x * rhs.m[0][3] ) + ( y * rhs.m[1][3] ) + ( z * rhs.m[2][3] ) + ( w * rhs.m[3][3] );

        // And we are done
        return *this;
    }

    // Matrix addition with scalar
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::add( const T& scalar )
    {
        this->rows[0].add( scalar );
        this->rows[1].add( scalar );
        this->rows[2].add( scalar );
        this->rows[3].add( scalar );

        return *this;
    }

    // Matrix subtraction with scalar
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::subtract( const T& scalar )
    {
        this->rows[0].subtract( scalar );
        this->rows[1].subtract( scalar );
        this->rows[2].subtract( scalar );
        this->rows[3].subtract( scalar );

        return *this;
    }

    // Matrix division with scalar
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::divide( const T& scalar )
    {
        this->rows[0].divide( scalar );
        this->rows[1].divide( scalar );
        this->rows[2].divide( scalar );
        this->rows[3].divide( scalar );

        return *this;
    }

    // Matrix multiplication with scalar
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::multiply( const T& scalar )
    {
        this->rows[0].multiply( scalar );
        this->rows[1].multiply( scalar );
        this->rows[2].multiply( scalar );
        this->rows[3].multiply( scalar );
        
        return *this;
    }

    // Matrix4x4-Vector3d multiplication ( vector as in a direction )
    // NOTE(klek): Since we scale with w, this function should also
    // work with points
    //
    // Matrix 4x4 multiplied by vector 4x1 gives a vector 4x1 as:
    //      ( x = m00 * x + m01 * y + m02 * z + m03 * w )
    //      ( y = m10 * x + m11 * y + m12 * z + m13 * w )
    //      ( z = m20 * x + m21 * y + m22 * z + m23 * w )
    //      ( w = m30 * x + m31 * y + m32 * z + m33 * w )
    //
    // We multiply row * column, and since the vector is a single column, it
    // will be multiplied with every row in the matrix
    template <typename T>
    constexpr inline vec3dTemplate<T> mat4Template<T>::multiplyMatVec( const vec3dTemplate<T>& rhs ) const
    {
        T x, y, z, w;

        // NOTE(klek): Vec3d does not have a w, so we add w = 1
        //             and then we scale x, y and z with w in the end
        x = m[0][0] * rhs.x + m[0][1] * rhs.y + m[0][2] * rhs.z + m[0][3];
        y = m[1][0] * rhs.x + m[1][1] * rhs.y + m[1][2] * rhs.z + m[1][3];
        z = m[2][0] * rhs.x + m[2][1] * rhs.y + m[2][2] * rhs.z + m[2][3];
        w = m[3][0] * rhs.x + m[3][1] * rhs.y + m[3][2] * rhs.z + m[3][3];

        // Normalize
        x /= w;
        y /= w;
        z /= w;
        
        return ( vec3dTemplate<T>( x, y, z ) );
    }

    // Vector3d-Matrix4x4 multiplication ( vector as in a direction )
    // NOTE(klek): Since we scale with w, this function should also
    // work with points
    //
    // Vector 4x1 multiplied by matrix 4x4 multiplied gives a vector 1x4 as:
    //      ( x = x * m00 + y * m10 + z * m20 + w * m30 )
    //      ( y = x * m01 + y * m11 + z * m21 + w * m31 )
    //      ( z = x * m02 + y * m12 + z * m22 + w * m32 )
    //      ( w = x * m03 + y * m13 + z * m23 + w * m33 )
    //
    // We multiply row * column, and since the vector is a single row, it
    // will be multiplied with every column in the matrix
    template <typename T>
    constexpr inline vec3dTemplate<T> mat4Template<T>::multiplyVecMat( const vec3dTemplate<T>& lhs ) const
    {
        T x, y, z, w;

        // NOTE(klek): Vec3d does not have a w, so we add w = 1
        //             and then we scale x, y and z with w in the end
        x = lhs.x * m[0][0] + lhs.y * m[1][0] + lhs.z * m[2][0] + lhs.w * m[3][0];
        y = lhs.x * m[0][1] + lhs.y * m[1][1] + lhs.z * m[2][1] + lhs.w * m[3][1];
        z = lhs.x * m[0][2] + lhs.y * m[1][2] + lhs.z * m[2][2] + lhs.w * m[3][2];
        w = lhs.x * m[0][3] + lhs.y * m[1][3] + lhs.z * m[2][3] + lhs.w * m[3][3];

        // Normalize
        x /= w;
        y /= w;
        z /= w;
        
        return ( vec3dTemplate<T>( x, y, z ) );
    }

    // Matrix4x4-Vector4d multiplication ( vector as in a direction )
    //
    // Matrix 4x4 multiplied by vector 4x1 gives a vector 4x1 as:
    //      ( x = m00 * x + m01 * y + m02 * z + m03 * w )
    //      ( y = m10 * x + m11 * y + m12 * z + m13 * w )
    //      ( z = m20 * x + m21 * y + m22 * z + m23 * w )
    //      ( w = m30 * x + m31 * y + m32 * z + m33 * w )
    //
    // We multiply row * column, and since the vector is a single column, it
    // will be multiplied with every row in the matrix
    template <typename T>
    constexpr inline vec4dTemplate<T> mat4Template<T>::multiplyMatVec( const vec4dTemplate<T>& rhs ) const
    {
        T x, y, z, w;

        x = m[0][0] * rhs.x + m[0][1] * rhs.y + m[0][2] * rhs.z + m[0][3] * rhs.w;
        y = m[1][0] * rhs.x + m[1][1] * rhs.y + m[1][2] * rhs.z + m[1][3] * rhs.w;
        z = m[2][0] * rhs.x + m[2][1] * rhs.y + m[2][2] * rhs.z + m[2][3] * rhs.w;
        w = m[3][0] * rhs.x + m[3][1] * rhs.y + m[3][2] * rhs.z + m[3][3] * rhs.w;

        return ( vec4dTemplate<T>( x, y, z, w ) );
    }

    // Vector4d-Matrix4x4 multiplication ( vector as in a direction )
    //
    // Vector 4x1 multiplied by matrix 4x4 multiplied gives a vector 1x4 as:
    //      ( x = x * m00 + y * m10 + z * m20 + w * m30 )
    //      ( y = x * m01 + y * m11 + z * m21 + w * m31 )
    //      ( z = x * m02 + y * m12 + z * m22 + w * m32 )
    //      ( w = x * m03 + y * m13 + z * m23 + w * m33 )
    //
    // We multiply row * column, and since the vector is a single row, it
    // will be multiplied with every column in the matrix
    template <typename T>
    constexpr inline vec4dTemplate<T> mat4Template<T>::multiplyVecMat( const vec4dTemplate<T>& lhs ) const
    {
        T x, y, z, w;

        x = lhs.x * m[0][0] + lhs.y * m[1][0] + lhs.z * m[2][0] + lhs.w * m[3][0];
        y = lhs.x * m[0][1] + lhs.y * m[1][1] + lhs.z * m[2][1] + lhs.w * m[3][1];
        z = lhs.x * m[0][2] + lhs.y * m[1][2] + lhs.z * m[2][2] + lhs.w * m[3][2];
        w = lhs.x * m[0][3] + lhs.y * m[1][3] + lhs.z * m[2][3] + lhs.w * m[3][3];

        return ( vec4dTemplate<T>( x, y, z, w ) );
    }

    //****************************************************************
    // Operators

    // Assignment operator
    template <typename T>
    constexpr mat4Template<T>& mat4Template<T>::operator=( const mat4Type& mat )
    {
        this->rows[0] = mat.rows[0];
        this->rows[1] = mat.rows[1];
        this->rows[2] = mat.rows[2];
        this->rows[3] = mat.rows[3];

        return *this;
    }

    // Add with other matrix
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::operator+=( const mat4Template<T>& other )
    {
        return this->add( other );
    }

    // Subtract with other matrix
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::operator-=( const mat4Template<T>& other )
    {
        return this->subtract( other );
    }

    // Matrix * matrix multiplication operator
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::operator*=( const mat4Template<T>& other )
    { 
        return this->multiply( other ); 
    }

    // Add with scalar
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::operator+=( const T& scalar )
    {
        return this->add( scalar );
    }

    // Subtract with scalar
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::operator-=( const T& scalar )
    {
        return this->subtract( scalar );
    }

    // Multiply with scalar
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::operator*=( const T& scalar )
    {
        return this->multiply( scalar );
    }

    // Divide with scalar
    template <typename T>
    constexpr inline mat4Template<T>& mat4Template<T>::operator/=( const T& scalar )
    {
        return this->divide( scalar );
    }

    //****************************************************************
    // General operators

    // Matrix * matrix multiplicaton operator
    template <typename T>
    constexpr inline mat4Template<T> operator*( const mat4Template<T>& left,
                                                const mat4Template<T>& right )
    {
        mat4Template<T> M { left };
        return M.multiply( right ); 
    }

    // Matrix * vec3d multiplication operator
    template <typename T>
    constexpr inline vec3dTemplate<T> operator*( const mat4Template<T>& left, 
                                                 const vec3dTemplate<T>& right )
    { 
        mat4Template<T> M { left };
        return M.multiplyMatVec( right ); 
    }

    // Vec3d * matrix multiplication operator
    template <typename T>
    constexpr inline vec3dTemplate<T> operator*( const vec3dTemplate<T>& left, 
                                                 const mat4Template<T>& right )
    { 
        mat4Template<T> M { right };
        return M.multiplyVecMat( left ); 
    }

    // Matrix * vec4d multiplication operator
    template <typename T>
    constexpr inline vec4dTemplate<T> operator*( const mat4Template<T>& left, 
                                                 const vec4dTemplate<T>& right )
    { 
        mat4Template<T> M { left };
        return M.multiplyMatVec( right ); 
    }

    // Vec4d * matrix multiplication operator
    template <typename T>
    constexpr inline vec4dTemplate<T> operator*( const vec4dTemplate<T>& left, 
                                                 const mat4Template<T>& right )
    { 
        mat4Template<T> M { right };
        return M.multiplyVecMat( left ); 
    }

    //****************************************************************
    // General purpose matrices

    // Returns the identity matrix
    // The identity matrix is a matrix with all elements of its diagonal
    // set to 1
    // The matrix looks like:
    //
    //    1     0     0     0
    //    0     1     0     0
    //    0     0     1     0
    //    0     0     0     1
    //
    template <typename T>
    constexpr mat4Template<T> identity( void ) 
    { 
        return mat4Template<T>( T( 1 ) ); 
    }

    //****************************************************************
    // Vulkan matrices
    
    // Vulkan orthographic projection matrix for off center
    // view volumes
    // This matrix is for a right handed coordinate system with the
    // x-axis pointing from left to right, the y-axis pointing from top
    // to bottom and the z-axis pointing into the screen. The matrix
    // is the product of a scale matrix multiplied by a translation
    // matrix
    // The matrix looks like:
    //
    //  2/(r-l)      0        0   -(r+l)/(r-l)
    //     0      2/(b-t)     0   -(b+t)/(b-t)
    //     0         0      1/(f-n)  -n/(f-n)
    //     0         0        0         1
    //
    // where    r = right, l = left, b = bottom, t = top,
    //          f = far, n = near
    //
    template <typename T>
    constexpr inline mat4Template<T> orthographicVkRHOffCenter( T left,
                                                                T right,
                                                                T bottom,
                                                                T top,
                                                                T near,
                                                                T far )
    {
        // Initializing the local variable with diagonal = 0
        mat4Template<T> M { 0.0f };

        M.m[0][0] = 2.0f / ( right - left );
        M.m[1][1] = 2.0f / ( bottom - top );
        M.m[2][2] = 1.0f / ( far - near );
        M.m[0][3] = -( right + left ) / ( right - left );
        M.m[1][3] = -( bottom + top ) / ( bottom - top );
        M.m[2][3] = -( near ) / ( far - near );
        M.m[3][3] = 1.0f;

        // Vulkan uses column-major ordering, so we return the
        // transposed version of this matrix
        return M.transpose();
    }

    // Vulkan orthographic projection matrix for centered
    // view volumes (centered to the z-axis )
    // This matrix is for a right handed coordinate system with the
    // x-axis pointing from left to right, the y-axis pointing from top
    // to bottom and the z-axis pointing into the screen. The matrix
    // is the product of a scale matrix multiplied by a translation
    // matrix. However, if we already are centered, the off centered 
    // matrix can be simplified as follows:
    //
    // l = -r => r - l = 2r and r + l = 0
    // t = -b => b - t = 2b and b + t = 0
    // 
    // The matrix then looks like:
    //
    //    1/r       0        0        0
    //     0       1/b       0        0
    //     0        0      1/(f-n) -n/(f-n)
    //     0        0        0        1
    //
    // where    r = width, b = height, f = far, n = near
    //
    template <typename T>
    constexpr inline mat4Template<T> orthographicVkRH( T width,
                                                       T height,
                                                       T near,
                                                       T far )
    {
        // Initializing the local variable with diagonal = 0
        mat4Template<T> M { 0.0f };

        M.m[0][0] = 1 / width;
        M.m[1][1] = 1 / height;
        M.m[2][2] = 1 / ( far - near );
        M.m[2][3] = -( near ) / ( far - near );
        M.m[3][3] = 1.0f;

        // Vulkan uses column-major ordering, so we return the
        // transposed version of this matrix
        return M.transpose();
    }

    // Vulkan perspective projection matrix for off centered
    // view volumes
    // This matrix is for a right handed coordinate system with the
    // x-axis pointing from left to right, the y-axis pointing from top
    // to bottom and the z-axis pointing into the screen. The matrix
    // is the product of an orthographic matrix multiplied by a
    // perspective matrix
    // The matrix looks like:
    //
    //  2n/(r-l)       0      -(r+l)/(r-l)     0
    //     0        2n/(b-t)  -(b+t)/(b-t)     0
    //     0           0         f/(f-n)   -fn/(f-n)
    //     0           0           1           0
    //
    // where    r = right, l = left, b = bottom, t = top,
    //          f = far, n = near
    //
    template <typename T>
    constexpr inline mat4Template<T> perspectiveVkRHOffCenter( T left,
                                                               T right,
                                                               T bottom,
                                                               T top,
                                                               T near,
                                                               T far )
    {
        // Initializing the local variable with diagonal = 0
        mat4Template<T> M { 0.0f };

        M.m[0][0] = 2 * near / ( right - left );
        M.m[0][2] = -( right + left ) / ( right - left );
        M.m[1][1] = 2 * near / ( bottom - top );
        M.m[1][2] = -( bottom + top ) / ( bottom - top );
        M.m[2][2] = far / ( far - near );
        M.m[2][3] = -( far * near ) / ( far - near );
        M.m[3][2] = 1.0f;

        // Vulkan uses column-major ordering, so we return the
        // transposed version of this matrix
        return M.transpose();
    }

    // Vulkan perspective projection matrix for centered
    // view volumes (centered to the z-axis)
    // This matrix is for a right handed coordinate system with the
    // x-axis pointing from left to right, the y-axis pointing from top
    // to bottom and the z-axis pointing into the screen. The matrix
    // is the product of an orthographic matrix multiplied by a
    // perspective matrix. However, if we already are centered, the 
    // off centered matrix can be simplified as follows:
    //
    // l = -r => r - l = 2r and r + l = 0
    // t = -b => b - t = 2b and b + t = 0
    // 
    // Furthermore, if we use aspect ratio and field of view, we can
    // substitute r and b as:
    // 
    // r = n*aspectRatio / ( tan( fov / 2 ) ), with fov in radians
    // b = n * tan( fov / 2 ), with fov in radians
    //
    // The matrix then looks like:
    //
    //   1/aR*s     0        0        0
    //     0       1/s       0        0
    //     0        0      f/(f-n) -fn/(f-n)
    //     0        0        1        0
    //
    // where    fov = fovY, aR = aspectRatio, f = far, n = near,
    //          s = tan( fov / 2 )
    //
    // NOTE(klek): FovY is expected to be in radians!
    template <typename T>
    constexpr inline mat4Template<T> perspectiveVkRH( T fovY,
                                                      T aspectRatio,
                                                      T near,
                                                      T far )
    {
        // Initializing the local variable with diagonal = 0
        mat4Template<T> M{ 0.0f };

        const float s = maths_tan( fovY / 2 );

        M.m[0][0] = 1 / ( aspectRatio * s ); 
        M.m[1][1] = 1 / s;
        M.m[2][2] = far / ( far - near );
        M.m[2][3] = -( far * near ) / ( far - near );
        M.m[3][2] = 1.0f;

        // Vulkan uses column-major ordering, so we return the
        // transposed version of this matrix
        return M.transpose();
    }

    // View matrix for look at target
    // This matrix can very easily be derived from the viewLookTo()
    // matrix by simply calculate the direction vector from the target
    // to the position
    template <typename T>
    constexpr inline mat4Template<T> viewLookAtVkRH( vec3dTemplate<T> pos,
                                                     vec3dTemplate<T> target,
                                                     vec3dTemplate<T> upDir )
    {
        return viewLookToVkRH( pos, target - pos, upDir );
    }

    // View matrix for look to direction
    // This matrix consists of two sub-matrices, one for translation
    // (T) and one for linear transformation (R) which together form
    // the view space to world space transformation (M). The R-matrix
    // consists of the cameras coordinate system (the unit-vectors 
    // describing it) with the camera being in the center. While the
    // T-matrix simply describes a translation from a point in the
    // camera coordinate system, to the world coordinate system.
    //
    //      1   0   0  c.x              u.x  v.x  w.x  0
    // T =  0   1   0  c.y     and R =  u.y  v.y  w.y  0
    //      0   0   1  c.z              u.z  v.z  w.z  0
    //      0   0   0   1                0    0    0   0
    //
    // However, we are actually interested in the inverse of this
    // matrix since we want to go from world space to view space, and
    // thus we need to invert the matrix M. This can be done by
    // inverting each of the sub-matrices since 1/M = 1/(TR) =>
    // 1/M = (1/T) * (1/R). Manually doing this results in:
    //
    //        1   0   0  -c.x              u.x  u.y  u.z  0
    // 1/T =  0   1   0  -c.y   and 1/R =  v.x  v.y  v.z  0
    //        0   0   1  -c.z              w.x  v.y  w.z  0
    //        0   0   0    1                0    0    0   0
    //
    // Notice that R has "only" been transposed, due to its property
    // of being orthogonal (containing orthogonal vectors from the our
    // cameras coordinate system) and thus the inverse of R is equal
    // to the transposed version of R. Finally, our view-matrix looks
    // as follows:
    //
    //       i.x   i.y   i.z   -(i.dot(pos))
    // 1/M = j.x   j.y   j.z   -(j.dot(pos))
    //       k.x   j.y   k.z   -(k.dot(pos))
    //        0     0     0     1
    //
    // where    k = dir.normalize(), our z-vector which we are looking
    //              in
    //          i = k.cross( upDir ), our cameras new x-axis, formed
    //              with the worlds up-vector, k and upDir might not
    //              be perpendicular
    //          j = k.cross( i ), our cameras new y-axis, formed from
    //              our other two axis
    template <typename T>
    constexpr inline mat4Template<T> viewLookToVkRH( vec3dTemplate<T> pos,
                                                     vec3dTemplate<T> dir,
                                                     vec3dTemplate<T> upDir )
    {
        const vec3dTemplate<T> k { dir.normalize( ) };
        const vec3dTemplate<T> i { k.cross( upDir.normalize( ) ) };
        const vec3dTemplate<T> j { k.cross( i ) };

        mat4Template<T> M { };
        M.m[0][0] = i.x;
        M.m[0][1] = i.y;
        M.m[0][2] = i.z;
        M.m[0][3] = -( i.dot( pos ) );
        M.m[1][0] = j.x;
        M.m[1][1] = j.y;
        M.m[1][2] = j.z;
        M.m[1][3] = -( j.dot( pos ) );
        M.m[2][0] = k.x;
        M.m[2][1] = k.y;
        M.m[2][2] = k.z;
        M.m[2][3] = -( k.dot( pos ) );
        M.m[3][0] = 0.0f;
        M.m[3][1] = 0.0f;
        M.m[3][2] = 0.0f;
        M.m[3][3] = 1.0f;

        // Vulkan uses column-major ordering, so we return the
        // transposed version of this matrix
        return M.transpose( );
    }

    //****************************************************************
    // OpenGL matrices

    // Calculate the orthographics matrix for a given:
    // - Left field
    // - Right field
    // - Bottom field
    // - Top field
    // - Near field
    // - Far field
    // The orthographics matrix is used to decide what is actually in our "scope" or what we
    // can actually display on the screen. It should "clip" away stuff that is outside of our
    // defined planes.
    // The matrix looks like:
    //
    //    2/(R-L)  0     0       -(L+R)/(R-L)
    //    0     2/(T-B)  0       -(B+T)/(T-B)
    //    0     0     -2/(F-N)   -(F+N)/(F-N)
    //    0     0     0          1
    //
    template <typename T>
    constexpr mat4Template<T> orthographicGL( T left, 
                                              T right, 
                                              T bottom, 
                                              T top, 
                                              T near, 
                                              T far )
    {
        mat4Template<T> result( T( 1 ) );

        // Setting diagonal
        result.elements[ 4 * 0 + 0 ] = T( 2.0f ) / ( right - left );
        result.elements[ 4 * 1 + 1 ] = T( 2.0f ) / ( top - bottom );
        result.elements[ 4 * 2 + 2 ] = T( 2.0f ) / ( near - far );

        // Setting last column
        result.elements[ 4 * 3 + 0 ] = ( left + right ) / ( left - right );
        result.elements[ 4 * 3 + 1 ] = ( bottom + top ) / ( bottom - top );    
        result.elements[ 4 * 3 + 2 ] = ( far + near ) / ( near - far );

        return result;
    }

    // Calculate the perspective matrix for a given: 
    // - Field of view
    // - Aspect ratio
    // - Near field
    // - Far field
    // The perspective matrix is used to show stuff in a perspective to were the
    // camera is. It should essentially filter out stuff we cannot see, and make
    // stuff in the distance smaller etc etc.
    // The matrix looks like:
    //
    //     1/(ar*tan(fov/2)  0    0    0
    //     0     1/(tan(fov/2)    0    0
    //     0     0     -(F+N)/(F-N)    -(2*F*N)/(F - N)
    //     0     0     -1              0
    //
    template <typename T>
    constexpr mat4Template<T> perspectiveGL( T fov, 
                                             T aspectRatio, 
                                             T near, 
                                             T far)
    {
        mat4Template<T> result;

        T index5 = T( 1.0f ) / maths_tan( toRadians( T( 0.5f ) * fov ) );
        T index0 = index5 / aspectRatio;
        T index10 = ( near + far ) / ( near - far );
        T index14 = ( T( 2.0f ) * near * far ) / ( near - far );

        result.elements[ 4 * 0 + 0 ] = index0;
        result.elements[ 4 * 1 + 1 ] = index5;
        result.elements[ 4 * 2 + 2 ] = index10;
        result.elements[ 4 * 2 + 3 ] = T( -1.0f );
        result.elements[ 4 * 3 + 2 ] = index14;

        return result;
    }

    // Calculate translation matrix for a given:
    // - Translation (vec3d)
    // The translation matrix is basically an identity matrix
    // with the last column containing the translation
    // The matrix looks like:
    //
    //    1     0     0     v.x
    //    0     1     0     v.y
    //    0     0     1     v.z
    //    0     0     0     1
    //
    template <typename T>
    constexpr mat4Template<T> translationGL( const vec3dTemplate<T>& translation ) 
    {
        mat4Template<T> result( T( 1.0f ) );

        // Set the last column
        result.elements[ 4 * 3 + 0 ] = translation.x;
        result.elements[ 4 * 3 + 1 ] = translation.y;
        result.elements[ 4 * 3 + 2 ] = translation.z;

        return result;
    }

    // Calculate the rotation matrix for a given 
    // - Axis (vec3d)
    // The rotation matrix is used to rotate stuff around an axis by the
    // specified amount of degrees
    // The matrix looks like:
    //
    // TODO(klek): Fix this doc to show actual rotation matrix
    //    1     0     0     v.x
    //    0     1     0     v.y
    //    0     0     1     v.z
    //    0     0     0     1
    //
    template <typename T>
    constexpr mat4Template<T> rotationGL( T angle, const vec3dTemplate<T>& axis)
    {
        mat4Template<T> result( T( 1.0f ) );

        T rad = toRadians( angle );
        T c = maths_cos( rad );
        T s = maths_sin( rad );
        T omc = T( 1.0f ) - c;

        result.elements[ 4 * 0 + 0 ] = axis.x * omc + c;
        result.elements[ 4 * 1 + 0 ] = axis.y * axis.x * omc + axis.z * s;
        result.elements[ 4 * 2 + 0 ] = axis.x * axis.z * omc - axis.y * s;

        result.elements[ 4 * 0 + 1 ] = axis.x * axis.y * omc - axis.z * s;
        result.elements[ 4 * 1 + 1 ] = axis.y * omc + c;
        result.elements[ 4 * 2 + 1 ] = axis.y * axis.z * omc + axis.x * s;

        result.elements[ 4 * 0 + 2 ] = axis.x * axis.z * omc + axis.y * s;
        result.elements[ 4 * 1 + 2 ] = axis.y * axis.z * omc - axis.x * s;
        result.elements[ 4 * 2 + 2 ] = axis.z * omc + c;
        
        return result;
    }

    // Calculate the scale matrix for a given:
    // - Scale (vec3d)
    // The scale matrix is basically an identity matrix with the
    // diagonal set to the scale and finishing of with a 1
    // The matrix looks like:
    //
    //    scale.x  0        0        0
    //    0        scale.y  0        0
    //    0        0        scale.z  0
    //    0        0        0        1
    //
    template <typename T>
    constexpr mat4Template<T> scaleGL(const vec3dTemplate<T>& scale)
    {
        mat4Template<T> result( T( 1.0f ) );

        // Set the diagonal
        result.elements[ 4 * 0 + 0 ] = scale.x;
        result.elements[ 4 * 1 + 1 ] = scale.y;
        result.elements[ 4 * 2 + 2 ] = scale.z;
        
        return result;
    }

    //****************************************************************
    // Friend operators

    // Output operators, overloaded
    template <typename T>
    std::ostream& operator<<(std::ostream &stream, const mat4Template<T>& mat)
    {
        constexpr uint32_t rowColSize = 4;

        stream << "Matrix: \n"; // << std::endl;
        for ( int row = 0; row < rowColSize; row++ )
        {
            stream << "       ( ";
            for ( int col = 0; col < rowColSize; col++ )
            {
                stream << mat.elements[ rowColSize * row + col ] << ", ";
            }
            stream << ")\n";// << std::endl;
        }

        return stream;
    }

} // namespace muggy::math
