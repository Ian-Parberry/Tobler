/// \file InfiniteAmortizedNoise2D.cpp
/// \brief Code file for the 2D amortized noise class CInfiniteAmortizedNoise2D.

// Copyright Ian Parberry, September 2013.
//
// This file is made available under the GNU All-Permissive License.
//
// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.
//
// Created by Ian Parberry, September 2013.
// Last updated May 6, 2014.

#define _USE_MATH_DEFINES //for M_SQRT2
#include <math.h> //for trig functions

#include "InfiniteAmortizedNoise2D.h"
#include "Common.h"
#include "MurmurHash3.h"

/// Constructor.
/// \param n Cell size.
/// \param s Hash function seed.

CInfiniteAmortizedNoise2D::CInfiniteAmortizedNoise2D(const unsigned int n, const unsigned int s): seed(s){ 
  //Allocate space for amortized noise tables.
  uax = new float [n]; vax = new float [n]; //ax
  ubx = new float [n]; vbx = new float [n]; //bx
  uay = new float [n]; uby = new float [n]; //ay
  vay = new float [n]; vby = new float [n]; //by

  //Allocate space for spline table.
  spline = new float [n]; 
} //constructor

CInfiniteAmortizedNoise2D::~CInfiniteAmortizedNoise2D(){
  //Deallocate space for amortized noise tables.
  delete [] uax; delete [] vax; //ax
  delete [] ubx; delete [] vbx; //bx
  delete [] uay; delete [] uby; //ay
  delete [] vay; delete [] vby; //by
  
  //Deallocate space for spline table.
  delete [] spline; 
} //destructor

/// Fill amortized noise table bottom up.
/// \param t Amortized noise table.
/// \param s Initial value.
/// \param n Granularity.

void CInfiniteAmortizedNoise2D::FillUp(float* t, const float s, const int n){
  const float d = s/n; //increment amount
  t[0] = 0.0f; //corner
  for(int i=1; i<n; i++) //edge values
    t[i] = t[i-1] + d;
} //FillUp

/// Fill amortized noise table top down.
/// \param t Amortized noise table.
/// \param s Initial value.
/// \param n Granularity.

void CInfiniteAmortizedNoise2D::FillDn(float* t, const float s, const int n){
  const float d = -s/n;  //increment amount
  t[n-1] = d; //corner
  for(int i=n-2; i>=0; i--) //edge values
    t[i] = t[i+1] + d;
} //FillDn

/// A 2D hash function.
/// Hash two dimensions into a single unsigned int using MurmurHash.
/// \param x X coordinate of value to be hashed.
/// \param y Y coordinate of value to be hashed.
/// \return Hash of (x, y).

unsigned int CInfiniteAmortizedNoise2D::h(const unsigned int x, const unsigned int y){
  unsigned int result;
  unsigned long long key = ((unsigned long long)x<<32) | y;
  MurmurHash3_32(&key, 8, seed, &result); //do the heavy lifting
  return result;
} //h

/// Initialize the amortized noise tables.
/// \param x0 x coordinate of top left corner of cell.
/// \param y0 y coordinate of top left corner of cell.
/// \param n Granularity.

void CInfiniteAmortizedNoise2D::initEdgeTables(const int x0, const int y0, const int n){
  //compute gradients at corner points
  unsigned int b00 = h(x0,   y0);
  unsigned int b01 = h(x0,   y0+1); 
  unsigned int b10 = h(x0+1, y0);
  unsigned int b11 = h(x0+1, y0+1);
  
  //fill inferred gradient tables from corner gradients
  FillUp(uax, cosf((float)b00), n); FillDn(vax, cosf((float)b01), n);
  FillUp(ubx, cosf((float)b10), n); FillDn(vbx, cosf((float)b11), n);
  FillUp(uay, sinf((float)b00), n); FillUp(vay, sinf((float)b01), n);
  FillDn(uby, sinf((float)b10), n); FillDn(vby, sinf((float)b11), n);
} //initEdgeTables

/// Initialize the spline table as described in the paper "Amortized Noise".
/// \param n Granularity.

void CInfiniteAmortizedNoise2D::initSplineTable(const int n){
  for(int i=0; i<n; i++){ //for each table entry
    float t = (float)i/n; //offset between grid points
    spline[i] = s_curve2(t); //quintic spline
  } //for
} //initSplineTable

/// Compute a single point of a single octave of Perlin noise. This is similar
/// to Perlin's noise2 function except that it substitutes table lookups for
///  floating pointmultiplication as described in the paper "Amortized Noise".
/// \param i x coordinate of point.
/// \param j y coordinate of point.
/// \return Noise value at (x, y).

