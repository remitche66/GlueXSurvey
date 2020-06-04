#include "countingUtilities.h"

vector<ModeCode3Particle> modeCode3Particles;
map<TString,int> particleOrder;

void setModeCode3Particles(){
  if (modeCode3Particles.size() > 0) return;
  FSControl::globalTweaks();
  ModeCode3Particle eta3pi; 
                    eta3pi.name  = "eta+-0";
                    eta3pi.decay = new FSModeInfo("pi+ pi- pi0");
                    eta3pi.mass  = "0.548";
                    eta3pi.cut   = "0.025";
  ModeCode3Particle eta3pi0; 
                    eta3pi0.name  = "eta000";
                    eta3pi0.decay = new FSModeInfo("pi0 pi0 pi0");
                    eta3pi0.mass  = "0.548";
                    eta3pi0.cut   = "0.025";
  ModeCode3Particle etaprime; 
                    etaprime.name  = "etaprime";
                    etaprime.decay = new FSModeInfo("eta pi+ pi-");
                    etaprime.mass  = "0.958";
                    etaprime.cut   = "0.025";
  ModeCode3Particle omega; 
                    omega.name  = "omega";
                    omega.decay = new FSModeInfo("pi+ pi- pi0");
                    omega.mass  = "0.782";
                    omega.cut   = "0.025";
  ModeCode3Particle phi; 
                    phi.name  = "phi";
                    phi.decay = new FSModeInfo("K+ K-");
                    phi.mass  = "1.020";
                    phi.cut   = "0.025";
  modeCode3Particles.push_back(eta3pi);
  modeCode3Particles.push_back(eta3pi0);
  modeCode3Particles.push_back(etaprime);
  modeCode3Particles.push_back(omega);
  modeCode3Particles.push_back(phi);
  particleOrder["gamma"]    =  1;
  particleOrder["e+"]       =  2;
  particleOrder["e-"]       =  3;
  particleOrder["mu+"]      =  4;
  particleOrder["mu-"]      =  5;
  particleOrder["phi"]      =  6;
  particleOrder["omega"]    =  7;
  particleOrder["K+"]       =  8;
  particleOrder["K-"]       =  9;
  particleOrder["Ks"]       = 10;
  particleOrder["eta"]      = 11;
  particleOrder["etaprime"] = 12;
  particleOrder["eta+-0"]   = 13;
  particleOrder["eta000"]   = 14;
  particleOrder["pi+"]      = 15;
  particleOrder["pi-"]      = 16;
  particleOrder["pi0"]      = 17;
  particleOrder["p-"]       = 18;
  particleOrder["p+"]       = 19;
  particleOrder["ALambda"]  = 20;
  particleOrder["Lambda"]   = 21;
}



vector<TString> getParticles(int modeCode1, int modeCode2, int modeCode3){
  setModeCode3Particles();
  vector<TString> particles;
  if (modeCode1 + modeCode2 > 0){ FSModeInfo mi(modeCode1,modeCode2); particles = mi.particles(); }
  int n1 = 1; int n2 = 10;
  for (unsigned int iP = 0; iP < modeCode3Particles.size(); iP++){
    int n = (((modeCode3%n2)-(modeCode3%n1))/n1); n1*=10; n2*=10;
    for (int i = 0; i < n; i++){ particles.push_back(modeCode3Particles[iP].name); }
  }
  if (particles.size() == 0) return particles;
  for (unsigned int i = 0; i < particles.size()-1; i++){
  for (unsigned int j = i+1; j < particles.size(); j++){
    if (particleOrder[particles[i]] > particleOrder[particles[j]]){
      TString temp = particles[i]; particles[i] = particles[j]; particles[j] = temp; }
  }}
  return particles;
}


TString getDescription(TString subMode){
  vector<TString> parts = FSString::parseTString(subMode,"_");
  if (parts.size() != 3) {cout << "problem in getDescription" << endl; exit(0); }
  int modeCode3 = FSString::TString2int(parts[0]);
  int modeCode2 = FSString::TString2int(parts[1]);
  int modeCode1 = FSString::TString2int(parts[2]);
  return getDescription(modeCode1,modeCode2,modeCode3);
}

TString getDescription(int modeCode1, int modeCode2, int modeCode3){
  vector<TString> particles = getParticles(modeCode1,modeCode2,modeCode3);
  TString description("");
  for (unsigned int i = 0; i < particles.size(); i++){
    description += particles[i];
    if (i != particles.size()-1) description += " ";
  }
  return description;
}

TString getFSCode(int modeCode1, int modeCode2, int modeCode3){
  setModeCode3Particles();
  int n1 = 1; int n2 = 10;
  for (unsigned int iP = 0; iP < modeCode3Particles.size(); iP++){
    int n = (((modeCode3%n2)-(modeCode3%n1))/n1); n1*=10; n2*=10;
    modeCode1 += n*modeCode3Particles[iP].decay->modeCode1();
    modeCode2 += n*modeCode3Particles[iP].decay->modeCode2();
  }
  TString fsCode = FSString::int2TString(modeCode2)+"_"+
                   FSString::int2TString(modeCode1);
  return fsCode;
}


