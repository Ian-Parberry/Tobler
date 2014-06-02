/// \file InfiniteAmortizedNoise2D.h
/// \brief Header file for the 2D amortized noise class CInfiniteAmortizedNoise2D.
///
/// This version of InfiniteAmortizedNoise2D.h differs from the original by
/// making everything that was private be protected, and making private functions
/// initEdgeTables and h be virtual functions. These modifications were made
/// so that CTerrainGenerator can be derived from CInfiniteAmortizedNoise2D.

// Copyright Ian Parberry, September 2013, 2014.
//
// This file is made available under the GNU All-Permissive License.
//
// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.
//
// Created by Ian Parberry, September 2013.
// Last updated May 30, 2014.

#pragma once

/// \brief The amortized 2D noise class.
///
/// The 2D amortized noise class implements the 2D infinite amortized noise algorithm.

class CInfiniteAmortizedNoise2D{
  protected: //Amortized noise stuff
    float *uax; ///< X coordinate of u used to compute a.
    float *vax; ///< X coordinate of v used to compute a.
    float *ubx; ///< X coordinate of u used to compute b.
    float *vbx; ///< X coordinate of v used to compute b.
    float *uay; ///< Y coordinate of u used to compute a.
    float *vay; ///< Y coordinate of v used to compute a.
    float *uby; ///< Y coordinate of u used to compute b.
    float *vby; ///< Y coordinate of v used to compute b.
    float* spline; ///< Spline table.
    unsigned int seed; ///< Hash seed.

    virtual unsigned int h(const unsigned int x, const unsigned int y); ///< 2D hash function.

    void FillUp(float* t, const float s, const int n); ///< Fill amortized noise table bottom up.
    void FillDn(float* t, const float s, const int n); ///< Fill amortized noise table top down.

    virtual void initEdgeTables(const int x, const int y, const int n); ///< Initialize the amortized noise tables.
    void initSplineTable(const int n); ///< Initialize the spline table.

    float getNoise(const int i, const int j);  ///< Get one point of amortized noise. 
    void getNoise(const int n, const int i0, const int j0, float** cell);  ///< Get 1 octave of amortized noise into cell.
    void addNoise(const int n, const int i0, const int j0, const float scale, float** cell);  ///< Add 1 octave of amortized noise into cell.

  public:
    CInfiniteAmortizedNoise2D(const unsigned n, const unsigned int s); ///< Constructor.
    ~CInfiniteAmortizedNoise2D(); ///< Destructor.
    float generate(int x, int y, const int m0, const int m1, const int n, float** cell); ///< Generate a cell of 2D amortized noise.
}; //CInfiniteAmortizedNoise2D
