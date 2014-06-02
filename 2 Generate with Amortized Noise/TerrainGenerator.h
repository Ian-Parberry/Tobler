/// \file TerrainGenerator.h
/// \brief Header file for the terrain generator CTerrainGenerator.

// Copyright Ian Parberry, September 2013.
//
// This file is made available under the GNU All-Permissive License.
//
// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.
//
// Created by Ian Parberry, May 2014.
// Last updated May 30, 2014.

#pragma once

#include "InfiniteAmortizedNoise2D.h"

/// \brief The terrain generator class.
///
/// The terrain class implements the 2D infinite amortized noise algorithm
/// with an exponential gradient magnitude distribution.

class CTerrainGenerator: public CInfiniteAmortizedNoise2D{
  protected: 
    unsigned int seed1; ///< Hash seed for gradient magnitude.
    unsigned int seed2; ///< Hash seed for tail of gradient magnitude distribution.
    float omega; ///< Tail height multiplier.
    
    unsigned int h(const unsigned int x, const unsigned int y, unsigned int seed); ///< 2D hash function.
    void initEdgeTables(const int x, const int y, const int n); ///< Initialize the amortized noise tables.

  public:
    CTerrainGenerator(const unsigned int n, const unsigned int s, float tail); ///< Constructor.
}; //CTerrainGenerator