vector< vector<int> > getComboIndices(int modeCode3){
  setModeCode3Particles();
  vector< vector<int> > indices;  indices.resize(modeCode3Particles.size());
  int indexStart = 6; vector<int> index;
  int n1 = 1; int n2 = 10;
  for (unsigned int iP = 0; iP < modeCode3Particles.size(); iP++){
    int n = (((modeCode3%n2)-(modeCode3%n1))/n1); n1*=10; n2*=10;
    index.clear();  for (int i = 0; i < n; i++){ index.push_back(indexStart++); } indices[iP] = index;
  }
  return indices;
}

TString getComboFormat(int modeCode1, int modeCode2, int modeCode3, int fullModeCode3){
  TString comboFormat("");
  if (modeCode1+modeCode2>0){ FSModeInfo mi(modeCode1,modeCode2); comboFormat = mi.modeComboFormat(1); }
  vector< vector<int> > comboIndices = getComboIndices(fullModeCode3);
  int n1 = 1; int n2 = 10;
  for (unsigned int iP = 0; iP < modeCode3Particles.size(); iP++){
    int n = (((modeCode3%n2)-(modeCode3%n1))/n1); n1*=10; n2*=10;
    for (unsigned int j = 0; j < n; j++){
      if (comboFormat != "") comboFormat += ",";
      comboFormat += modeCode3Particles[iP].decay->modeComboFormat(comboIndices[iP][j]);
    }
  }
  return comboFormat;
}

TString removeRecoilBaryon(int& modeCode1, int& modeCode2, int& modeCode3){
  setModeCode3Particles();
  FSModeInfo mi(modeCode1,modeCode2);
  FSModeInfo miProton("p+");
  FSModeInfo miLambda("Lambda");
  if (mi.modeContains(miProton.modeString())){ modeCode1 -= miProton.modeCode1();
                                               modeCode2 -= miProton.modeCode2(); 
                                               return miProton.modeString();}
  else if (mi.modeContains(miLambda.modeString())){ modeCode1 -= miLambda.modeCode1();
                                                    modeCode2 -= miLambda.modeCode2(); 
                                                    return miLambda.modeString();}
  cout << "no recoil proton or Lambda? quitting " << endl; exit(0);
  return TString("");
}


TString getSubModeCuts(int modeCode3){
  TString cuts("");
  vector< vector<int> > comboIndices = getComboIndices(modeCode3);
  for (unsigned int iP = 0; iP < comboIndices.size(); iP++){
    for (unsigned int j = 0; j < comboIndices[iP].size(); j++){
      if (cuts != "") cuts += "&&";
      cuts += "(abs(MASS("
               + modeCode3Particles[iP].decay->modeComboFormat(comboIndices[iP][j]) + ")-"
               + modeCode3Particles[iP].mass + ")<"
               + modeCode3Particles[iP].cut + ")";
    }
  }
  if (cuts == "") cuts = "(1==1)";
  return cuts;
}


TString getOtherCuts(int modeCode1, int modeCode2, int modeCode3, TString cutCode, TString histType){
  setModeCode3Particles();
  int modeCode1X = modeCode1; int modeCode2X = modeCode2; int modeCode3X = modeCode3;
  removeRecoilBaryon(modeCode1X,modeCode2X,modeCode3X);
  TString listX(getComboFormat(modeCode1X,modeCode2X,modeCode3X,modeCode3));
  FSCut::defineCut("T","(-1*MASS2("+listX+";GLUEXBEAM)<0.5)&&(REnPB>8.0)");
  if (histType == "T") FSCut::defineCut("T","(REnPB>8.0)");
  FSCut::defineCut("Chi2DOF","Chi2DOF<5.0","Chi2DOF>10.0&&Chi2DOF<15.0",1.0);
  if (histType == "CHI2DOF") FSCut::defineCut("Chi2DOF","(1==1)","(1==1)",1.0);
  FSCut::defineCut("RFDeltaT","abs(RFDeltaT)<2.0","abs(RFDeltaT)>2.0&&abs(RFDeltaT)<10.0",1.0/4.0);
  if (histType == "RFTIME") FSCut::defineCut("RfDeltaT","(1==1)","(1==1)",1.0);
  int icutCode = FSString::TString2int(cutCode);
  int iT      = (((icutCode%1000)-(icutCode%100))/100);
  int iCHI2   = (((icutCode%100)-(icutCode%10))/10);
  int iRFTIME = (((icutCode%10)-(icutCode%1))/1);
  TString cuts("");
  if (iT == 0)     {if (cuts != "") cuts += "&&"; cuts += "(1==1)"; }
  if (iT == 1)     {if (cuts != "") cuts += "&&"; cuts += "CUT(T)"; }
  if (iCHI2 == 0)  {if (cuts != "") cuts += "&&"; cuts += "(1==1)"; }
  if (iCHI2 == 1)  {if (cuts != "") cuts += "&&"; cuts += "CUT(Chi2DOF)"; }
  if (iCHI2 == 2)  {if (cuts != "") cuts += "&&"; cuts += "CUTSB(Chi2DOF)"; }
  if (iRFTIME == 0){if (cuts != "") cuts += "&&"; cuts += "(1==1)"; }
  if (iRFTIME == 1){if (cuts != "") cuts += "&&"; cuts += "CUT(RFDeltaT)"; }
  if (iRFTIME == 2){if (cuts != "") cuts += "&&"; cuts += "CUTSB(RFDeltaT)"; }
  return cuts;
}