float CInfiniteAmortizedNoise2D::getNoise(const int i, const int j){  
  float u = uax[j] + uay[i];
  float v = vax[j] + vay[i];
  const float a = lerp(spline[j], u, v); 
  u = ubx[j] + uby[i];
  v = vbx[j] + vby[i];
  const float b = lerp(spline[j], u, v);   
  return lerp(spline[i], a, b);   
} //getNoise

/// Get a single octave of noise into a subcell.
/// This differs from CInfiniteAmortizedNoise2D::addNoise in that it copies the noise
/// to the cell instead of adding it in.
/// \param n Granularity.
/// \param i0 x offset of this subcell in cell.
/// \param j0 y offset of this subcell in cell.
/// \param cell Noise cell.

void CInfiniteAmortizedNoise2D::getNoise(const int n, const int i0, const int j0, float** cell){  
  for(int i=0; i<n; i++)
    for(int j=0; j<n; j++) 
      cell[i0 + i][j0 + j] = 
        getNoise(i, j); //the only line that differs from addNoise
} //getNoise

/// Add a single octave of noise into a subcell.
/// This differs from CInfiniteAmortizedNoise2D::getNoise in that it adds the noise
/// to the cell instead of copying it there.
/// \param n Granularity.
/// \param i0 x offset of this subcell in cell.
/// \param j0 y offset of this subcell in cell.
/// \param scale Noise is to be rescaled by this factor.
/// \param cell Noise cell.

void CInfiniteAmortizedNoise2D::addNoise(const int n, const int i0, const int j0, const float scale, float** cell){  
  for(int i=0; i<n; i++)
    for(int j=0; j<n; j++)
      cell[i0 + i][j0 + j] += 
        scale * getNoise(i, j); //the only line that differs from getNoise
} //addNoise

/// Generate a cell of 1/f amortized noise with persistence 0.5 and lacunarity 2.0.
/// \param x x coordinate of top left corner of cell.
/// \param y y coordinate of top left corner of cell.
/// \param m0 First octave.
/// \param m1 Last octave.
/// \param n Granularity.
/// \param cell Cell to put generated noise into.
/// \return Multiply noise by this to get into the range -1..1

float CInfiniteAmortizedNoise2D::generate(int x, int y, const int m0, const int m1, int n, float** cell){
  int r = 1; //side length of cell divided by side length of subcell.

  //Skip over unwanted octaves.
  for(int i=1; i<m0; i++){
    n /= 2; r += r;
  } //for

  if(n < 2)return 1.0f; //fail and bail - should not happen

  //Generate first octave directly into cell.
  //  We could add all octaves into the cell directly if we zero out the cell
  //  before we begin. However, that is a nontrivial overhead that Perlin noise
  //  does not have, and we can avoid it too by putting in the first octave and
  // adding in the rest.

  initSplineTable(n); //initialize the spline table to cells of size n
  for(int i0=0; i0<r; i0++)
    for(int j0=0; j0<r; j0++){ //for each subcell
      initEdgeTables(x + i0, y + j0, n); //initialize the amortized noise tables
      getNoise(n, i0*n, j0*n, cell); //generate noise directly into cell
    } //for

  float scale = 1.0f; //scale factor

  //Generate the other octaves and add them into cell. See previous comment.
  for(int k=m0; k<m1 && n>=2; k++){ //for each octave after the first
    n /= 2; r += r;  x += x; y += y; scale *= 0.5f; //rescale for next octave
    initSplineTable(n); //initialize the spline table to cells of size n
    for(int i0=0; i0<r; i0++)
      for(int j0=0; j0<r; j0++){ //for each subcell
        initEdgeTables(x + i0, y + j0, n); //initialize the edge tables
        addNoise(n, i0*n, j0*n, scale, cell); //generate directly into cell
      } //for
  } //for each octave

  //Compute 1/magnitude and return it. 
  //  A single octave of Perlin noise returns a value of magnitude at most 
  //  1/sqrt(2). Adding magnitudes over all scaled octaves gives a total
  //  magnitude of (1 + 0.5 + 0.25 +...+ scale)/sqrt(2). This is equal to
  //  (2 - scale)/sqrt(2) (using the standard formula for the sum of a geometric
  //  progression). 1/magnitude is therefore sqrt(2)/(2-scale).

  return (float)M_SQRT2/(2.0f - scale); //multiply by this to bring noise to [-1,1]
} //generate
