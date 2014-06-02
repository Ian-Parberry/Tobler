/// \file Main.cpp
/// \brief Main.
///
/// \mainpage Terrain Gradient Analyzer
///
/// This is the gradient analysis program from Ian Parberry, "Tobler's First 
/// Law of Geography, Self Similarity, and Perlin Noise: A Large Scale Analysis
/// of Gradient Distribution in Southern Utah with Application to Procedural
/// Terrain Generation".
/// You will need to download DEM files from the UARGRC and run the
/// Pack program on them to create UtahDEMData.bin before the thought of using
/// this program should even consider the merest possibility of crossing
/// your mind. Go look in the Pack folder in this archive.
///
/// This program reads a packed elevation data file called UtahDEMData.bin
/// and records various interesting statistics in a tab-separated
/// text file called output.txt.
/// It must be compiled for a 64-bit target because
/// it uses a quite large array to store the data. Don't execute it
/// unless you have at least 16GB of RAM (preferably 32GB). A solid
/// state hard drive is also recommended. Otherwise it may take a
/// very, very long time to run instead of just a long time. On a good
/// day on a fairly recent PC that has nothing much else to do at the
/// same time, running downhill with a tail wind, assuming that
/// the gods are with you and your karma is good and that your computer, in
/// its infinite wisdom, doesn't decide on a whim that it must install
/// 128 important updates and and shut down NOW, then it should
/// take between 15 and 30 minutes to run. Otherwise your guess is
/// as good as mine. This program does report various interesting things
/// to the console so you can reassure yourself that it is actually
/// working and not just hanging around hogging system resources.

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

#include <stdio.h>  //for printf()
#include <stdlib.h>  //for malloc() and free()
#include <math.h> //for abs()

#include "defines.h" //OS porting defines
#include "CPUtime.h" //measure CPU time
  
const int GRIDSIZE = 20; ///< Number of DEM files on one side of square grid.

const int CELLSIZE = 4000; ///< Number of points on one side of a DEM file.
const unsigned int ARRAYSIZE = CELLSIZE*GRIDSIZE; ///< Number of points on one side of the whole array of heights.

const int NUMOCTAVES = 16;  ///< Number of octaves.
const int MIDOCTAVE = 9; ///< First octave at which width is greater than height.

const int GRANULARITY = 50; ///< Granularity of distribution.
const double MAXGRADIENT = 1.0; ///< Maximum gradient sampled.

const double GRADIENT_DELTA = (double)MAXGRADIENT/GRANULARITY; ///< Small change in gradient.
const double GRADIENT_DELTA_INV = 1.0/GRADIENT_DELTA; ///< Inverse of GRADIENT_DELTA.

double g_dMaxSlope[NUMOCTAVES]; ///< Maximum slope in each octave.
double g_dSumSlope[NUMOCTAVES]; ///< Sum of slopes in each octave, for computing averages.
long long g_nPointCount[NUMOCTAVES]; ///< Number of samples.
long long g_nDistribution[NUMOCTAVES][GRANULARITY]; ///< Gradient distribution for each octave.

unsigned char* g_pHeightBuffer = NULL; ///< Buffer for packed height data.
unsigned short* g_nHeight; ///< Swizzle to access height data by word instead of by byte.
const long long g_nBufSize = 
  (long long)ARRAYSIZE * (long long)ARRAYSIZE * (long long)sizeof(short); ///< Size of height buffer.

int g_nStartTime; ///< Time that computation began.

/// \brief Read height data.
///
/// Read the height data from a packed file UtahDEMData.bin into g_pHeightBuffer,
/// which it assumes points to a buffer of the right size. This data will be
/// accessed automagically through g_nHeight.
/// \return true if it succeeds, false if it fails

bool ReadHeightData(){  
  FILE* datafile;
  datafile = fopen("UtahDEMData.bin", "rb");
  
  if(datafile){ //success
    size_t count = fread(g_pHeightBuffer, g_nBufSize, 1, datafile);
    printf("  %llud bytes read", count * g_nBufSize);
    fclose(datafile);
    return true; //success
  } //if
  else return false; //fail
} //ReadHeightData

/// \brief Record statistics for a gradient. 
///
/// Add the new gradient to the octave statistics we are gathering.
/// \param g A new gradient.
/// \param k The octave from which the gradient was recorded.

void RecordGradient(double g, int k){
  if(g < MAXGRADIENT){ //gradient is good
    g_nPointCount[k]++; //one more slope
    g_dMaxSlope[k] = max(g_dMaxSlope[k], g); //compare to maximum
    g_dSumSlope[k] += g; //add to sum

    //add to distribution
    int n = (int)(g*GRADIENT_DELTA_INV + 0.5f);
    if(n>=0 && n<GRANULARITY)
      g_nDistribution[k][n]++;
  } //if
} //RecordGradient

/// \brief Get the height.
///
/// Get the elevation at a given point.
/// \param x X coordinate of point.
/// \param y Y coordinate of point.
/// \return Elevation at (x, y).

double ht(int x, int y){
  return (double)g_nHeight[x*ARRAYSIZE + y];
}; //ht

/// \brief Process the slope data.
///
/// Process all of the slopes for a given octave.
/// \param k The octave number.