vector<TString> getSubModes(TString fsCode){
  setModeCode3Particles();
  FSModeInfo miFS(fsCode);
  vector<TString> subModes; subModes.push_back("0_"+miFS.modeString());
  int max3 = 4;  TString smax3 = FSString::int2TString(max3);
  TString sMax(""); for (unsigned int i = 0; i < modeCode3Particles.size(); i++){ sMax += smax3; }
  vector<TString> modeCode3List = expandIntegers(sMax);
  for (unsigned int i = 0; i < modeCode3List.size(); i++){
    int modeCode3 = FSString::TString2int(modeCode3List[i]);
    if (modeCode3 == 0) continue;
    if (getParticles(0,0,modeCode3).size() > max3) continue;
    FSModeInfo mi3(getFSCode(0,0,modeCode3));
    if (!miFS.modeContains(mi3.modeString())) continue;
    subModes.push_back(FSString::int2TString(modeCode3)+"_"+
                       FSString::int2TString(miFS.modeCode2() - mi3.modeCode2())+"_"+
                       FSString::int2TString(miFS.modeCode1() - mi3.modeCode1()));
  }
//cout << "SUBMODES:" << endl;
//for (unsigned int i = 0; i < subModes.size(); i++){ cout << subModes[i] << endl; }
  return subModes;
}


vector<TString> getMassCombinations(TString subMode){
  vector<TString> massCombosAll = expandIntegers(subMode);
  if (massCombosAll.size() == 0) return massCombosAll;
  vector<TString> massCombos;
  for (unsigned int i = 0; i < massCombosAll.size()-1; i++){
    vector<TString> modeCodes = FSString::parseTString(massCombosAll[i],"_");
    int modeCode1 = FSString::TString2int(modeCodes[2]); 
    int modeCode2 = FSString::TString2int(modeCodes[1]); 
    int modeCode3 = FSString::TString2int(modeCodes[0]); 
    int n = getParticles(modeCode1,modeCode2,modeCode3).size();
    if (n < 2) continue;
    if (modeCode1 + modeCode2 > 0){
      FSModeInfo mi(modeCode1,modeCode2);
      if ((mi.modeBaryonNumber() == 1) && (n > 2)) continue;
      if ((mi.modeBaryonNumber() == 1) && (abs(mi.modeCharge()) > 2)) continue;
      if ((mi.modeBaryonNumber() != 1) && (abs(mi.modeCharge()) > 1)) continue;
    }
    massCombos.push_back(FSString::int2TString(modeCode3)+"_"
                        +FSString::int2TString(modeCode2)+"_"
                        +FSString::int2TString(modeCode1));
  }
//cout << "massCombos:" << endl;
//for (unsigned int i = 0; i < massCombos.size(); i++){ cout << massCombos[i] << endl; }
  return massCombos;
}



vector<TString> getHistogramList(TString fsCode){
  vector<TString> histogramList;
  vector<TString> subModes = getSubModes(fsCode);
  vector<TString> cutCombos = expandIntegers("122");
  for (unsigned int iS = 0; iS < subModes.size(); iS++){
    TString histIndexFS = "hist_FS_" + subModes[iS];
    vector<TString> massCombos = getMassCombinations(subModes[iS]);
    for (unsigned int iC = 0; iC < cutCombos.size(); iC++){
      TString histIndexCUT = "CUTS_" + cutCombos[iC];
      int icutCode = FSString::TString2int(cutCombos[iC]);
      int iT      = (((icutCode%1000)-(icutCode%100))/100);
      int iCHI2   = (((icutCode%100)-(icutCode%10))/10);
      int iRFTIME = (((icutCode%10)-(icutCode%1))/1);
        // ** MASS **
      if (iCHI2 != 0 && iRFTIME != 0){
        for (unsigned int iM = 0; iM < massCombos.size(); iM++){
          TString histIndexMASS = "MASS_" + massCombos[iM];
          histogramList.push_back(histIndexFS+"_"+histIndexCUT+"_"+histIndexMASS);
        }
      }
        // ** OTHERS **
      if (iCHI2 == 0 && iRFTIME != 0)
        histogramList.push_back(histIndexFS+"_"+histIndexCUT+"_CHI2DOF");
      if (iCHI2 != 0 && iRFTIME != 0) 
        histogramList.push_back(histIndexFS+"_"+histIndexCUT+"_T");
      if (iCHI2 != 0 && iRFTIME == 0) 
        histogramList.push_back(histIndexFS+"_"+histIndexCUT+"_RFTIME");
    }
  }
//cout << "histogramList:" << endl;
//for (unsigned int i = 0; i < histogramList.size(); i++){ cout << histogramList[i] << endl; }
  return histogramList;
}


TString getTreeNameFromFile(TString fileName){
  TString treeName("");
  TFile* inFile = new TFile(fileName);
  TIter nextkey(inFile->GetListOfKeys());
  while (TKey* key = (TKey*)nextkey() ){
    TObject* obj = key->ReadObj();
    if (obj->IsA()->InheritsFrom("TTree")){
      TTree* tree = (TTree*)obj;
      treeName = tree->GetName();
      cout << "FOUND TREE: " << treeName << endl;
    }
  }
  inFile->Close();
  delete inFile;
  if (treeName == ""){ cout << "no tree found in " << fileName << endl; exit(0); }
  return treeName;
}


