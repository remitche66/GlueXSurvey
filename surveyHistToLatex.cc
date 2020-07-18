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
          "        -in2   (2nd hist file with thrown info)     (optional) \n"
          "        -dir   <output directory>                   (required) \n"
          "        -name  <output base name>                   (required) \n"
          "***********************************************************\n";
  if (argc != 7 && argc != 9){
     cout << "ERROR: wrong number of arguments -- see usage notes above" << endl;
     exit(0);
  }
  TString inHistName("");
  TString inHist2Name("");
  TString outputDirectory("");
  TString baseName("");
  for (int i = 0; i < argc-1; i++){
    TString argi(argv[i]);
    TString argi1(argv[i+1]);
    if (argi == "-in")   inHistName = argi1;
    if (argi == "-in2")  inHist2Name = argi1;
    if (argi == "-dir")  outputDirectory = argi1;
    if (argi == "-name") baseName = argi1;
  }
  cout << endl;
  cout << "INPUT PARAMETERS:" << endl << endl;
  cout << "  input hist file:         " << inHistName << endl;
  cout << "  input hist file 2:       " << inHist2Name << endl;
  cout << "  output directory:        " << outputDirectory << endl;
  cout << "  base name:               " << baseName << endl;
  cout << endl;
  if ((inHistName == "") || (outputDirectory == "") || (baseName == "")){
     cout << "ERROR: specify input file, output directory, and name -- see usage notes above" << endl;
     exit(0);
  }
  if  (isMCThrownFromHistFile(inHistName)) makePDFGen(inHistName, outputDirectory, baseName);
  if (!isMCThrownFromHistFile(inHistName)) makePDF(inHistName, inHist2Name, outputDirectory, baseName);
  return 0;
}

