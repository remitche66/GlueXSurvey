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

vector<int> getModeCode3Numbers(int modeCode3){
  setModeCode3Particles();
  vector<int> numbers;
  int n1 = 1; int n2 = 10;
  for (unsigned int iP = 0; iP < modeCode3Particles.size(); iP++){
    int n = (((modeCode3%n2)-(modeCode3%n1))/n1); n1*=10; n2*=10;
    numbers.push_back(n);
  }
  return numbers;
}

FSModeInfo addModeCode3(FSModeInfo mi, int modeCode3, int sign){
  vector<int> numbers = getModeCode3Numbers(modeCode3);
  int modeCode1 = mi.modeCode1(); 
  int modeCode2 = mi.modeCode2(); 
  for (unsigned int iP = 0; iP < modeCode3Particles.size(); iP++){
    int n = numbers[iP];
    FSModeInfo mi1(modeCode1,modeCode2);
    modeCode1 += sign*n*modeCode3Particles[iP].decay->modeCode1();
    modeCode2 += sign*n*modeCode3Particles[iP].decay->modeCode2();
    FSModeInfo mi2(modeCode1,modeCode2);
    if (mi1.modeNParticles() + sign*n*modeCode3Particles[iP].decay->modeNParticles()
         != mi2.modeNParticles()) return FSModeInfo(0,0);
  }
  return FSModeInfo(modeCode1,modeCode2);
}

FSModeInfo getRecoilBaryon(FSModeInfo mi){
  FSModeInfo miP("p+");      if (mi.modeContains(miP.modeString())) return miP;
  FSModeInfo miL("Lambda");  if (mi.modeContains(miL.modeString())) return miL;
  cout << "no recoil proton or Lambda? quitting " << endl; exit(0);
  return FSModeInfo("");
}

FSModeInfo removeRecoilBaryon(FSModeInfo mi){
  FSModeInfo miB = getRecoilBaryon(mi);
  return FSModeInfo(mi.modeCode1()-miB.modeCode1(), mi.modeCode2()-miB.modeCode2());
}

