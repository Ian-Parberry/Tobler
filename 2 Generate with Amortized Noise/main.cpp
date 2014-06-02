/// \file Main.cpp
/// \brief Main.
///
/// \mainpage Amortized Noise Terrain Generator
///
/// This is the amortized noise based terrain generator from Ian Parberry,
/// "Tobler's First Law of Geography, Self Similarity, and Perlin Noise:
/// A Large Scale Analysis of Gradient Distribution in Southern Utah with 
/// Application to Procedural Terrain Generation". You don't actually need to 
/// know much about the  paper to try it out, but you at least need to know 
/// some of the concepts.  It will generate a square cell of terrain elevations 
/// using amortized noise with an exponentially distributed gradient magnitude and
/// save it in a DEM  file called output.asc. After that you are on your own, 
/// which you may find disconcerting. If I might make a small suggestion, 
/// Terragen 3 will do an   excellent job of rendering your terrain, and it 
/// reads DEM files natively. You can download an only mildly crippled version 
/// for free from  http://planetside.co.uk/products/terragen3, but I should 
/// in all good faith warn you that the process of learning its foibles is 
/// not for the faint of heart.
/// To fully understand what is going on here, you should probably also read
/// Ian Parberry, "Amortized Noise", Journal of Computer Graphics Techniques,
/// To Appear. There's code at https://github.com/Ian-Parberry/AmortizedNoise
/// and a webpage at http://larc.unt.edu/ian/research/amortizednoise/.
///
/// The program will prompt you for a random number seed (9999 is as good
/// as any), the tail multiplier omega (a number between 0 and 1, try 0.3 
/// if your imagination fails you), and an elevation cap in meters (somewhat 
/// tenuously related to the maximum height of the terrain, try 4000).

// Copyright Ian Parberry, May 2014.
//
// This file is made available under the GNU All-Permissive License.
// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.
//
// Created by Ian Parberry, May 2014.
// Last updated May 30, 2014.

#include "defines.h" //OS porting defines

#include <stdlib.h> //for rand()
#include <stdio.h> //for printf()

#include "TerrainGenerator.h"
#include "CPUtime.h"

CTerrainGenerator* g_pTerrainGenerator = NULL; ///< Pointer to the terrain generator. 

/// \brief Save a cell of terrain elevations.
///
/// Save a cell of noise as a DEM file. The output file
/// will have a ".asc" file expension, which is standard for DEM files.
/// \param cell Pointer to 2D array of elevations.
/// \param n Width and height of array.
/// \param scale Scale value to normalize amortized noise.
/// \param altitude Multiplier to convert noise to height.
/// \param basefilename Name of DEM file for output, without extension.

void SaveDEMFile(float** cell, const int n, const float scale, 
                 const float altitude, const char* basefilename){
  char filename[MAX_PATH];
  sprintf(filename, "%s.asc", basefilename); 
  printf("Saving to %dx%d DEM file %s\n", n, n, filename);

  FILE* output = fopen(filename, "wt");

  if(output){
    fprintf(output, "nrows %d\n", n);
    fprintf(output, "ncols %d\n", n);
    fprintf(output, "xllcenter %0.6f\n", 0.0f);
    fprintf(output, "yllcenter %0.6f\n", 0.0f);
    fprintf(output, "cellsize 5.000000\n");
    fprintf(output, "NODATA_value  -9999\n");

    for(int i=0; i<n; i++){
      for(int j=0; j<n; j++)
        fprintf(output, "%0.2f ", altitude * (1.0f + cell[i][j] * scale)/2.0f);
      fprintf(output, "\n");
      if(i%100 == 0)printf(".");
    } //for
    printf("\n");

    fclose(output);
  } //if
} //SaveDEMFile

/// \brief Generate a cell of amortized noise.
///
/// Generate a cell of 2D amortized noise using the infinite amortized noise generator.
/// \param cell Pointer to array in which to store the noise.
/// \param x Tile column index.
/// \param y Tile row index.
/// \param m0 Largest octave.
/// \param m1 Smallest octave.
/// \param n Cell size.

float Generate2DNoise(float** cell, const int x, const int y, const int m0, const int m1, const int n){ 
  printf("Generating %d octaves of 2D noise.\n", m1 - m0 + 1);
  int t = CPUTimeInMilliseconds();
  float scale = g_pTerrainGenerator->generate(y, x, m0, m1, n, cell);
  t = CPUTimeInMilliseconds() - t;
  printf("Generated %d points in %0.2f seconds CPU time.\n", n*n, t/1000.0f);
  return scale;
} //Generate2DNoise

/// \brief Generate and save a cell of amortized noise.
///
/// Generate a cell of amortized noise and save it as a DEM file. This function
/// allocates memory for the cell, creates an infinite amortized noise generator,
/// uses it to generate the noise, saves it as a DEM file,
/// deallocates the generator and cell memory and returns.
/// \param nRow Tile row index.
/// \param nCol Tile column index.
/// \param m0 Largest octave.
/// \param m1 Smallest octave.
/// \param altitude Multiplier to convert noise to height.
/// \param n Cell size.

void GenerateAndSave2DNoise(int nRow, int nCol, const int m0, const int m1, 
                            const float altitude, const int n){ 

  //adjust for tile size of smallest octave
  for(int i=1; i<m0; i++){ 
    nCol *= 2; nRow *= 2;
  } //for

  //allocate space to store noise cells
  float** cell = new float* [n];
  for(int i=0; i<n; i++)
    cell[i] = new float [n]; 

  //generate and save noise
  float scale = Generate2DNoise(cell, nCol, nRow, m0, m1, n);
  SaveDEMFile(cell, n, scale, altitude, "output");

  //deallocate noise cell space
  for(int i=0; i<n; i++)
    delete [] cell[i];
  delete [] cell;

} //GenerateAndSave2DNoise

/// \brief Main.
///
/// Prompts the user for a random number seed, a tail multiplier and
/// an altitude cap and then generates and saves a 4096x4096 cell of terrain 
/// elevations generated using 8 octaves of amortized noise with an 
/// exponentially distributed gradient magnitude.
/// \param argc Argument count
/// \param argv Arguments.
/// \return 0 for success, 1 for failure.

int main(int argc, char *argv[]){ 
  printf("Amortized Noise Terrain Generator, Ian Parberry, 2014\n");
  printf("--------------------------------------------------------------\n\n");

  unsigned int seed = 1;
  printf("Hash seed:\n> "); scanf("%d", &seed);

  //get omega
  float omega = 0.3f;
  do{
    printf("Omega: ");
    scanf("%f", &omega);
    if(omega < 0.0f)
      printf("  Omega must be at least 1.\n");
    if(omega > 1.0f)
      printf("  Omega must be at most 1.\n");
  }while(omega < 0.0f || omega > 1.0f); 

  //get elevation cap
  float altitude = 5000.0f;
  do{
    printf("Elevation cap (meters): ");
    scanf("%f", &altitude);
    if(altitude <= 0.0f)
      printf("  Elevation cap must be greater than 0.\n");
  }while(altitude <= 0.0f); 

  const int cellsize = 4096;
  g_pTerrainGenerator = new CTerrainGenerator(cellsize, seed, omega);
  GenerateAndSave2DNoise(9999, 7777, 5, 12, altitude, cellsize);
  delete g_pTerrainGenerator;

#if defined(_MSC_VER) //Windows Visual Studio 
  //wait for user keystroke and exit
  printf("\nHit Almost Any Key to Exit...\n");
  _getch();
#endif

  return 0;
} //main
