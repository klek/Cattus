//********************************************************************
//  File:    test.h
//  Date:    Thu, 16 Nov 2023: 05:23
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(TEST_H)
#define TEST_H

#include <thread>
#include <iostream>

// Defines for test to run
#define TEST_ENTITIES           0
#define TEST_WINDOW             0
#define TEST_FREELIST           0
#define TEST_VECTOR             0
#define TEST_RENDERER           1
#define TEST_MATH               0

class test
{
public:
    virtual bool initialize() = 0;
    virtual void run() = 0;
    virtual void shutdown() = 0;
};

class test_timer
{
public:
    using clock = std::chrono::high_resolution_clock;
    using timeStamp = std::chrono::high_resolution_clock::time_point;

    void begin( void )
    {
        m_Start = clock::now();
    }

    void end( void )
    {
        auto dt = clock::now() - m_Start;
        m_MsAvg += ( (float)std::chrono::duration_cast<std::chrono::milliseconds>(dt).count() - m_MsAvg ) / (float)m_Counter;
        m_Counter++;

        // Check if 1 second has passed
        if ( std::chrono::duration_cast<std::chrono::seconds>( clock::now() - m_Seconds ).count() >= 1 )
        {
            std::cout << "Avg. frame (ms): ";
            std::cout << std::to_string( m_MsAvg ).c_str();
            std::cout << " @ ";
            std::cout << std::to_string( m_Counter ).c_str();
            std::cout << " fps" << std::endl;

            // Reset variables
            m_MsAvg = 0.0f;
            m_Counter = 1;
            m_Seconds = clock::now();
        }
    }

private:
    float       m_MsAvg { 0.0f };
    uint32_t    m_Counter { 1 };
    timeStamp   m_Start;
    timeStamp   m_Seconds { clock::now() };
};

#endif