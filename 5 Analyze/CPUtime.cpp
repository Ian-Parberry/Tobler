/// \file CPUtime.cpp
/// \brief Code for getting access to CPU time under Windows.

// Copyright Ian Parberry, December 2013.
//
// This file is made available under the GNU All-Permissive License.
//
// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.
//
// Created by Ian Parberry, December 2013.
// Last updated  May 7, 2014.

#if defined(_MSC_VER) //Windows Visual Studio 

#include <windows.h>

/// Get CPU time in units of 100 nanoseconds, which is the default.
/// \return User CPU time in hundreds of nanoseconds.

long long CPUTimeInCentiNanoseconds(){  
  long long llCNS=0LL;
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());

  if(hProcess != NULL){
    FILETIME ftCreation, ftExit, ftKernel, ftUser;
    if(GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser))
      llCNS = *((long long*)&ftUser);
    CloseHandle(hProcess);
  } //if
  return llCNS;
} //CPUTimeInCentiNanoseconds

/// Get CPU time in units of a millisecond.
/// \return User CPU time in ms.

unsigned int CPUTimeInMilliseconds(){
  return (unsigned int)(CPUTimeInCentiNanoseconds()/10000LL);
} //CPUTimeInMilliseconds

#else//other OS

#include <time.h>

/// Get CPU time in units of a millisecond.
/// \return User CPU time in ms.

unsigned int CPUTimeInMilliseconds(){
  return (unsigned int)(1000.0*(double)(clock())/CLOCKS_PER_SEC);
} //CPUTimeInMilliseconds
#endif
