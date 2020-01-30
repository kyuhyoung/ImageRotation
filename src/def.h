#ifndef _DEF_H_
#define _DEF_H_

//#include <stdio.h>
//#include <string.h>
#include <iostream>

using namespace std;


#ifndef WDIBPIXEL_DEFINED
#define WDIBPIXEL_DEFINED

#ifndef _WIN32
#pragma message("AA")
#define WDIBPIXEL_BITS  8 
//#define WDIBPIXEL_BITS  16 
#endif  //  _WIN32



#include <stdint.h>
#if WDIBPIXEL_BITS==8
#pragma message("888")
typedef uint8_t WDIBPIXEL;
#elif WDIBPIXEL_BITS==16
#pragma message("1616")
typedef uint16_t WDIBPIXEL;
//#define WDIBPIXEL_BITS (16) 
#else
#pragma message("3232")
typedef uint32_t WDIBPIXEL;
#define WDIBPIXEL_BITS (32) 
#endif      //  WDIBPIXEL_BITS
#endif      //  WDIBPIXEL_DEFINED

void cout_indented(int n_space, const string& str);



#endif      //  _DEF_H_
