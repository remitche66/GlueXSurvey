#include <iostream>
#include "RVersion.h"
#include "FSBasic/FSString.h"
#include "countingUtilities.h"

using namespace std;


// **************************************
//   MAIN
// **************************************


int main(int argc, char** argv){
  cout << endl;
  cout << "***********************************************************\n"
          "This program creates a set of histograms from a tree in the\n"
          "  \"FSRoot format\". \n\n"
          "Usage:  \n"
          "  surveyFSTreeToHist  \n"
          "        -in    <tree file name>                         (required) \n"
          "        -out   <hist file name>                         (required) \n"
          "***********************************************************\n";
  if (argc != 5){
     cout << "ERROR: wrong number of arguments -- see usage notes above" << endl;
     exit(0);
  }
  TString inTreeName("");
  TString outHistName("");
  for (int i = 0; i < argc-1; i++){
    TString argi(argv[i]);
    TString argi1(argv[i+1]);
    if (argi == "-in")   inTreeName = argi1;
    if (argi == "-out")  outHistName = argi1;
  }
  cout << endl;
  cout << "INPUT PARAMETERS:" << endl << endl;
  cout << "  input tree file:          " << inTreeName << endl;
  cout << "  output hist file:         " << outHistName << endl;
  cout << endl;
  if ((inTreeName == "") || (outHistName == "")){
     cout << "ERROR: specify input file and output file -- see usage notes above" << endl;
     exit(0);
  }
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,18,0)
  //FSHistogram::enableRDataFrame(true);
#endif
  writeHistograms(inTreeName, outHistName);
  //FSHistogram::executeRDataFrame();
  //writeHistograms(inTreeName, outHistName);
  cout << "*************************************************\n"
          "  FINISHED SurveyFSTreeToHist \n"
          "***********************************************************\n";
  return 0;
}