TH1F* getTH1F(TString fileName, TString treeName, TString histName){
cout << "getTH1F:  " << histName << endl;
    // set up initial variables
  TString FN(fileName);
  TString NT(treeName);
  TString CAT("");
    // parse the histogram name
  vector<TString> parts = FSString::parseTString(histName,"_");
  if (parts[0] != "hist") { cout << "problem with histogram name" << endl; exit(0); }
  if (parts[1] != "FS")   { cout << "problem with histogram name" << endl; exit(0); }
  int modeCode3 = FSString::TString2int(parts[2]);
  int modeCode2 = FSString::TString2int(parts[3]);
  int modeCode1 = FSString::TString2int(parts[4]);
  if (parts[5] != "CUTS") { cout << "problem with histogram name" << endl; exit(0); }
  TString cutCode = parts[6];
  TString histType = parts[7];
  TString fsCode = getFSCode(modeCode1,modeCode2,modeCode3);
  FSModeCollection::clear();  FSModeCollection::addModeInfo(fsCode);
  TString title("#gamma p^{+} #rightarrow "); 
  title += FSString::rootSymbols(getDescription(modeCode1,modeCode2,modeCode3));
    // get the cuts
  TString CUTS = getSubModeCuts(modeCode3)+"&&"+
                 getOtherCuts(modeCode1,modeCode2,modeCode3,cutCode,histType);
    // MASS plots
  if (histType == "MASS"){
    int massModeCode3 = FSString::TString2int(parts[8]);
    int massModeCode2 = FSString::TString2int(parts[9]);
    int massModeCode1 = FSString::TString2int(parts[10]);
    TString VAR = "MASS("+getComboFormat(massModeCode1,massModeCode2,massModeCode3,modeCode3)+")";
    TString BOUNDS("(350,0.0,3.5)");
    TH1F* hist = FSModeHistogram::getTH1F(FN,NT,CAT,VAR,BOUNDS,CUTS);
    hist->SetTitle(title);
    hist->SetXTitle("Mass(" +
            FSString::rootSymbols(getDescription(massModeCode1,massModeCode2,massModeCode3))
                            + ")   [GeV]");
    hist->SetYTitle("Entries / 10 MeV");
    return hist;
  }
    // T distribution
  if (histType == "T"){
    int modeCode1X = modeCode1; int modeCode2X = modeCode2; int modeCode3X = modeCode3;
    removeRecoilBaryon(modeCode1X,modeCode2X,modeCode3X);
    TString VAR = "-1*MASS2("+getComboFormat(modeCode1X,modeCode2X,modeCode3X,modeCode3)+";GLUEXBEAM)";
    TString BOUNDS("(100,0.0,4.0)");
    TH1F* hist = FSModeHistogram::getTH1F(FN,NT,CAT,VAR,BOUNDS,CUTS);
    hist->SetTitle(title);
    hist->SetXTitle("-t  [GeV^{2}]");
    hist->SetYTitle("Entries / Bin");
    return hist;
  }
    // CHI2DOF distribution
  if (histType == "CHI2DOF"){
    TString VAR = "Chi2DOF";
    TString BOUNDS("(100,0.0,25.0)");
    TH1F* hist = FSModeHistogram::getTH1F(FN,NT,CAT,VAR,BOUNDS,CUTS);
    hist->SetTitle(title);
    hist->SetXTitle("#chi^{2}/dof");
    hist->SetYTitle("Entries / Bin");
    return hist;
  }
    // RFTIME distribution
  if (histType == "RFTIME"){
    TString VAR = "RFDeltaT";
    TString BOUNDS("(400,-20.0,20.0)");
    TH1F* hist = FSModeHistogram::getTH1F(FN,NT,CAT,VAR,BOUNDS,CUTS);
    hist->SetTitle(title);
    hist->SetXTitle("RF #DeltaT   [ns]");
    hist->SetYTitle("Entries / Bin");
    return hist;
  }
  return NULL;
}



void writeHistograms(TString treeFileName, TString histFileName){
  setModeCode3Particles();
  TString treeName = getTreeNameFromFile(treeFileName);
  TFile outFile(FSString::TString2string(histFileName).c_str(), "recreate"); outFile.cd();
  vector<TString> histList = getHistogramList(treeName);
  for (unsigned int i = 0; i < histList.size(); i++){
    TH1F* hist = getTH1F(treeFileName,treeName,histList[i]);
    outFile.cd();
    hist->SetName(histList[i]);
    hist->Write();
  }
  outFile.Close();
}


