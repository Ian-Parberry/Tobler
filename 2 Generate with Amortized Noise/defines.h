/// \file defines.h
/// \brief Header file for OS dependent things.

// Copyright Ian Parberry, April 2014.
//
// This file is made available under the GNU All-Permissive License.
//
// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.
//
// Created by Ian Parberry, April 2014.
// Last updated May 7, 2014.

#pragma once

#if defined(_MSC_VER) //Windows Visual Studio 

  #include <windows.h> //for timeGetTime() mainly
  #include <conio.h> //for _getch()
  #pragma warning(disable : 4996) //disable annoying security warnings for stdio functions

#else//other OS

  #include <time.h>
  #define MAX_PATH 256 ///< Maximum length of a path in Windows.

#endif
