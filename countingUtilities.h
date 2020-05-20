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
// define a new modeCode3
// ************************
//  modeCode3 = abcd
//               a: #phi
//               b: #omega
//               c: #etaprime
//               d: #eta+-0


class ModeCode3Particle{
  public:
    TString name;
    FSModeInfo* decay;
    TString mass;
    TString cut;
};
void setModeCode3Particles();
vector<TString> getParticles(int modeCode1, int modeCode2, int modeCode3);
TString getDescription(TString subMode);
TString getDescription(int modeCode1, int modeCode2, int modeCode3);
TString getFSCode(int modeCode1, int modeCode2, int modeCode3);
vector< vector<int> > getComboIndices(int modeCode3);
TString getComboFormat(int modeCode1, int modeCode2, int modeCode3, int fullModeCode3);
TString removeRecoilBaryon(int& modeCode1, int& modeCode2, int& modeCode3);
TString getSubModeCuts(int modeCode3);
TString getOtherCuts(int modeCode1, int modeCode2, int modeCode3, TString cutCode);
vector<TString> getSubModes(TString fsCode);
vector<TString> getMassCombinations(TString subMode);
vector<TString> getHistogramList(TString fsCode);
TString getTreeNameFromFile(TString fileName);
TH1F* getTH1F(TString fileName, TString treeName, TString histName);
void writeHistograms(TString treeFileName, TString histFileName);
TString readHistograms(TString histFileName);
pair<TString,TString> makeFigure(TString histFileName, TString subMode, TString histType, TString outputFigures);
void makePDF(TString histFileName, TString outputDirectory, TString baseName);
vector< vector<int> > pushBackToEach(vector< vector<int> > originalList, vector<int> newPart);
vector<TString> expandIntegers(TString input);

#endif