TString readHistograms(TString histFileName){
  setModeCode3Particles();
  vector<TString> histNames;
  TFile* inFile = new TFile(histFileName);
  TIter nextkey(inFile->GetListOfKeys());
  while (TKey* key = (TKey*)nextkey() ){
    TObject* obj = key->ReadObj();
    if (obj->IsA()->InheritsFrom("TH1")){
      TH1F* hist = (TH1F*)obj;
      TString histName = hist->GetName();
      cout << histName << endl;
      histNames.push_back(histName);
    }
  }
  inFile->Close();
  delete inFile;
    // extract fsCode from file
  if (histNames.size() == 0) { cout << "no histograms found" << endl; exit(0); }
  vector<TString> parts = FSString::parseTString(histNames[0],"_");
  if (parts.size() < 8)   { cout << "problem with histogram name" << endl; exit(0); }
  if (parts[0] != "hist") { cout << "problem with histogram name" << endl; exit(0); }
  if (parts[1] != "FS")   { cout << "problem with histogram name" << endl; exit(0); }
  int modeCode3 = FSString::TString2int(parts[2]);
  int modeCode2 = FSString::TString2int(parts[3]);
  int modeCode1 = FSString::TString2int(parts[4]);
  TString fsCode = getFSCode(modeCode1, modeCode2, modeCode3);
    // check all the histogram names
  vector<TString> histList = getHistogramList(fsCode);
  if (histNames.size() != histList.size()){ cout << "wrong number of histograms" << endl; exit(0); }
  for (unsigned int i = 0; i < histList.size(); i++){
    bool found = false;
    for (unsigned int j = 0; j < histNames.size(); j++){
      if (histList[i] == histNames[j]){ found = true; continue; }
    }
    if (!found){ cout << "histogram mismatch" << endl; exit(0); }
  }
  return fsCode;
}



