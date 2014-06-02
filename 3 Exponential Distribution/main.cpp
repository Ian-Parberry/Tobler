/// \file Main.cpp
/// \brief Main.
///
/// \mainpage Exponentially Distributed Random Numbers
///
/// This project tests the generation of exponentially distributed random
/// numbers. For more details on why this is an interesting and useful thing
/// to do, see Ian Parberry, "Tobler's First Law of Geography, Self Similarity,
/// and Perlin Noise: A Large Scale Analysis of Gradient Distribution in 
/// Southern Utah with Application to Procedural Terrain Generation". 
/// You don't actually need to know much about the paper to try it out, 
/// but you at least need to know some of the concepts.
///
/// The program will prompt you for the tail multiplier omega (a number 
/// between 0 and 1). It saves the results in a text file distribution.txt.

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

#include <stdio.h>
#include <stdlib.h>

#include "Random.h"
#include "defines.h"

const int GRANULARITY = 100; ///< Granularity at which to measure the distribution.
const int REPEATS = 10000000; ///< Number of times we repeat the experiment.

int g_nDistribution[GRANULARITY]; ///< Experimental frequency distribution.
int g_nMissedSmall; ///< How many experiments were too small (should be 0).
int g_nMissedLarge; ///< How many experiments were too large (should be 0).
float g_fMin; ///< Smallest value found (ideally close to 0).
float g_fMax;  ///< Largest value found (ideally close to 1).

/// \brief Reset the measured distribution.
/// 
/// Reset all the statistics to zero. Call this before you start measuring.

void ResetDistribution(){
  g_fMax = -999999.0f; g_fMin = 999999.0f;
  g_nMissedSmall = g_nMissedLarge = 0;
  for(int j=0; j<GRANULARITY; j++)
    g_nDistribution[j] = 0;
} //ResetDistribution

/// \brief Run an experiment.
///
/// Run an experiment to measure the frequency distribution.
/// \param omega The tail multiplier, which controls how low the tails of the distribution can be.
/// \param n Number of times to repeat the experiment.

void RunExperiment(float omega, int n){
  ResetDistribution();
  for(int i=0; i<n; i++){ //run the experiment n times
    float fSample = ExpRand(omega); //random sample
    g_fMin = min(g_fMin, fSample); g_fMax = max(g_fMax, fSample);
    int sample = (int)(fSample*(GRANULARITY-1)); //discrete sample
    if(sample < 0)g_nMissedSmall++; //too small (should never happen)
    else if(sample >= GRANULARITY)g_nMissedLarge++; //too large (should never happen)
    else g_nDistribution[sample]++; //just right
  } //for
} //RunExperiment

/// \brief Save.
///
/// Save the frequency distribution.
/// \param filehandle File handle opened for writing.

void SaveDistribution(FILE* filehandle){
  if(filehandle){     
    for(int i=0; i<GRANULARITY; i++)
      fprintf(filehandle, "%0.4f\n", (float)g_nDistribution[i]/REPEATS);
    fprintf(filehandle, "\n");
  } //if
} //SaveDistribution

/// \brief Check the frequency distribution and report to console.
/// 
/// Check the frequency distribution and report interesting things to console.

void CheckDistribution(){
  if(g_nMissedSmall + g_nMissedLarge > 0)
    printf("Missed %d small, %d large\n", g_nMissedSmall, g_nMissedLarge);
  printf("%d experiments, Min = %0.4f, Max = %0.4f\n", REPEATS, g_fMin, g_fMax);
  int sum = 0;
  for(int i=0; i<GRANULARITY; i++)
    sum += g_nDistribution[i];
  printf("%d successes out of %d\n", sum, REPEATS);
} //CheckDistribution

/// \brief Main.
///
/// Prompts the user for a random number seed and a tail multiplier,
/// then measures statistics, and saves information about the distribution
/// to a file.
/// \param argc Argument count
/// \param argv Arguments.
/// \return 0 for success, 1 for failure.

int main(int argc, char *argv[]){ 
  printf("Exponentially Distributed Random Numbers, Ian Parberry, 2014\n");
  printf("-------------------------------------------------------------\n");

  unsigned int seed = 1;
  printf("Enter a hash seed for the pseudorandom number generator.\n");
  printf("Hash seed: "); scanf("%d", &seed);
  srand(seed); //seed the random number generator.

  //get omega
  float omega = 0.5f;
  do{
    printf("Enter a value between 0 and 1 for omega, the tail multiplier.\n");
    printf("Omega: ");
    scanf("%f", &omega);
    if(omega < 0.0f)
      printf("  Omega must be at least 0.\n");
    if(omega > 1.0f)
      printf("  Omega must be at most 1.\n");
  }while(omega < 0.0f || omega > 1.0f); 

  //open the output file
  FILE* distributionfile = fopen("distribution.txt", "wt");

  if(distributionfile){
    //do the actual work
    RunExperiment(omega, REPEATS);
    SaveDistribution(distributionfile);
    CheckDistribution();

    //clean up and exit
    fclose(distributionfile);
  } //if
  else printf("Save failed.\n");

#if defined(_MSC_VER) //Windows Visual Studio 
  //wait for user keystroke and exit
  printf("\nHit Almost Any Key to Exit...\n");
  _getch();
#endif

  return 0;
} //main
