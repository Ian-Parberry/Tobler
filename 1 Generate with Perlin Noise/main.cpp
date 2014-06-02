/// \file Main.cpp
/// \brief Main.
///
/// \mainpage Perlin Noise Terrain Generator
///
/// This is the Perlin noise based terrain generator from Ian Parberry,
/// "Tobler's First Law of Geography, Self Similarity, and Perlin Noise:
/// A Large Scale Analysis of Gradient Distribution in Southern Utah with 
/// Application to Procedural Terrain Generation". You don't actually need to 
/// know much about the  paper to try it out, but you at least need to know 
/// some of the concepts.  It will generate a square cell of terrain elevations 
/// using Perlin noise with an exponentially distributed gradient magnitude and
/// save it in a DEM  file called output.asc. After that you are on your own, 
/// which you may find disconcerting. If I might make a small suggestion, 
/// Terragen 3 will do an   excellent job of rendering your terrain, and it 
/// reads DEM files natively. You can download an only mildly crippled version 
/// for free from  http://planetside.co.uk/products/terragen3, but I should 
/// in all good faith warn you that the process of learning its foibles is 
/// not for the faint of heart.
///
/// The program will prompt you for a random number seed (9999 is as good
/// as any), number of octaves (try 8 if you are not sure), the height exponent
/// mu (a number between 1.0 and 1.16-ish, try 1.002 if your imagination fails
/// you), and an elevation cap in meters (somewhat tenuously related to the
/// maximum height of the terrain, try 5000).

// Copyright Ian Parberry, May 2014.
//
// This file is made available under the GNU All-Permissive License.
// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.
//
// Created by Ian Parberry, May 2014.
// Last updated May 29, 2014.

#include <stdio.h> //for printf()
#include <stdlib.h> //for srand()

#include "defines.h" //OS porting defines
#include "perlin.h" //Perlin noise


const int CELLSIZE = 4096; ///< Number of vertices on side of square cell.

int g_nNumOctaves = 8; ///< Number of octaves.
float g_fAltitude = 5000.0f; ///< Altitude cap.
float g_fMu = 1.02f; ///< Mu, the gradient magnitude exponent.

/// \brief Generate and save a cell of terrain elevations.
///
/// Generate and save a cell of noise as a DEM file. The output file
/// will have a ".asc" file expension, which is standard for DEM files.
/// \param x X coordinate of corner of cell.
/// \param y Y coordinate of corner of cell.
/// \param filename Name of DEM file for output, without extension.

void GenerateAndSave(const unsigned int x, const unsigned int y, const char* filename){  
  FILE* output = fopen(filename, "wt");
  if(output){
    //header
    fprintf(output, "nrows %d\n", CELLSIZE);
    fprintf(output, "ncols %d\n", CELLSIZE);
    fprintf(output, "xllcenter %0.6f\n", 0.0f);
    fprintf(output, "yllcenter %0.6f\n", 0.0f);
    fprintf(output, "cellsize 5.000000\n");
    fprintf(output, "NODATA_value  -9999\n");

    //minimum and maximum height to report to console
    float maxht = -9999.9f;
    float minht = 9999.9f;

    //generate and save terrain heights
    for(int i=0; i<CELLSIZE; i++){
      for(int j=0; j<CELLSIZE; j++){   
        float pnoise = PerlinNoise2D(x + i/256.0f, y + j/256.0f, g_nNumOctaves); //Perlin noise
        float ht = g_fAltitude*0.5f*(1.0f + pnoise); //height
        fprintf(output, "%0.2f ", ht); 
        minht = min(minht, ht);
        maxht = max(maxht, ht);
      } //for
      fprintf(output, "\n");
      if(i%100 == 0)printf(".");
    } //for

    //report good things and close out
    printf("\nElevation Min = %0.2f, Max = %0.2f\n", minht, maxht);
    fclose(output);
  } //if
  else printf("Save failed.\n");
} //GenerateAndSave

/// \brief Main.
///
/// Prompts the user for a random number seed, number of octaves,
/// mu, and an elevation cap, saves a DEM file output.asc of
/// terrain elevations generated using Perlin noise with an exponentially
/// distributed gradient magnitude.
/// \param argc Argument count
/// \param argv Arguments.
/// \return 0 for success, 1 for failure.

int main(int argc, char *argv[]){ 
  printf("Perlin Noise Terrain Generator, Ian Parberry, 2014\n");
  printf("-------------------------------------------\n");

  //get random number seed
  int seed = 9999;
  printf("Random number seed: ");
  scanf("%d", &seed);

  //get number of octaves
  do{
    printf("Number of octaves: ");
    scanf("%d", &g_nNumOctaves);
    if(g_nNumOctaves < 1)
      printf("  Number of octaves must be at least 1.\n");
  }while(g_nNumOctaves < 1); 

  //get mu
  do{
    printf("Mu: ");
    scanf("%f", &g_fMu);
    if(g_fMu < 1.0f)
      printf("  Mu must be at least 1.\n");
    if(g_fMu > 1.16f)
      printf("  Mu must be at most 1.16.\n");
  }while(g_fMu < 1.0f || g_fMu > 1.16f); 

  //get elevation cap
  do{
    printf("Elevation cap (meters): ");
    scanf("%f", &g_fAltitude);
    if(g_fAltitude <= 0.0f)
      printf("  Elevation cap must be greater than 0.\n");
  }while(g_fAltitude <= 0.0f); 

  srand(seed); //seed the random number generator
  initPerlin2D(); //initialize the Perlin noise ganaerator
  GenerateAndSave(7777, 9999, "output.asc"); //generate noise cell and save as a DEM file.
  
#if defined(_MSC_VER) //Windows Visual Studio 
  //wait for user keystroke and exit
  printf("\nHit Almost Any Key to Exit...\n");
  _getch();
#endif

  return 0;
} //main
