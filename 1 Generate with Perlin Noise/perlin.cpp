/// \file perlin.cpp
/// \brief Code file for 2D Perlin noise with exponentially distributed gradients.

// Copyright Ian Parberry, May 2014.
//
// This file is made available under the GNU All-Permissive License.
//
// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.

// Created by Ian Parberry, May 2014.
// Last updated May 31, 2014.

#include <stdlib.h>
#include <stdio.h>

#define _USE_MATH_DEFINES ///< Enable use of constant M_SQRT2 in math.h
#include <math.h>
const float FM_SQRT2 = (float)M_SQRT2; ///< Square root of 2 as a float.

#define B 0x100 ///< Perlin's B, a power of 2 usually equal to 256.
#define BM 0xff ///< A bit mask, one less than B.
#define N 0x1000 ///< Perlin's N.

extern float g_fMu;

#define lerp(t, a, b) (a + t*(b - a)) ///< Linear interpolation.
#define s_curve(t) (t*t*(3.0f - 2.0f*t)) ///< Cubic spline.

/// Perlin's setup macro.
#define setup(i,b0,b1,r0,r1)\
  t = vec[i] + N;\
  b0 = ((int)t) & BM;\
  b1 = (b0+1) & BM;\
  r0 = t - (int)t;\
  r1 = r0 - 1.0f;

#define at2(rx, ry) (rx*q[0] + ry*q[1]) ///< Perlin's dot product macro.

static int p[B]; ///< Perlin's permutation table.
static float g2[B][2]; ///< Perlin's gradient table.

static float m[B]; ///< Ian Parberry's gradient magnitude table.

/// \brief 2D vector normalize.
///
/// Works by side-effect on the parameter v.
/// \param v 2D vector as a 2-element array.

void normalize2(float v[2]){
  float s = sqrt(v[0]*v[0] + v[1]*v[1]);
  v[0] /= s; v[1] /= s;
} //normalize2

/// \brief Swap two integers.
///
/// Works by side-effect.
/// \param x First value.
/// \param y Second value.

void swap(int& x, int& y){
  int k = x; x = y; y = k;
} //swap

/// \brief Get random floating point number.
///
/// Get random floating point number between -1 and 1. Uses rand() to
/// do the heavy lifting.
/// \return Pseudorandom floating point number between -1 and 1.

float randomflt(){
  return (2.0f*rand())/RAND_MAX - 1.0f;
} //randomflt

/// \brief Initialize Perlin noise.
///
/// Initialize the permutation, gradient, and magnitude tables.

void initPerlin2D(){  
  //random gradient vectors
  for(int i=0; i<B; i++){
    g2[i][0] = randomflt();
    g2[i][1] = randomflt();
    normalize2(g2[i]);
  } //for
  
  //random permutation 
  for(int i=0; i<B; i++) //identity permutation
    p[i] =  i;
  for(int i=B-1; i>0; i--) //randomly transpose elements
    swap(p[i], p[rand()%(i + 1)]); //bug fix - Perlin had i, not i+1.
  
  //gradient magnitude array
  float s = 1.0; //current magnitude
  for(int i=0; i<B; i++){
    m[i] = s; s /= g_fMu;
  } //for
} //initPerlin2D

/// \brief COmpute one point of Perlin noise.
///
/// Compute a single octave of noise at 2D noise at a single point.
/// This is mostly taken from Perlin's original code, with a few tweaks.
/// \param vec Point at which to evaluate noise.
/// \return Noise value between -1.0 and 1.0.

float noise2(float vec[2]){
  int bx0, bx1, by0, by1;
  float rx0, rx1, ry0, ry1, *q, t;

  setup(0, bx0, bx1, rx0, rx1);
  setup(1, by0, by1, ry0, ry1);

  int b00 = p[(p[bx0] + by0) & BM];
  int b10 = p[(p[bx1] + by0) & BM];
  int b01 = p[(p[bx0] + by1) & BM];
  int b11 = p[(p[bx1] + by1) & BM];

  float sx = s_curve(rx0);

  float u, v;
  q = g2[b00]; u = m[b00] * at2(rx0, ry0);
  q = g2[b10]; v = m[b10] * at2(rx1, ry0);
  float a = lerp(sx, u, v);

  q = g2[b01]; u = m[b01] * at2(rx0, ry1);
  q = g2[b11]; v = m[b11] * at2(rx1, ry1);
  float b = lerp(sx, u, v);

  float sy = s_curve(ry0);
  return lerp(sy, a, b);
} //noise2

/// \brief Compute turbulence value.
//
/// Turbulence is also known as 1/f noise.
/// \param x X coordinate.
/// \param y Y coordinate.
/// \param n Number of octaves.
/// \return Noise value in the range -1 to 1.

float PerlinNoise2D(const float x, const float y, const int n){
  float sum=0.0f, p[2], scale=1.0f;
  p[0] = x; p[1] = y;

  for(int i=0; i<n; i++){ //for each octave
    scale *= 0.5f; //apply persistence
    sum += noise2(p)*scale; //add in an octave of noise
    p[0] *= 2.0f;	p[1] *= 2.0f; //apply lacunarity
  } //for
  return FM_SQRT2*sum/(1.0f - scale);
} //PerlinNoise2D
