/// \file ExponentialHash.cpp
/// \brief Code for generating an exponentially distributed hash function.

// Copyright Ian Parberry, May 2014.
//
// This file is made available under the GNU All-Permissive License.
// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.
//
// Created by Ian Parberry, May 2014.
// Last updated May 31, 2014.

#include <stdlib.h>
#include <math.h>
#include <algorithm>

#include "defines.h"

using namespace std;

#define clip(x,a,b) min(max(x, a), b) ///< Clip x to [a,b].

/// \brief Uniform hash.
///
/// Hash an unsigned integer uniformly into the range (0, 1).
/// \param x Value to be hashed.
/// \param max Largest possible value of x.
/// \return A uniformly distributed hash value > 0 and < 1.

float UniformHash(unsigned int x, unsigned int max){
  return ((float)x + 1.0f)/((float)max + 2.0f);
} //UniformHash

/// \brief Exponential hash.
///
/// Hash an unsigned integer into the range (0, 1) with an exponential distribution.
/// \param x Value to be hashed.
/// \param max Largest possible value of x.
/// \return An exponentially distributed hash value > 0 and < 1.

float ExpHash(unsigned int x, unsigned int max){ 
  static const float scale = 1/log(0.5f*((float)max + 2.0f));
  return -scale * log(0.5f*(float)x + 1.0f) + 1.0f; 
} //ExpHash


/// \brief Exponential hash with control of exponent and tail of distribution.
///
/// Hash an unsigned integer into the range (0, 1) with an exponential distribution
/// giving some control over the base of the exponent and the height of the
/// tail of the distribution.
/// \param x Value to be hashed.
/// \param y Second value to be hashed to select distribution.
/// \param m Largest possible value of x.
/// \param omega The tail multiplier, which controls how low the tails of the distribution can be.
/// \return An exponentially distributed random number > 0 and < 1.

float ExpHash(unsigned int x, unsigned int y, unsigned int m, float omega){  
  omega = clip(omega, 0.0f, 1.0f); 
  return (UniformHash(y, m) < omega)? UniformHash(x, m): ExpHash(x, m);
} //ExpHash