pair<TString,TString> makeFigure(TString histFileName, TString subMode, TString histType, TString outputFigures){
  TString histIndexFS = "hist_FS_" + subMode;
  TH1F* histAllTAllChi2AllRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_000_"+histType);
  TH1F* histAllTAllChi2SigRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_001_"+histType);
  TH1F* histAllTAllChi2OutRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_002_"+histType);
  TH1F* histAllTSigChi2AllRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_010_"+histType);
  TH1F* histAllTSigChi2SigRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_011_"+histType);
  TH1F* histAllTSigChi2OutRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_012_"+histType);
  TH1F* histAllTOutChi2AllRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_020_"+histType);
  TH1F* histAllTOutChi2SigRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_021_"+histType);
  TH1F* histAllTOutChi2OutRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_022_"+histType);
  TH1F* histLowTAllChi2AllRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_100_"+histType);
  TH1F* histLowTAllChi2SigRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_101_"+histType);
  TH1F* histLowTAllChi2OutRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_102_"+histType);
  TH1F* histLowTSigChi2AllRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_110_"+histType);
  TH1F* histLowTSigChi2SigRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_111_"+histType);
  TH1F* histLowTSigChi2OutRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_112_"+histType);
  TH1F* histLowTOutChi2AllRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_120_"+histType);
  TH1F* histLowTOutChi2SigRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_121_"+histType);
  TH1F* histLowTOutChi2OutRF = FSHistogram::getTH1F(histFileName,histIndexFS+"_CUTS_122_"+histType);
  TH1F* histAllTAllChi2SubRF = new TH1F(*histAllTAllChi2SigRF); histAllTAllChi2SubRF->Add(histAllTAllChi2OutRF,-1.0);
  TH1F* histAllTSigChi2SubRF = new TH1F(*histAllTSigChi2SigRF); histAllTSigChi2SubRF->Add(histAllTSigChi2OutRF,-1.0);
  TH1F* histAllTOutChi2SubRF = new TH1F(*histAllTOutChi2SigRF); histAllTOutChi2SubRF->Add(histAllTOutChi2OutRF,-1.0);
  TH1F* histLowTAllChi2SubRF = new TH1F(*histLowTAllChi2SigRF); histLowTAllChi2SubRF->Add(histLowTAllChi2OutRF,-1.0);
  TH1F* histLowTSigChi2SubRF = new TH1F(*histLowTSigChi2SigRF); histLowTSigChi2SubRF->Add(histLowTSigChi2OutRF,-1.0);
  TH1F* histLowTOutChi2SubRF = new TH1F(*histLowTOutChi2SigRF); histLowTOutChi2SubRF->Add(histLowTOutChi2OutRF,-1.0);
  TString caption("");
  TCanvas* c1;
  if (histType == "RFTIME"){
    c1 = new TCanvas("c1","c1",1000,1000);
    c1->Divide(1,2);
    c1->cd(1);
    FSHistogram::setHistogramMaxima(histLowTSigChi2AllRF,histLowTOutChi2AllRF);
    histLowTOutChi2AllRF->SetLineColor(kAzure-4);
    histLowTOutChi2AllRF->SetFillStyle(3002);
    histLowTOutChi2AllRF->SetFillColor(kAzure-4);
    histLowTSigChi2AllRF->Draw("");
    histLowTOutChi2AllRF->Draw("hist,same");
    histLowTSigChi2AllRF->Draw("same");
    c1->cd(2);
    FSHistogram::setHistogramMaxima(histAllTSigChi2AllRF,histAllTOutChi2AllRF);
    histAllTOutChi2AllRF->SetLineColor(kAzure-4);
    histAllTOutChi2AllRF->SetFillStyle(3002);
    histAllTOutChi2AllRF->SetFillColor(kAzure-4);
    histAllTSigChi2AllRF->Draw("");
    histAllTOutChi2AllRF->Draw("hist,same");
    histAllTSigChi2AllRF->Draw("same");
    caption = "Distributions of RF $\\Delta t$ "
              "for the $\\chi^2/\\mathrm{dof}<5$ signal region~(points) "
              "and the $10<\\chi^2/\\mathrm{dof}<15$ sideband region~(shaded). "
              "TOP is low $|t|$~($|t|<0.5$~GeV$^{2}$) "
                   "and high $E_{\\mathrm{beam}}$~($E_{\\mathrm{beam}}>8$~GeV); "
              "BOTTOM is all $|t|$ and all $E_{\\mathrm{beam}}$.";
  }
  if (histType == "CHI2DOF"){
    c1 = new TCanvas("c1","c1",1000,1000);
    c1->Divide(2,2);
    c1->cd(1);
    FSHistogram::setHistogramMaxima(histLowTAllChi2SigRF,histLowTAllChi2OutRF);
    histLowTAllChi2OutRF->SetLineColor(kRed-3);
    histLowTAllChi2OutRF->SetFillStyle(3002);
    histLowTAllChi2OutRF->SetFillColor(kRed-3);
    histLowTAllChi2SigRF->Draw("");
    histLowTAllChi2OutRF->Draw("hist,same");
    histLowTAllChi2SigRF->Draw("same");
    c1->cd(2);
    histLowTAllChi2SubRF->Draw("");
    c1->cd(3);
    FSHistogram::setHistogramMaxima(histAllTAllChi2SigRF,histAllTAllChi2OutRF);
    histAllTAllChi2OutRF->SetLineColor(kRed-3);
    histAllTAllChi2OutRF->SetFillStyle(3002);
    histAllTAllChi2OutRF->SetFillColor(kRed-3);
    histAllTAllChi2SigRF->Draw("");
    histAllTAllChi2OutRF->Draw("hist,same");
    histAllTAllChi2SigRF->Draw("same");
    c1->cd(4);
    histAllTAllChi2SubRF->Draw("");
    caption = "Distributions of $\\chi^2$/dof.  "
              "TOP is low $|t|$~($|t|<0.5$~GeV$^{2}$) "
                   "and high $E_{\\mathrm{beam}}$~($E_{\\mathrm{beam}}>8$~GeV); "
              "BOTTOM is all $|t|$ and all $E_{\\mathrm{beam}}$. "
              "LEFT is in time (points) and out of time (shaded); "
              "RIGHT is time subtracted.";
  }
  if (histType == "T"){
    c1 = new TCanvas("c1","c1",1000,1000);
    c1->Divide(2,2);
    c1->cd(1);
    FSHistogram::setHistogramMaxima(histLowTSigChi2SigRF,histLowTSigChi2OutRF);
    histLowTSigChi2OutRF->SetLineColor(kRed-3);
    histLowTSigChi2OutRF->SetFillStyle(3002);
    histLowTSigChi2OutRF->SetFillColor(kRed-3);
    histLowTSigChi2SigRF->Draw("");
    histLowTSigChi2OutRF->Draw("hist,same");
    histLowTSigChi2SigRF->Draw("same");
    c1->cd(2);
    FSHistogram::setHistogramMaxima(histLowTSigChi2SubRF,histLowTOutChi2SubRF);
    FSHistogram::setHistogramMinima(histLowTSigChi2SubRF,histLowTOutChi2SubRF);
    histLowTOutChi2SubRF->SetLineColor(kAzure-4);
    histLowTOutChi2SubRF->SetFillStyle(3002);
    histLowTOutChi2SubRF->SetFillColor(kAzure-4);
    histLowTSigChi2SubRF->Draw("");
    histLowTOutChi2SubRF->Draw("hist,same");
    histLowTSigChi2SubRF->Draw("same");
    c1->cd(3);
    FSHistogram::setHistogramMaxima(histAllTSigChi2SigRF,histAllTSigChi2OutRF);
    histAllTSigChi2OutRF->SetLineColor(kRed-3);
    histAllTSigChi2OutRF->SetFillStyle(3002);
    histAllTSigChi2OutRF->SetFillColor(kRed-3);
    histAllTSigChi2SigRF->Draw("");
    histAllTSigChi2OutRF->Draw("hist,same");
    histAllTSigChi2SigRF->Draw("same");
    c1->cd(4);
    FSHistogram::setHistogramMaxima(histAllTSigChi2SubRF,histAllTOutChi2SubRF);
    FSHistogram::setHistogramMinima(histAllTSigChi2SubRF,histAllTOutChi2SubRF);
    histAllTOutChi2SubRF->SetLineColor(kAzure-4);
    histAllTOutChi2SubRF->SetFillStyle(3002);
    histAllTOutChi2SubRF->SetFillColor(kAzure-4);
    histAllTSigChi2SubRF->Draw("");
    histAllTOutChi2SubRF->Draw("hist,same");
    histAllTSigChi2SubRF->Draw("same");
    caption = "Distributions of $-t$. "
              "TOP is high $E_{\\mathrm{beam}}$~($E_{\\mathrm{beam}}>8$~GeV); "
              "BOTTOM is all $E_{\\mathrm{beam}}$. "
              "LEFT is in time (points) and out of time (shaded); "
              "RIGHT is time subtracted "
              "for the $\\chi^2/\\mathrm{dof}<5$ signal region~(points) "
              "and the $10<\\chi^2/\\mathrm{dof}<15$ sideband region~(shaded).";
  }
  if (histType.Contains("MASS")){
    c1 = new TCanvas("c1","c1",1000,1000);
    c1->Divide(2,2);
    c1->cd(1);
    FSHistogram::setHistogramMaxima(histLowTSigChi2SigRF,histLowTSigChi2OutRF);
    histLowTSigChi2OutRF->SetLineColor(kRed-3);
    histLowTSigChi2OutRF->SetFillStyle(3002);
    histLowTSigChi2OutRF->SetFillColor(kRed-3);
    histLowTSigChi2SigRF->Draw("");
    histLowTSigChi2OutRF->Draw("hist,same");
    histLowTSigChi2SigRF->Draw("same");
    c1->cd(2);
    FSHistogram::setHistogramMaxima(histLowTSigChi2SubRF,histLowTOutChi2SubRF);
    FSHistogram::setHistogramMinima(histLowTSigChi2SubRF,histLowTOutChi2SubRF);
    histLowTOutChi2SubRF->SetLineColor(kAzure-4);
    histLowTOutChi2SubRF->SetFillStyle(3002);
    histLowTOutChi2SubRF->SetFillColor(kAzure-4);
    histLowTSigChi2SubRF->Draw("");
    histLowTOutChi2SubRF->Draw("hist,same");
    histLowTSigChi2SubRF->Draw("same");
    c1->cd(3);
    FSHistogram::setHistogramMaxima(histAllTSigChi2SigRF,histAllTSigChi2OutRF);
    histAllTSigChi2OutRF->SetLineColor(kRed-3);
    histAllTSigChi2OutRF->SetFillStyle(3002);
    histAllTSigChi2OutRF->SetFillColor(kRed-3);
    histAllTSigChi2SigRF->Draw("");
    histAllTSigChi2OutRF->Draw("hist,same");
    histAllTSigChi2SigRF->Draw("same");
    c1->cd(4);
    FSHistogram::setHistogramMaxima(histAllTSigChi2SubRF,histAllTOutChi2SubRF);
    FSHistogram::setHistogramMinima(histAllTSigChi2SubRF,histAllTOutChi2SubRF);
    histAllTOutChi2SubRF->SetLineColor(kAzure-4);
    histAllTOutChi2SubRF->SetFillStyle(3002);
    histAllTOutChi2SubRF->SetFillColor(kAzure-4);
    histAllTSigChi2SubRF->Draw("");
    histAllTOutChi2SubRF->Draw("hist,same");
    histAllTSigChi2SubRF->Draw("same");
    caption = "Mass distributions.  "
              "TOP is low $|t|$~($|t|<0.5$~GeV$^{2}$) "
                   "and high $E_{\\mathrm{beam}}$~($E_{\\mathrm{beam}}>8$~GeV); "
              "BOTTOM is all $|t|$ and all $E_{\\mathrm{beam}}$. "
              "LEFT is in time (points) and out of time (shaded); "
              "RIGHT is time subtracted "
              "for the $\\chi^2/\\mathrm{dof}<5$ signal region~(points) "
              "and the $10<\\chi^2/\\mathrm{dof}<15$ sideband region~(shaded).";
  }
  c1->cd();
  TString pdfFileName = outputFigures+"/"+histIndexFS+"_"+histType+".pdf";
  c1->Print(pdfFileName);
  return pair<TString,TString>(pdfFileName,caption);
}


