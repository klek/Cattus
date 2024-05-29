//********************************************************************
//  File:    main.cpp
//  Date:    Mon, 06 Nov 2023: 18:43
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

//#pragma comment(lib, "muggy.lib")

#include "muggy.h"
#include <iostream>

#include "tests/test.h"

#if TEST_ENTITIES
#include "tests/testEntities.h"
#elif TEST_WINDOW
#include "tests/testWindow.h"
#elif TEST_FREELIST
#include "tests/testFreelist.h"
#elif TEST_VECTOR
#include "tests/testVector.h"
#elif TEST_RENDERER
#include "tests/testRenderer.h"
#elif TEST_MATH
#include "tests/testMath.h"
#else
#error One of the tests have to be enabled in test.h
#endif



int main( int argc , char* argv[] ) 
{
    std::cout << "Hello, from muggyExample!\n";

//    muggy::print("Hello from Engine!\n");

    engineTest test;

    if ( test.initialize() ) {
        // bool isRunning = true;
        // uint16_t i = 0;
        // while ( isRunning )
        // {
        //     // We need to determine if any windows has closed yet
        //     //while ( )
        //     if ( !( i++ < ( uint16_invalid_id / 16 ) ) )
        //         isRunning = false;
        //     test.run();
        // }

        // Call the tests run function
        test.run();
    }

    test.shutdown();

    std::cout << "Finished testing!" << std::endl;
    return 0;
}
