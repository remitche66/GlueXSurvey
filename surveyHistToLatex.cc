#include <iostream>
#include "FSBasic/FSString.h"
#include "countingUtilities.h"

using namespace std;


// **************************************
//   MAIN
// **************************************


int main(int argc, char** argv){
  cout << endl;
  cout << "***********************************************************\n"
          "This program creates a latex file from a histogram file. \n"
          "The histogram file comes from the surveyFSTreeToHist script. \n\n"
          "Usage:  \n"
          "  surveyHistToLatex  \n"
          "        -in    <hist file name>                     (required) \n"
          "        -dir   <output directory>                   (required) \n"
          "***********************************************************\n";
  if (argc != 5){
     cout << "ERROR: wrong number of arguments -- see usage notes above" << endl;
     exit(0);
  }
  TString inHistName("");
  TString outputDirectory("");
  for (int i = 0; i < argc-1; i++){
    TString argi(argv[i]);
    TString argi1(argv[i+1]);
    if (argi == "-in")   inHistName = argi1;
    if (argi == "-dir")  outputDirectory = argi1;
  }
  cout << endl;
  cout << "INPUT PARAMETERS:" << endl << endl;
  cout << "  input hist file:         " << inHistName << endl;
  cout << "  output directory:        " << outputDirectory << endl;
  cout << endl;
  if ((inHistName == "") || (outputDirectory == "")){
     cout << "ERROR: specify input file and output directory -- see usage notes above" << endl;
     exit(0);
  }
  makePDF(inHistName, outputDirectory);
  return 0;
}