vector<TString> getParticles(FSModeInfo mi, int modeCode3){
  vector<int> numbers = getModeCode3Numbers(modeCode3);
  vector<TString> particles = mi.particles();
  for (unsigned int iP = 0; iP < modeCode3Particles.size(); iP++){
    int n = numbers[iP];
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

TString getDescription(FSModeInfo mi, int modeCode3){
  vector<TString> particles = getParticles(mi,modeCode3);
  TString description("");
  for (unsigned int i = 0; i < particles.size(); i++){
    description += particles[i];
    if (i != particles.size()-1) description += " ";
  }
  return description;
}


TString getComboFormat(FSModeInfo mi, int modeCode3, int offset){
  vector<int> numbers = getModeCode3Numbers(modeCode3);
  TString comboFormat = mi.modeComboFormat(1);
  for (unsigned int iP = 0; iP < modeCode3Particles.size(); iP++){
    int n = numbers[iP];
    for (unsigned int j = 0; j < n; j++){
      if (comboFormat != "") comboFormat += ",";
      TString name = modeCode3Particles[iP].name; name += FSString::int2TString(j+1+offset);
      comboFormat += modeCode3Particles[iP].decay->modeComboFormat(1,name);
    }
  }
  return comboFormat;
}


TString getSubModeCuts(int modeCode3){
  vector<int> numbers = getModeCode3Numbers(modeCode3);
  TString cuts("");
  for (unsigned int iP = 0; iP < modeCode3Particles.size(); iP++){
    int modeCode3iP = pow(10,iP);
    int n = numbers[iP];
    for (unsigned int j = 0; j < n; j++){
      if (cuts != "") cuts += "&&";
      TString comboFormat = getComboFormat(FSModeInfo(0,0),modeCode3iP,j);
      cuts += "(abs(MASS("
               + comboFormat + ")-"
               + modeCode3Particles[iP].mass + ")<"
               + modeCode3Particles[iP].cut + ")";
    }
  }
  if (cuts == "") cuts = "(1==1)";
  return cuts;
}


TString getOtherCuts(FSModeInfo mi, int modeCode3, TString cutCode, TString histType){
  FSModeInfo miX = removeRecoilBaryon(mi);
  TString listX(getComboFormat(miX,modeCode3));
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


vector<int> getModeCode3List(FSModeInfo mi){
  vector<int> modeCode3List; modeCode3List.push_back(0);
  int MAXNCODE3 = 4;
  int MAXNCODE3SAME = 3;
  TString sMAX3 = FSString::int2TString(MAXNCODE3SAME);
  TString sMax(""); for (unsigned int i = 0; i < modeCode3Particles.size(); i++){ sMax += sMAX3; }
  vector<TString> smodeCode3List = expandIntegers(sMax);
  for (unsigned int i = 0; i < smodeCode3List.size(); i++){
    int modeCode3 = FSString::TString2int(smodeCode3List[i]);
    if (modeCode3 == 0) continue;
    if (getParticles(FSModeInfo(0,0),modeCode3).size() > MAXNCODE3) continue;
    FSModeInfo mi3 = addModeCode3(FSModeInfo(0,0),modeCode3);
    if (mi3.modeNParticles() == 0) continue;
    if (!mi.modeContains(mi3.modeString())) continue;
    modeCode3List.push_back(modeCode3);
  }
//cout << "MODECODE3 LIST:" << endl;
//for (unsigned int i = 0; i < modeCode3List.size(); i++){ 
//cout << modeCode3List[i] << " " << addModeCode3(mi,modeCode3List[i],-1).modeString() << endl; }
  return modeCode3List;
}

vector<TString> getMassCombinations(FSModeInfo mi, int modeCode3){
  TString subMode = FSString::int2TString(modeCode3) + "_" + mi.modeString();
  vector<TString> massCombosAll = expandIntegers(subMode);
  if (massCombosAll.size() == 0) return massCombosAll;
  vector<TString> massCombos;
  for (unsigned int i = 0; i < massCombosAll.size()-1; i++){
    vector<TString> modeCodes = FSString::parseTString(massCombosAll[i],"_");
    int modeCode1 = FSString::TString2int(modeCodes[2]); 
    int modeCode2 = FSString::TString2int(modeCodes[1]);
    int modeCode3 = FSString::TString2int(modeCodes[0]); 
    FSModeInfo mi(modeCode1,modeCode2); 
    int n = getParticles(mi,modeCode3).size();
    if (n < 2) continue;
    if (modeCode1 + modeCode2 > 0){
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



vector<TString> getHistogramList(FSModeInfo mi){
  vector<TString> histogramList;
  vector<int> modeCode3s = getModeCode3List(mi);
  vector<TString> cutCombos = expandIntegers("122");
  for (unsigned int iS = 0; iS < modeCode3s.size(); iS++){
    int modeCode3 = modeCode3s[iS];
    FSModeInfo mi12 = addModeCode3(mi,modeCode3,-1);
    TString histIndexFS = "hist_FS_" + FSString::int2TString(modeCode3) 
                               + "_" + mi12.modeString();
    vector<TString> massCombos = getMassCombinations(mi12,modeCode3);
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
  TString fsCode = addModeCode3(FSModeInfo(modeCode1,modeCode2),modeCode3).modeString();
  FSModeCollection::clear();  FSModeCollection::addModeInfo(fsCode);
  TString title("#gamma p^{+} #rightarrow "); 
  title += FSString::rootSymbols(getDescription(FSModeInfo(modeCode1,modeCode2),modeCode3));
    // get the cuts
  TString CUTS = getSubModeCuts(modeCode3)+"&&"+
                 getOtherCuts(FSModeInfo(modeCode1,modeCode2),modeCode3,cutCode,histType);
    // MASS plots
  if (histType == "MASS"){
    int massModeCode3 = FSString::TString2int(parts[8]);
    int massModeCode2 = FSString::TString2int(parts[9]);
    int massModeCode1 = FSString::TString2int(parts[10]);
    TString VAR = "MASS("+getComboFormat(FSModeInfo(massModeCode1,massModeCode2),massModeCode3)+")";
    TString BOUNDS("(350,0.0,3.5)");
    TH1F* hist = FSModeHistogram::getTH1F(FN,NT,CAT,VAR,BOUNDS,CUTS);
    hist->SetTitle(title);
    hist->SetXTitle("Mass(" +
            FSString::rootSymbols(getDescription(FSModeInfo(massModeCode1,massModeCode2),massModeCode3))
                            + ")   [GeV]");
    hist->SetYTitle("Entries / 10 MeV");
    return hist;
  }
    // T distribution
  if (histType == "T"){
    FSModeInfo miX = removeRecoilBaryon(FSModeInfo(modeCode1,modeCode2));
    TString VAR = "-1*MASS2("+getComboFormat(miX,modeCode3)+";GLUEXBEAM)";
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


void setMCComponentsFromTreeFile(TString treeFileName, TString treeName, FSModeInfo miFS){
  vector<int> modeCode3List = getModeCode3List(miFS);
  for (unsigned int i = 0; i < modeCode3List.size(); i++){
    int modeCode3 = modeCode3List[i];
    FSModeInfo mi = addModeCode3(miFS,modeCode3,-1);
    TString subMode = FSString::int2TString(modeCode3)+"_"+mi.modeString();
    TString cutCode = "11";
    TString FN(treeFileName);
    TString NT(treeName);
    TString CAT("");
    TString VAR("1.234");
    TString BOUNDS("(10,0.0,2.0)");
    TString CUTS = getSubModeCuts(modeCode3)+"&&"+
                   getOtherCuts(mi,modeCode3,cutCode,"");
    FSModeCollection::clear();  FSModeCollection::addModeInfo(miFS.modeString());
    vector< pair<TString, float> > components = 
        FSModeHistogram::getMCComponentsAndSizes(FN,NT,CAT,VAR,BOUNDS,CUTS);
  }
}

void setMCComponentsFromHistFile(TString histFileName, FSModeInfo miFS){
  vector<int> modeCode3List = getModeCode3List(miFS);
  for (unsigned int i = 0; i < modeCode3List.size(); i++){
    int modeCode3 = modeCode3List[i];
    FSModeInfo mi = addModeCode3(miFS,modeCode3,-1);
    TString subMode = FSString::int2TString(modeCode3)+"_"+mi.modeString();
  }
}

void writeHistograms(TString treeFileName, TString histFileName){
  setModeCode3Particles();
  TString treeName = FSTree::getTreeNameFromFile(treeFileName);
  TFile outFile(FSString::TString2string(histFileName).c_str(), "recreate"); outFile.cd();
  vector<TString> histList = getHistogramList(FSModeInfo(treeName));
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
  FSModeInfo mi = addModeCode3(FSModeInfo(modeCode1, modeCode2), modeCode3);
    // check all the histogram names
  vector<TString> histList = getHistogramList(mi);
  if (histNames.size() != histList.size()){ cout << "wrong number of histograms" << endl; exit(0); }
  for (unsigned int i = 0; i < histList.size(); i++){
    bool found = false;
    for (unsigned int j = 0; j < histNames.size(); j++){
      if (histList[i] == histNames[j]){ found = true; continue; }
    }
    if (!found){ cout << "histogram mismatch" << endl; exit(0); }
  }
  return mi.modeString();
}


pair<TString,TString> makeFigure(TString histFileName, TString subMode, TString histType, TString outputFigures){
  TString histIndexFS = "hist_FS_" + subMode;
  cout << "******************************************************" << endl;
  cout << "FIGURES FOR " << histIndexFS << endl;
  cout << "histType = " << histType << endl;
  cout << "subMode = " << subMode << endl;
  cout << "******************************************************" << endl;
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
  TString fsDescription(getDescription(miFS,0));
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
  vector<int> modeCode3List = getModeCode3List(miFS);
  for (unsigned int i = 0; i < modeCode3List.size(); i++){
    FSModeInfo mi = addModeCode3(miFS,modeCode3List[i],-1);
    TString subMode = FSString::int2TString(modeCode3List[i])+"_"+mi.modeString();
    pair<TString,TString> histInfo;
    cout << "Making plots for submode: " << getDescription(mi,modeCode3List[i]) << endl;
    FSString::writeTStringToFile(latexFile,
      "\\newpage\n\n"
      "\\section{Histograms for $"+
      FSString::root2latexSymbols(FSString::rootSymbols(getDescription(mi,modeCode3List[i])))+"$}\n");

    histInfo = makeFigure(histFileName,subMode,"RFTIME",outputFigures);
    FSString::writeTStringToFile(latexFile,
      "\\subsection{RF $\\Delta t$}\n\n"
      "\\begin{figure}[h!]\n"
      "\\begin{center}\n"
      "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
      "\\end{center}\n"
      "\\caption{"+histInfo.second+"}\n"
      "\\end{figure}\n\n");

    histInfo = makeFigure(histFileName,subMode,"CHI2DOF",outputFigures);
    FSString::writeTStringToFile(latexFile,
      "\\newpage\n\n"
      "\\subsection{$\\chi^{2}$/dof}\n\n"
      "\\begin{figure}[h!]\n"
      "\\begin{center}\n"
      "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
      "\\end{center}\n"
      "\\caption{"+histInfo.second+"}\n"
      "\\end{figure}\n\n");

    histInfo = makeFigure(histFileName,subMode,"T",outputFigures);
    FSString::writeTStringToFile(latexFile,
      "\\newpage\n\n"
      "\\subsection{$-t$}\n\n"
      "\\begin{figure}[h!]\n"
      "\\begin{center}\n"
      "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
      "\\end{center}\n"
      "\\caption{"+histInfo.second+"}\n"
      "\\end{figure}\n\n");

    vector<TString> massCombinations = getMassCombinations(mi,modeCode3List[i]);
    for (unsigned int j = 0; j < massCombinations.size(); j++){
      vector<TString> modeCodes = FSString::parseTString(massCombinations[j],"_");
      int modeCode1 = FSString::TString2int(modeCodes[2]); 
      int modeCode2 = FSString::TString2int(modeCodes[1]);
      int modeCode3 = FSString::TString2int(modeCodes[0]); 
      histInfo = makeFigure(histFileName,subMode,"MASS_"+massCombinations[j],outputFigures);
      TString title = "Mass($"+
        FSString::root2latexSymbols(FSString::rootSymbols(getDescription(FSModeInfo(modeCode1,modeCode2),modeCode3)))
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

