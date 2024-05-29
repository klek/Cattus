//********************************************************************
//  File:    testMath.h
//  Date:    Mon, 27 May 2024: 20:51
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(TEST_MATH_H)
#define TEST_MATH_H

class engineTest : public test
{
public:
    bool initialize( void ) override;
    void run( void ) override;
    void shutdown( void ) override;

private:
    // Any internal variables?
};


#endif