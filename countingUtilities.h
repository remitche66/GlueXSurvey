#if !defined(COUNTINGUTILITIES_H)
#define COUNTINGUTILITIES_H

#include "TCanvas.h"
#include "TKey.h"
#include "FSBasic/FSSystem.h"
#include "FSBasic/FSString.h"
#include "FSBasic/FSCut.h"
#include "FSMode/FSModeInfo.h"
#include "FSMode/FSModeCollection.h"
#include "FSMode/FSModeHistogram.h"


// ************************
// define a new modeCode3 (flexible)
// ************************
//  modeCode3 = abcde
//               a: #phi
//               b: #omega
//               c: #etaprime
//               d: #eta000
//               e: #eta+-0


class ModeCode3Particle{
  public:
    TString name;
    FSModeInfo* decay;
    TString mass;
    TString cut;
};
void setModeCode3Particles();
vector<int> getModeCode3Numbers(int modeCode3);
FSModeInfo addModeCode3(FSModeInfo mi, int modeCode3, int sign = 1);
FSModeInfo getRecoilBaryon(FSModeInfo mi);
FSModeInfo removeRecoilBaryon(FSModeInfo mi);
vector<TString> getParticles(FSModeInfo mi, int modeCode3);
TString getDescription(FSModeInfo mi, int modeCode3);
TString getComboFormat(FSModeInfo mi, int modeCode3, int offset = 0);
TString getSubModeCuts(int modeCode3);
TString getOtherCuts(FSModeInfo mi, int modeCode3, TString cutCode);
vector<int> getSubModes(FSModeInfo mi);
vector<TString> getMassCombinations(FSModeInfo mi, int modeCode3);
vector<TString> getHistogramList(FSModeInfo mi);
TH1F* getTH1F(TString fileName, TString treeName, TString histName);
void setMCComponentsFromTreeFile(TString treeFileName, TString treeName, FSModeInfo mi);
void setMCComponentsFromHistFile(TString histFileName, FSModeInfo mi);
void writeHistograms(TString treeFileName, TString histFileName);
TString readHistograms(TString histFileName);
pair<TString,TString> makeFigure(TString histFileName, TString subMode, TString histType, TString outputFigures);
void makePDF(TString histFileName, TString outputDirectory, TString baseName);
vector< vector<int> > pushBackToEach(vector< vector<int> > originalList, vector<int> newPart);
vector<TString> expandIntegers(TString input);

#endif