void makePDF(TString histFileName, TString outputDirectory, TString baseName){
  outputDirectory = FSSystem::getAbsolutePath(outputDirectory,false);
  if (outputDirectory == ""){ cout << "problem with output directory" << endl; exit(0); }
  vector<TString> nameParts = FSString::parseTString(histFileName,".",true);
  TString outputName = baseName;
  TString fsCode = readHistograms(histFileName);
  FSModeInfo miFS(fsCode);
  TString fsDescription(getDescription(miFS.modeCode1(),miFS.modeCode2(),0));
  outputDirectory = outputDirectory + "/" + outputName;
  if (FSSystem::getAbsolutePath(outputDirectory,false) != "")
    { cout << "directory already exists: " << outputDirectory << endl; 
      cout << "WARNING, replacing it!" << endl;  system("rm -rf "+outputDirectory);}
  cout << "Creating PDF directory:     " << outputDirectory << endl;
  system("mkdir "+outputDirectory);
  TString outputFigures = outputDirectory + "/figures";
  cout << "Creating figures directory: " << outputFigures << endl;
  system("mkdir "+outputFigures);
  TString latexFile = outputDirectory + "/"+outputName+".tex";
  cout << "Creating latex file:        " << latexFile << endl;
  FSString::latexHeader(latexFile);
  FSString::writeTStringToFile(latexFile,
    "\n\n"
    "\\title{\\vspace{-2cm}Histograms for $\\gamma p^{+} \\to "+
      FSString::root2latexSymbols(FSString::rootSymbols(fsDescription))+"$}\n"
    "\\author{}\n"
    "\\date{\\vspace{-1cm}\\today}\n"
    "\\maketitle\n\n"
    "\\tableofcontents\n\n");
  FSString::writeTStringToFile(latexFile,
    "\\newpage\n\n"
    "\\section{Notes on Selection Criteria}\n\n"
    " Cuts applied in addition to those applied during the analysis launch:\n"
    " \\begin{itemize}\n"
    "  \\item Cuts on $\\chi^2$/DOF, $t$, RF timing, and beam energy are described\n"
    "           in the captions under the figures. "
    "  \\item The number of unused tracks is $\\le 1$.\n"
    "  \\item The number of unused neutrals is $\\le 2$.\n"
    "  \\item All photons have a shower quality $>0.5$.\n"
    "  \\item Cuts on all intermediate state masses are 50~MeV wide,\n"
    "  including $\\pi^0$, $\\eta$, $K_{S}$, and $\\Lambda$ masses (when not constrained).\n"
    // "  \\item Photon combinations not from a $\\pi^0$ are vetoed if they land in a 50~MeV wide window\n"
    // "    around the $\\pi^0$ mass.\n" 
    " \\end{itemize}\n");
  vector<TString> subModes = getSubModes(fsCode);
  for (unsigned int i = 0; i < subModes.size(); i++){
    pair<TString,TString> histInfo;
    cout << "Making plots for submode: " << getDescription(subModes[i]) << endl;
    FSString::writeTStringToFile(latexFile,
      "\\newpage\n\n"
      "\\section{Histograms for $"+
      FSString::root2latexSymbols(FSString::rootSymbols(getDescription(subModes[i])))+"$}\n");

    histInfo = makeFigure(histFileName,subModes[i],"RFTIME",outputFigures);
    FSString::writeTStringToFile(latexFile,
      "\\subsection{RF $\\Delta t$}\n\n"
      "\\begin{figure}[h!]\n"
      "\\begin{center}\n"
      "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
      "\\end{center}\n"
      "\\caption{"+histInfo.second+"}\n"
      "\\end{figure}\n\n");

    histInfo = makeFigure(histFileName,subModes[i],"CHI2DOF",outputFigures);
    FSString::writeTStringToFile(latexFile,
      "\\newpage\n\n"
      "\\subsection{$\\chi^{2}$/dof}\n\n"
      "\\begin{figure}[h!]\n"
      "\\begin{center}\n"
      "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
      "\\end{center}\n"
      "\\caption{"+histInfo.second+"}\n"
      "\\end{figure}\n\n");

    histInfo = makeFigure(histFileName,subModes[i],"T",outputFigures);
    FSString::writeTStringToFile(latexFile,
      "\\newpage\n\n"
      "\\subsection{$-t$}\n\n"
      "\\begin{figure}[h!]\n"
      "\\begin{center}\n"
      "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
      "\\end{center}\n"
      "\\caption{"+histInfo.second+"}\n"
      "\\end{figure}\n\n");

    vector<TString> massCombinations = getMassCombinations(subModes[i]);
    for (unsigned int j = 0; j < massCombinations.size(); j++){
      histInfo = makeFigure(histFileName,subModes[i],"MASS_"+massCombinations[j],outputFigures);
      TString title = "Mass($"+
        FSString::root2latexSymbols(FSString::rootSymbols(getDescription(massCombinations[j])))
        +"$)";
      FSString::writeTStringToFile(latexFile,
        "\\newpage\n\n"
        "\\subsection{"+title+"}\n\n"
        "\\begin{figure}[h!]\n"
        "\\begin{center}\n"
        "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
        "\\end{center}\n"
        "\\caption{"+histInfo.second+"}\n"
        "\\end{figure}\n\n");
    }

  }
  FSString::latexCloser(latexFile);
  system("pdflatex -output-directory "+outputDirectory+" "+latexFile);
  system("pdflatex -output-directory "+outputDirectory+" "+latexFile);
  system("pdflatex -output-directory "+outputDirectory+" "+latexFile);
}




