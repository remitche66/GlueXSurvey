#if !defined(COUNTINGUTILITIES_H)
#define COUNTINGUTILITIES_H

#include "TCanvas.h"
#include "TKey.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLine.h"
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
void setModeCode3ParticlesEtc();
vector<int> getModeCode3Numbers(int modeCode3);
FSModeInfo addModeCode3(FSModeInfo mi, int modeCode3, int sign = 1);
FSModeInfo getRecoilBaryon(FSModeInfo mi);
FSModeInfo removeRecoilBaryon(FSModeInfo mi);
vector<TString> getParticles(FSModeInfo mi, int modeCode3);
TString getDescription(FSModeInfo mi, int modeCode3);
TString getComboFormat(FSModeInfo mi, int modeCode3, int offset = 0);
TString getSubModeCuts(int modeCode3);
TString getOtherCuts(FSModeInfo mi, int modeCode3, TString cutCode);
TString getMCCuts(TString mcComponent);
vector<int> getModeCode3List(FSModeInfo mi, bool show = false);
vector<TString> getMassCombinations(FSModeInfo mi, int modeCode3, bool show = false);
vector<TString> getHistogramList(FSModeInfo mi, bool isMC, bool isMCThrown, bool show = false);
TH1F* getTH1FFromTreeFile(TString treeFileName, TString treeName, TString histName);
TH1F* getTH1FFromHistFile(TString histFileName, TString histName);
pair<THStack*,TLegend*> getTHStFromHistFile(TString histFileName, TString histName);
void setMCComponentsFromTreeFile(TString treeFileName);
void setMCComponentsFromHistFile(TString histFileName);
void setMCComponentsGen(TString histFileName, int bin1 = 1, int bin2 = 140);
void writeHistograms(TString treeFileName, TString histFileName);
TString fsCodeFromHistFile(TString histFileName);
pair<TString,TString> makeMCFigure(TString histFileName, TString subMode, TString histType, TString outputFigures);
pair<TString,TString> makeFigure(TString histFileName, TString subMode, TString histType, TString outputFigures, TString histCompName = "", double compScale = 1.0);
void makePDF(TString histFileName, TString histFileName2, TString outputDirectory, TString baseName, TString histCompName = "", double compScale = 1.0);
void makePDFGen(TString histFileName, TString outputDirectory, TString baseName);
bool isMCFromTreeFile(TString treeFileName);
bool isMCThrownFromTreeFile(TString treeFileName);
bool isMCFromHistFile(TString histFileName);
bool isMCThrownFromHistFile(TString histFileName);
void drawZeroLine(TH1F* hist);
int color(int color);

#endif
