/// \file CPUtime.h
/// \brief Header for getting access to CPU time under Windows.

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
// Last updated May 7, 2014.

#pragma once

long long CPUTimeInCentiNanoseconds(); ///< CPU time in hundredths of a nanosecond.
unsigned int CPUTimeInMilliseconds();  ///< CPU time in milliseconds.
