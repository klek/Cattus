//********************************************************************
//  File:    testMath.cpp
//  Date:    Mon, 27 May 2024: 20:51
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "test.h"

#if TEST_MATH
#include "testMath.h"
#include "../../muggy/code/math/math.h"
#include <iostream>

muggy::math::mat4Template<float>    identity{ 1.0f };
muggy::math::mat4Template<float>    testMatrix;

bool engineTest::initialize( void )
{
    // No initialization here
    return true;
}

void engineTest::run( void )
{

    std::cout << identity << std::endl;

    // for ( uint32_t i = 0; i < FOUR_BY_FOUR; i++ )
    // {
    //     testMatrix.elements[i] = i + 1;
    // }
    testMatrix = muggy::math::mat4Template<float>{ 
        5.0f, 6.0f, 6.0f, 8.0f,
        2.0f, 2.0f, 2.0f, 8.0f,
        6.0f, 6.0f, 2.0f, 8.0f,
        2.0f, 3.0f, 6.0f, 7.0f
    };
    std::cout << testMatrix << std::endl;

    muggy::math::mat4Template<float> inverted { testMatrix.inverse() };

    std::cout << "The determinant is: " << testMatrix.determinant() << std::endl;
    std::cout << "The original " << testMatrix << std::endl;
    std::cout << "The inverted " << inverted << std::endl;
    testMatrix.invertMe();
    std::cout << "The inverted " << testMatrix << std::endl;

    testMatrix = muggy::math::mat4Template<float>{ 
        1.0f, 0.0f, 4.0f, -6.0f,
        2.0f, 5.0f, 0.0f, 3.0f,
        -1.0f, 2.0f, 3.0f, 5.0f,
        2.0f, 1.0f, -2.0f, 3.0f
    };
    std::cout << "The determinant is: " << testMatrix.determinant() << std::endl;
    std::cout << "The original " << testMatrix << std::endl;
    std::cout << "The inverted " << testMatrix.inverse() << std::endl;
    testMatrix.invertMe();
    std::cout << "The inverted " << testMatrix << std::endl;

}

void engineTest::shutdown( void )
{
    // Nothing to do here currently
}



#endif