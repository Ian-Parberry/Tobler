/// \file Random.cpp
/// \brief Code for generating exponentially distributed random numbers.

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

#include "Random.h"
#include "defines.h"

using namespace std;

#define clip(x,a,b) min(max(x, a), b) ///< Clip x to [a,b].

/// \brief Uniform random number generator.
/// 
/// A random number generator that generates a uniformly distributed pseudorandom
/// floating point number between 0 and 1.
/// \return A uniformly distributed random number > 0 and < 1.

float UniformRand(){
  return (rand() + 1)/(RAND_MAX + 2.0f);
} //UniformRand

/// \brief Exponential random number generator.
/// 
/// A random number generator that generates an exponentially distributed pseudorandom
/// floating point number between 0 and 1.
/// \return An exponentially distributed random number > 0 and < 1.

float ExpRand(){ 
  static const float scale = 1/log(0.5f*((float)RAND_MAX + 2.0f));
  return -scale * log(0.5f*rand() + 1.0f) + 1.0f; 
} //ExpRand

/// \brief Exponential random number generator with lifted tail.
/// 
/// A random number generator that generates an exponentially distributed pseudorandom
/// floating point number between 0 and 1 with the tail of the distribution
/// artificially lifted up.
/// \param omega The tail multiplier, which controls how low the tails of the distribution can be.
/// \return An exponentially distributed random number > 0 and < 1.

float ExpRand(float omega){  
  omega = clip(omega, 0.0f, 1.0f); 
  return (UniformRand() < omega)? UniformRand(): ExpRand();
} //ExpRand