void ProcessSlopeData(int k){
  int m = 1; //to adjust for octaves longer than they are high.
  int scale=1; //octave scale
  double length = 5.0; //distance between sample points
  int nStartTime = CPUTimeInMilliseconds();

  //set scale and length for this octave.
  for(int i=0; i<k; i++){
    scale *= 2; length *= 2.0;
  } //for

  //adjust multiplier for long octaves.
  for(int i=MIDOCTAVE; i<k; i++)
    m *= 2;
    
  //record gradients at each point
  const double d = 10.0*length; //divisor
  for(int i=0; i<ARRAYSIZE; i++) //for each row 
    for(int j=0; j<ARRAYSIZE; j++){ //for each point
      if(g_nHeight[i*ARRAYSIZE + j] > 0){ //current point has good data
        double h0 = ht(i, j); //ht at current point
        if(i+scale < ARRAYSIZE){ //if not at right edge
          double h1 = ht(i+scale, j); //height at next point to the right
          if(h1 > 0) //if the data value there is good
            RecordGradient(m*fabs(h0 - h1)/d, k); //record the gradient
        } //if not at right edge

        if(j+scale < ARRAYSIZE){ //if not at bottom edge
          double h2 = ht(i, j + scale); //height at next point down
          if(h2 > 0) //if the data value there is good
            RecordGradient(m*fabs(h0 - h2)/d, k); //record the gradient
        } //if not at bottom edge
      } //if current point has good data
    } //for each point
} //ProcessSlopeData

/// \brief Save slope statistics.
///
/// Save the gradient statistics to a tab-separated text file output.txt.

void SaveSlopeStats(){
  printf("Saving results.\n");

  FILE* output;
  output = fopen("output.txt", "wt");
  if(output){ //if file opened successfully
    fprintf(output, "Gradients:\t");
    for(int i=0; i<NUMOCTAVES; i++)
      fprintf(output, "%lld\t", g_nPointCount[i]);
    fprintf(output, "\n");

    fprintf(output, "Mean\t");
    for(int i=0; i<NUMOCTAVES; i++)
      if(g_nPointCount[i] > 0)
        fprintf(output, "%0.4f\t", g_dSumSlope[i]/(double)g_nPointCount[i]);
      else fprintf(output, "0.0000\t");
    fprintf(output, "\n");

    fprintf(output, "Max\t");
    for(int i=0; i<NUMOCTAVES; i++)
      fprintf(output, "%0.4f\t", g_dMaxSlope[i]); 
    fprintf(output, "\n\n");
  
    double g = 0.0;
    for(int j=0; j<GRANULARITY; j++){
      g += GRADIENT_DELTA;
      fprintf(output, "%0.2f\t", g);
      for(int i=0; i<NUMOCTAVES; i++)
        if(g_nDistribution[i][j] > 0)
          fprintf(output, "%0.4f\t", 100.0f * 
            (float)((double)g_nDistribution[i][j])/(double)g_nPointCount[i]);
        else fprintf(output, "0\t"); //this helps see the ones that are *really* zero
      fprintf(output, "\n");
    } //for

    fclose(output);
  } //if file opened successfully
  else printf("Failed to save results.\n"); //fail
} //SaveSlopeStats

/// \brief Main.
///
/// Does initialization, allocates a honking big block of memory (so you'd better
/// compile this in 64 bit mode, and hopefully you have at least 16GB RAM
/// onboard), reads the data, performs the analysis, saves the results,
/// deallocates the memory and shuts down.
/// \param argc Argument count
/// \param argv Arguments.
/// \return 0 for success, 1 for failure.

int main(int argc, char *argv[]){ 

  //initialize stats
  for(int k=0; k<NUMOCTAVES; k++){ 
    g_dMaxSlope[k] = -9999;
    g_dSumSlope[k] = 0.0;
    g_nPointCount[k] = 0LL;

    for(int i=0; i<GRANULARITY; i++)
      g_nDistribution[k][i] = 0LL;
  } //for

  //grab memory for the grid
  printf("Allocating memory.\n");
  g_pHeightBuffer = (unsigned char*)malloc(g_nBufSize);
  g_nHeight = (unsigned short*)g_pHeightBuffer;

  //read height files
  printf("Reading height data.\n");
  g_nStartTime = CPUTimeInMilliseconds();
  if(ReadHeightData()){ //process the height data
    printf(" in %0.2f seconds.\n", (float)(CPUTimeInMilliseconds() - g_nStartTime)/1000.0f);    
    printf("Processing height data.\n");
    g_nStartTime = CPUTimeInMilliseconds();
    printf("  ");
    for(int k=0; k<NUMOCTAVES; k++){
      printf("%d ", k+1);
      ProcessSlopeData(k);
    } //for
    printf("\nHeight data processed in %0.2f seconds.\n", (float)(CPUTimeInMilliseconds() - g_nStartTime)/1000.0f);
    SaveSlopeStats();
  } //if
  else printf("Read fail\n");

  //recover grid memory
  printf("Deallocating memory...\n");
  if(g_pHeightBuffer)
    free(g_pHeightBuffer);

#if defined(_MSC_VER) //Windows Visual Studio 
  //wait for user keystroke and exit
  printf("\nHit Almost Any Key to Exit...\n");
  _getch();
#endif

  return 0;
} //main
