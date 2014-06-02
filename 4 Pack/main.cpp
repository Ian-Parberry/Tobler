/// \file Main.cpp
/// \brief Main.
///
/// \mainpage DEM File Packer
///
/// This is the DEM file packer from Ian Parberry, "Tobler's First 
/// Law of Geography, Self Similarity, and Perlin Noise: A Large Scale Analysis
/// of Gradient Distribution in Southern Utah with Application to Procedural
/// Terrain Generation".
/// You will need to download the DEM files listed in filelist20x20.txt
/// from the UARGRC before running it.
/// This program outputs a packed elevation data file called UtahDEMData.bin
/// which you should move to the Analyze folder before running the
/// Terrain Gradient Analyzer.
///
/// Don't execute this program unless you have at least 16GB of RAM (preferably
/// 32GB). A solid state hard drive is also recommended. Otherwise it may take
/// a very, very long time to run instead of just a long time. On a good
/// day on a fairly recent PC that has nothing much else to do at the
/// same time, running downhill with a tail wind, assuming that
/// the gods are with you and your karma is good and that your computer, in
/// its infinite wisdom, doesn't decide on a whim that it must install
/// 128 important updates and and shut down NOW, then it should
/// take between 30 and 45 minutes to run. Otherwise your guess is
/// as good as mine. This program does report various interesting things
/// to the console so you can reassure yourself that it is actually
/// working and not just hanging around hogging system resources.
///
/// If you are thinking about trying it out on a single file before going
/// Whole Hog, just uncomment out the \#define TESTRUN and provide a file
/// named filelisttestrun.txt with the name of a single DEM file in it.

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

#include "defines.h"

//#define TESTRUN //undefine for real run, define for a small single-file test

#ifdef TESTRUN
  char filenamefilename[] = "filelisttestrun.txt"; ///< Name of a file that contains a list of file names.
  const int GRIDSIZE = 1; ///< Number of DEM files on one side of square grid.
#else
  char filenamefilename[] = "filelist20x20.txt"; ///< Name of a file that contains a list of file names.
  const int GRIDSIZE = 20; ///< Number of DEM files on one side of square grid.
#endif

const int CELLSIZE = 4000; ///< Number of points on one side of a DEM file.
const int ARRAYSIZE = CELLSIZE*GRIDSIZE; ///< Number of points on one side of the whole array of heights.

long long g_nPointCount = 0LL; ///< Number of points processed.
long long g_nBadPointCount = 0LL;  ///< Number of points with bad data processed.

unsigned short** g_nHeight; ///< Buffer for packed height data.

  

/// \brief Get time.
/// 
/// Get the current time in milliseconds.
/// \return Time in ms.

int GetTime(){
  #if defined(_MSC_VER) //Windows Visual Studio 
    return timeGetTime();
  #else //other OS
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec*1000 + t.tv_usec/1000;
  #endif
} //GetTime

/// \brief Read height data.
///
/// Read the height data from a single DEM file and put it into the correct place
/// in g_pHeight,  which it assumes points to a buffer of the right size.
/// \param row Y coordinate of cell.
/// \param col X coordinate of cell.
/// \param filename Name of DEM file to be read.
/// \return true if it succeeds, false if it fails

bool ReadHeightData(int row, int col, char* filename){ 
  row *= CELLSIZE; col *= CELLSIZE;
  FILE* input = fopen(filename, "rt");
  if(input){
    char buffer[256];
    float fFoobar; //A place to dump the trash.

    //skip over header
    for(int i=0; i<6; i++)
     fscanf(input, "%s %f\n", buffer, &fFoobar);

    //get data
    float h;
    for(int i=0; i<CELLSIZE; i++)
      for(int j=0; j<CELLSIZE; j++){
        fscanf(input, "%f", &h);
        if(h > 0.0f)
          g_nHeight[row + i][col + j] = (unsigned short)(h * 10.0f);
        else{
          g_nHeight[row + i][col + j] = 0;
          g_nBadPointCount++;
        } //else
        g_nPointCount++;
      } //for
    fclose(input);
    return true;
  } //if
  else{
    printf(" FAILED\n");
    return false;
  } //else
} //ReadHeightData

/// \brief Main.
///
/// Does initialization, allocates memory, reads the list of file names,
/// reads those files into the height buffer g_pHeight, saves the height
/// buffer into UtahDEMData.bin, deallocates the memory and shuts down.
/// \param argc Argument count
/// \param argv Arguments.
/// \return 0 for success, 1 for failure.

int main(int argc, char *argv[]){ 
  int nStartTime;

  //grab memory for the grid
  nStartTime = GetTime();
  printf("Allocating memory...\n");
  g_nHeight = new unsigned short* [ARRAYSIZE];
  for(int i=0; i<ARRAYSIZE; i++)
    g_nHeight[i] = new unsigned short [ARRAYSIZE];
  printf("  %lld bytes allocated in %0.2f seconds.\n", (long long)ARRAYSIZE*ARRAYSIZE*sizeof(unsigned short),
    (float)(GetTime() - nStartTime)/1000.0f);

  //read height files
  nStartTime = GetTime();
  printf("Reading height data\n");
  char filenamefilename[32] = "filelist20x20.txt";
  FILE* filenamefile = fopen(filenamefilename, "rt");
  if(filenamefile){
    printf("Opened %s\n", filenamefilename);
    for(int i=0; i<GRIDSIZE; i++)
      for(int j=0; j<GRIDSIZE; j++){
        char filename[256];
        fscanf(filenamefile, "%s", filename);
        ReadHeightData(i, j, filename);
        printf(".");
      } //for
    fclose(filenamefile);
    printf("\n");
    printf("  Height data read in %0.2f seconds.\n", (float)(GetTime() - nStartTime)/1000.0f);
    printf("  Read %lld points, %lld of which were bad.\n", g_nPointCount, g_nBadPointCount);

    //output the packed data
    FILE* outputfile = fopen("UtahDEMData.bin", "wbS");
    printf("Writing height data...");
    if(outputfile){
      size_t count = 0;
      const long long llRecordSize = (size_t)ARRAYSIZE*sizeof(unsigned short);
      nStartTime = GetTime();
      for(int i=0; i<ARRAYSIZE; i++)
        count += fwrite(g_nHeight[i], llRecordSize, 1, outputfile);
      printf("  %lld bytes written in %0.2f seconds.\n", count*llRecordSize, (float)(GetTime() - nStartTime)/1000.0f);
      fclose(outputfile);
    } //if
    else printf("  Write failed.\n");
  } //if
  else printf("  Failed to read file list %s.\n", filenamefilename);

  //recover grid memory
  printf("Deallocating memory...\n");
  nStartTime = GetTime();
  for(int i=0; i<ARRAYSIZE; i++)
    delete [] g_nHeight[i];
  delete [] g_nHeight;
  printf("  Memory deallocated in %0.2f seconds.\n", (float)(GetTime() - nStartTime)/1000.0f);

#if defined(_MSC_VER) //Windows Visual Studio 
  //wait for user keystroke and exit
  printf("\nHit Almost Any Key to Exit...\n");
  _getch();
#endif

  return 0;
} //main
