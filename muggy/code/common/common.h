//********************************************************************
//  File:    common.h
//  Date:    Mon, 13 Nov 2023: 00:20
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(COMMON_H)
#define COMMON_H

//#pragma warning(disables 4530) // Disable exception warning

// Common includes for the the engine
// C/C++ 
#include <cstdint>
#include <assert.h>
#include <cstring>
#include <typeinfo>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <iostream>


// Internal common headers
#include "myMacros.h"
#include "myTypes.h"
#include "id.h"
#include "../utilities/utilities.h"

// Including math library
#include "../math/math.h"


#endif