vector< vector<int> > pushBackToEach(vector< vector<int> > originalList, vector<int> newPart){
  vector< vector<int> > newList;
  for (unsigned int i = 0; i < originalList.size() || ((originalList.size()==0)&&(i==0)); i++){
  for (unsigned int j = 0; j < newPart.size(); j++){
    vector<int> newComponent;
    if (originalList.size() != 0) newComponent = originalList[i];
    newComponent.push_back(newPart[j]);
    newList.push_back(newComponent);
  }}
  return newList;
}


vector<TString> expandIntegers(TString input){
    // get all the integers from input
  vector<int> intInput;
  for (unsigned int i = 0; i < input.Length(); i++){
    TString digit(input[i]);
    if (digit.IsDigit()) intInput.push_back(FSString::TString2int(digit));
  }
    // make all the combinations
  vector< vector<int> > expandedIntInputs;
  for (unsigned int i = 0; i < intInput.size(); i++){
    vector<int> combo;
    for (unsigned int j = 0; j <= intInput[i]; j++){ combo.push_back(j); }
    expandedIntInputs = pushBackToEach(expandedIntInputs,combo);
  }
    // put these back into string format
  vector<TString> expandedInput;
  for (unsigned int i = 0; i < expandedIntInputs.size(); i++){
    TString newCode = "";  int idigit = 0;
    for (unsigned int j = 0; j < input.Length(); j++){
      TString digit(input[j]);
      if (!digit.IsDigit()){ newCode = newCode + digit; }
      else { newCode += expandedIntInputs[i][idigit++]; }
    }
    expandedInput.push_back(newCode);
  }
  return expandedInput;
}

