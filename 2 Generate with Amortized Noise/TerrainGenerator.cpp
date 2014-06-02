/// \file TerrainGenerator.cpp
/// \brief Code file for the terrain generator CTerrainGenerator.

// Copyright Ian Parberry, May 2014.
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

#define _USE_MATH_DEFINES //for M_SQRT2
#include <math.h> //for trig functions

#include "TerrainGenerator.h"
#include "Common.h"
#include "MurmurHash3.h"

#include "ExponentialHash.h"

/// The constructor initializes the new random number seeds and omega, and
/// relies on the CInfiniteAmortizedNoise2D constructor to do the rest.
/// \param n Cell size.
/// \param s Hash function seed.
/// \param tail Value of omega.

CTerrainGenerator::CTerrainGenerator(const unsigned int n, const unsigned int s, float tail):
  CInfiniteAmortizedNoise2D(n, s), omega(tail)
{ 
  seed1 = s + 9999; //hopefully murmurhash can handle this
  seed2 = s + 314159;  //hopefully murmurhash can handle this too
} //constructor

/// Hash two unsigned ints into a single unsigned int using MurmurHash.
/// \param x X coordinate of value to be hashed.
/// \param y Y coordinate of value to be hashed.
/// \param seed Seed for the hash function.
/// \return Hash of (x, y).

unsigned int CTerrainGenerator::h(const unsigned int x, const unsigned int y, unsigned int seed){
  unsigned int result;
  unsigned long long key = ((unsigned long long)x<<32) | y;
  MurmurHash3_32(&key, 8, seed, &result); //do the heavy lifting
  return result;
} //h

/// Initialize the edge tables with gradients with a uniformly distributed
/// direction and an exponentially distributed magnitude.
/// \param x0 x coordinate of top left corner of cell.
/// \param y0 y coordinate of top left corner of cell.
/// \param n Granularity.

void CTerrainGenerator::initEdgeTables(const int x0, const int y0, const int n){
  //compute gradients at corner points
  unsigned int b00 = CInfiniteAmortizedNoise2D::h(x0,   y0);
  unsigned int b01 = CInfiniteAmortizedNoise2D::h(x0,   y0+1); 
  unsigned int b10 = CInfiniteAmortizedNoise2D::h(x0+1, y0);
  unsigned int b11 = CInfiniteAmortizedNoise2D::h(x0+1, y0+1);

  //compute magnitudes at corner points
  const unsigned int max = 0xFFFFFFFF;
  float m00 = ExpHash(h(x0, y0,     seed1), h(x0, y0,     seed2), max, omega);
  float m01 = ExpHash(h(x0, y0+1,   seed1), h(x0, y0+1,   seed2), max, omega); 
  float m10 = ExpHash(h(x0+1, y0,   seed1), h(x0+1, y0,   seed2), max, omega);
  float m11 = ExpHash(h(x0+1, y0+1, seed1), h(x0+1, y0+1, seed2), max, omega);
  
  //fill inferred gradient tables from corner gradients
  FillUp(uax, m00 * cosf((float)b00), n); FillDn(vax, m01 * cosf((float)b01), n);
  FillUp(ubx, m10 * cosf((float)b10), n); FillDn(vbx, m11 * cosf((float)b11), n);
  FillUp(uay, m00 * sinf((float)b00), n); FillUp(vay, m01 * sinf((float)b01), n);
  FillDn(uby, m10 * sinf((float)b10), n); FillDn(vby, m11 * sinf((float)b11), n);
} //initEdgeTables
