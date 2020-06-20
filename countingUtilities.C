#include "countingUtilities.h"

// some conventions:
//  modeCode3 (integer) = code for extra decaying particles, e.g. phi, omega, eta+-0
//  subMode (TString) = modeCode3_modeCode2_modeCode1
//  mcComponent (TString) = MCExtras_MCDecayCode2_MCDecayCode1

  // global
vector<ModeCode3Particle> modeCode3Particles;
map<TString,int> particleOrder;

  // global map from submode to list of mc components
map< TString, vector<TString> > mcComponentsMap; 

  // global map from hist files to fs codes (to save time)
map< TString, TString > histFileNameToFSCodeMap;

// not important -- just for cleaning up memory
vector<TH1F*> memCache;
void clearMemCache(){ 
  for (unsigned int i = 0; i < memCache.size(); i++){
    if (memCache[i]) delete memCache[i]; 
  } memCache.clear(); 
}

void setModeCode3ParticlesEtc(){
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
  setModeCode3ParticlesEtc();
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

TString getMCCuts(TString mcComponent){
  TString mcCuts("");
  if ((mcComponent == "0_0_0") || (mcComponent == "")) return TString("(1==1)");
  vector<TString> parts = FSString::parseTString(mcComponent,"_");
  if (parts.size() != 3){ cout << "mc component problem, quitting" << endl; exit(0); }
  mcCuts +=   "((MCDecayCode1==";  mcCuts += parts[2];
  mcCuts += ")&&(MCDecayCode2==";  mcCuts += parts[1];
  mcCuts += ")&&(MCExtras==";      mcCuts += parts[0];  mcCuts += "))";
  return mcCuts;
}

vector<int> getModeCode3List(FSModeInfo mi, bool show){
  setModeCode3ParticlesEtc();
  vector<int> modeCode3List; modeCode3List.push_back(0);
  int MAXNCODE3 = 4;
  int MAXNCODE3SAME = 3;
  TString sMAX3 = FSString::int2TString(MAXNCODE3SAME);
  TString sMax(""); for (unsigned int i = 0; i < modeCode3Particles.size(); i++){ sMax += sMAX3; }
  vector<TString> smodeCode3List = FSString::expandIntegers(sMax);
  for (unsigned int i = 0; i < smodeCode3List.size(); i++){
    int modeCode3 = FSString::TString2int(smodeCode3List[i]);
    if (modeCode3 == 0) continue;
    if (getParticles(FSModeInfo(0,0),modeCode3).size() > MAXNCODE3) continue;
    FSModeInfo mi3 = addModeCode3(FSModeInfo(0,0),modeCode3);
    if (mi3.modeNParticles() == 0) continue;
    if (!mi.modeContains(mi3.modeString())) continue;
    modeCode3List.push_back(modeCode3);
  }
  if (show){
    cout << "MODECODE3 LIST:" << endl;
    for (unsigned int i = 0; i < modeCode3List.size(); i++){ 
    cout << modeCode3List[i] << " " << addModeCode3(mi,modeCode3List[i],-1).modeString() << endl; }
  }
  return modeCode3List;
}

vector<TString> getMassCombinations(FSModeInfo mi, int modeCode3, bool show){
  TString subMode = FSString::int2TString(modeCode3) + "_" + mi.modeString();
  vector<TString> massCombosAll = FSString::expandIntegers(subMode);
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
  if (show){
    cout << "massCombos:" << endl;
    for (unsigned int i = 0; i < massCombos.size(); i++){ cout << massCombos[i] << endl; }
  }
  return massCombos;
}



vector<TString> getHistogramList(FSModeInfo miFS, bool isMC, bool show){
  vector<TString> histogramList;
  vector<int> modeCode3List = getModeCode3List(miFS);
  vector<TString> cutCombos = FSString::expandIntegers("122");
  for (unsigned int i3 = 0; i3 < modeCode3List.size(); i3++){
    int modeCode3 = modeCode3List[i3];
    FSModeInfo mi = addModeCode3(miFS,modeCode3,-1);
    TString subMode = FSString::int2TString(modeCode3)+"_"+mi.modeString();
    vector<TString> mcComponents;
    mcComponents.push_back("0_0_0");
    if (isMC){
      if (mcComponentsMap.find(subMode) != mcComponentsMap.end()){
        vector<TString> tempComponents = mcComponentsMap[subMode];
        for (unsigned int itmp = 0; itmp < tempComponents.size(); itmp++){
          mcComponents.push_back(tempComponents[itmp]); }
      }
    }
    vector<TString> massCombos = getMassCombinations(mi,modeCode3);
    for (unsigned int iMC = 0; iMC < mcComponents.size(); iMC++){
      TString histIndexFS = "hist_FS_" + subMode;
      if (isMC) histIndexFS += ("_MC_" + mcComponents[iMC]);
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
  }
  if (show){
    cout << "histogramList:" << endl;
    for (unsigned int i = 0; i < histogramList.size(); i++){ cout << histogramList[i] << endl; }
  }
  return histogramList;
}



TH1F* getTH1FFromTreeFile(TString fileName, TString treeName, TString histName){
  bool isMC = histName.Contains("_MC_"); int iMC = 0; if (isMC) iMC = 4;
    // set up initial variables
  TString FN(fileName);
  TString NT(treeName);
  TString CAT("");
    // parse the histogram name
  vector<TString> parts = FSString::parseTString(histName,"_");
  if (parts.size() < 8+iMC){ cout << "problem with histogram name" << endl; exit(0); }
  if (parts[0] != "hist")  { cout << "problem with histogram name" << endl; exit(0); }
  if (parts[1] != "FS")    { cout << "problem with histogram name" << endl; exit(0); }
  int modeCode3 = FSString::TString2int(parts[2]);
  int modeCode2 = FSString::TString2int(parts[3]);
  int modeCode1 = FSString::TString2int(parts[4]);
  if (isMC && parts[5] != "MC")   { cout << "problem with histogram name" << endl; exit(0); }
  TString mcComponent("");
  if (isMC) mcComponent = parts[6] + "_" + parts[7] + "_" + parts[8];
  if (parts[5+iMC] != "CUTS") { cout << "problem with histogram name" << endl; exit(0); }
  TString cutCode = parts[6+iMC];
  TString histType = parts[7+iMC];
  TString fsCode = addModeCode3(FSModeInfo(modeCode1,modeCode2),modeCode3).modeString();
  FSModeCollection::clear();  FSModeCollection::addModeInfo(fsCode);
  TString title("#gamma p^{+} #rightarrow "); 
  title += FSString::rootSymbols(getDescription(FSModeInfo(modeCode1,modeCode2),modeCode3));
    // get the cuts
  TString CUTS = getSubModeCuts(modeCode3)+"&&"+
                 getOtherCuts(FSModeInfo(modeCode1,modeCode2),modeCode3,cutCode,histType)+"&&"+
                 getMCCuts(mcComponent);
    // MASS plots
  if (histType == "MASS"){
    if (parts.size() < 11+iMC)  { cout << "problem with histogram name" << endl; exit(0); }
    int massModeCode3 = FSString::TString2int(parts[8+iMC]);
    int massModeCode2 = FSString::TString2int(parts[9+iMC]);
    int massModeCode1 = FSString::TString2int(parts[10+iMC]);
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


TH1F* getTH1FFromHistFile(TString histFileName, TString histName){
  bool isMC = histName.Contains("_MC_"); int iMC = 0; if (isMC) iMC = 4;
  vector<TString> parts = FSString::parseTString(histName,"_");
  if (parts[5+iMC] != "CUTS") { cout << "problem with histogram name" << endl; exit(0); }
  TString cutCode = "CUTS_"+parts[6+iMC];
  if (cutCode.Length() != 8){ cout << "problem with histogram name" << endl; exit(0); }
  if (FSString::subString(cutCode,7,8) != "S"){
    TH1F* hist = (TH1F*)FSHistogram::getTH1F(histFileName,histName)->Clone();
    memCache.push_back(hist);
    return hist;
  }
  TString cutCodeSig(cutCode);  cutCodeSig.Replace(7,1,"1");
  TString cutCodeOut(cutCode);  cutCodeOut.Replace(7,1,"2");
  TString histNameSig(histName); histNameSig.Replace(histName.Index(cutCode),8,cutCodeSig);
  TString histNameOut(histName); histNameOut.Replace(histName.Index(cutCode),8,cutCodeOut);
  TH1F* hsig = (TH1F*)FSHistogram::getTH1F(histFileName,histNameSig)->Clone();
  TH1F* hout = (TH1F*)FSHistogram::getTH1F(histFileName,histNameOut)->Clone();
  TH1F* hsub = new TH1F(*hsig);  hsub->Add(hout,-1.0);  
  hsub->SetMinimum(-0.05*hsub->GetMaximum());
  memCache.push_back(hsig);
  memCache.push_back(hout);
  memCache.push_back(hsub);
  return hsub;
}

pair<THStack*,TLegend*> getTHStFromHistFile(TString histFileName, TString histName){
  bool isMC = histName.Contains("_MC_"); if (!isMC){ cout << "not MC" << endl; exit(0); }
  setModeCode3ParticlesEtc();
  setMCComponentsFromHistFile(histFileName);
  vector<TString> keys; keys.push_back("_FS_"); keys.push_back("_MC_"); keys.push_back("_CUTS_");
  map<TString,TString> parseMap = FSString::parseTStringToMap1(histName,keys);
  TString subMode = parseMap["_FS_"];
  vector<TString> subModeParts = FSString::parseTString(subMode,"_");
  if (subModeParts.size() != 3){ cout << "histName problem" << endl; exit(0); }
  if (!histName.Contains("_MC_0_0_0_")){ cout << "histName problem" << endl; exit(0); }
  int modeCode3 = FSString::TString2int(subModeParts[0]);
  FSModeInfo mi(subMode);
  FSModeInfo miFS = addModeCode3(mi,modeCode3);
  if (mcComponentsMap.find(subMode) == mcComponentsMap.end()){ cout << "problem " << subMode << endl; exit(0); }
  TH1F* hTot = getTH1FFromHistFile(histFileName,histName);
  double nTot = hTot->Integral(1,hTot->GetNbinsX());
  if (nTot <= 0.0){ nTot = 1.0; }
  vector<TString> mcComponents = mcComponentsMap[subMode];
  vector<TH1F*> histograms;
  vector<double> fractions;
  int iSignal = -1;
  for (unsigned int i = 0; i < mcComponents.size(); i++){
    TString histNameI = histName;
    histNameI.Replace(histNameI.Index("_MC_0_0_0_")+4,5,mcComponents[i]);
    TH1F* histI = getTH1FFromHistFile(histFileName,histNameI);
    double nI = histI->Integral(1,histI->GetNbinsX());
    histograms.push_back(histI);
    fractions.push_back(nI/nTot);
    if (mcComponents[i] == "0_"+miFS.modeString()) iSignal = i;
  }
  THStack* stack = new THStack("sDrawMCComponents","sDrawMCComponents");
  TLegend* legend = new TLegend(0.7,0.5,1.0,1.0);
  int iCol = 2;
  for (unsigned int i = 0; i < histograms.size(); i++){
    if (i == iSignal) continue;
    TH1F* hcomp = histograms[i];
    hcomp->SetFillColor(iCol);
    hcomp->SetLineColor(iCol++);
    stack->Add(hcomp,"hist");
    TString legendString("");
    legendString +=
      FSString::rootSymbols(FSModeHistogram::formatMCComponent(mcComponents[i],fractions[i]));
    legend->AddEntry(hcomp,legendString,"F");
  }
  if (iSignal >= 0){
    int i = iSignal;
    TH1F* hcomp = histograms[i];
    hcomp->SetLineColor(1);
    stack->Add(hcomp,"hist");
    TString legendString("");
    legendString +=
      FSString::rootSymbols(FSModeHistogram::formatMCComponent(mcComponents[i],fractions[i]));
    legend->AddEntry(hcomp,legendString,"F");
  }
  return pair<THStack*,TLegend*>(stack,legend);
}


void setMCComponentsFromTreeFile(TString treeFileName){
  setModeCode3ParticlesEtc();
  TString FN(treeFileName);
  TString NT(FSTree::getTreeNameFromFile(FN));
  if (FSTree::getBranchNamesFromTree(FN,NT,"MCDecayCode1").size() == 0) return;
  FSModeInfo miFS(NT);
  TString subMode0 = "0_"+miFS.modeString();
  if (mcComponentsMap.find(subMode0) != mcComponentsMap.end()) return;
  TString CAT(""); 
  TString VAR("2.0");
  TString BOUNDS("(10,0.0,4.0)");
  FSModeCollection::clear();
  FSModeCollection::addModeInfo(miFS.modeString());
  vector<int> modeCode3List = getModeCode3List(miFS);
  for (unsigned int i = 0; i < modeCode3List.size(); i++){
    int modeCode3 = modeCode3List[i];
    FSModeInfo mi = addModeCode3(miFS,modeCode3,-1);
    TString subMode = FSString::int2TString(modeCode3)+"_"+mi.modeString();
    TString cutCode = "11";
    TString CUTS = getSubModeCuts(modeCode3)+"&&"+
                   getOtherCuts(mi,modeCode3,cutCode,"");
    cout << "*****************************************************\n"
         << " SETTING MC COMPONENTS FOR SUBMODE = " << getDescription(mi,modeCode3) << "\n"
         << "*****************************************************" << endl;
    vector< pair<TString, float> > mcComponents = 
        FSModeHistogram::getMCComponentsAndSizes(FN,NT,CAT,VAR,BOUNDS,CUTS,1.0,false,true);
    cout << "*****************************************************\n"
         << " SELECTING MC COMPONENTS FOR SUBMODE = " << getDescription(mi,modeCode3) << "\n"
         << "*****************************************************" << endl;
    vector<TString> selectedComponents;
    for (unsigned int j = 0; j < mcComponents.size(); j++){
      if (mcComponents[j].second*100 > 0.05){
        selectedComponents.push_back(mcComponents[j].first); 
        cout << FSModeHistogram::formatMCComponent(mcComponents[j].first,mcComponents[j].second) << endl;
      }
    }
    mcComponentsMap[subMode] = selectedComponents;
    cout << "*****************************************************\n"
         << " DONE SETTING MC COMPONENTS FOR SUBMODE = " << getDescription(mi,modeCode3) << "\n"
         << "*****************************************************" << endl;
  }
}

void setMCComponentsFromHistFile(TString histFileName){
  setModeCode3ParticlesEtc();
  TString fsCode = fsCodeFromHistFile(histFileName);
  TString subMode0 = "0_"+fsCode;
  if (mcComponentsMap.find(subMode0) != mcComponentsMap.end()) return;
  if (!isMCFromHistFile(histFileName)) {cout << "not mc" << endl; exit(0);}
  vector<TString> keys; keys.push_back("_FS_"); keys.push_back("_MC_"); keys.push_back("_CUTS_");
  FSModeInfo miFS(fsCode);
  vector<int> modeCode3List = getModeCode3List(miFS);
  for (unsigned int i = 0; i < modeCode3List.size(); i++){
    int modeCode3 = modeCode3List[i];
    FSModeInfo mi = addModeCode3(miFS,modeCode3,-1);
    TString subMode = FSString::int2TString(modeCode3)+"_"+mi.modeString();
    cout << "*****************************************************\n"
         << " SETTING MC COMPONENTS FOR SUBMODE = " << getDescription(mi,modeCode3) << "\n"
         << "*****************************************************" << endl;
    vector< pair<TString,float> > mcComponents1;
    TH1F* hTot = FSHistogram::getTH1F(histFileName,"hist_FS_"+subMode+"_MC_0_0_0_CUTS_011_T");
    if (!hTot){ cout << "histogram problem" << endl; exit(0); }
    double nTot = hTot->GetEntries();
    cout << "TOTAL ENTRIES = " << nTot << endl; 
    if (nTot <= 0.0){ nTot = 1.0; }
    vector<TString> histograms = FSTree::getTObjNamesFromFile(histFileName,"TH1F","hist_FS_"+subMode+"_MC_*_CUTS_011_T");
    if (histograms.size() == 0){ cout << "histogram problem" << endl; exit(0); }
    for (unsigned int j = 0; j < histograms.size(); j++){
      map<TString,TString> parseMap = FSString::parseTStringToMap1(histograms[j],keys);
      TString mcComponent = parseMap["_MC_"];
      if (mcComponent == "0_0_0") continue;
      TH1F* hComp = FSHistogram::getTH1F(histFileName,histograms[j]);
      if (!hComp){ cout << "histogram problem" << endl; exit(0); }
      double nComp = hComp->GetEntries();
      mcComponents1.push_back(pair<TString,float>(mcComponent,nComp/nTot));
    }
    if (mcComponents1.size() > 1){
    for (unsigned int imc1 = 0; imc1 < mcComponents1.size()-1; imc1++){
    for (unsigned int imc2 = imc1+1; imc2 < mcComponents1.size(); imc2++){
      if (mcComponents1[imc1].second < mcComponents1[imc2].second){
        pair<TString,float> temp = mcComponents1[imc1];
        mcComponents1[imc1] = mcComponents1[imc2];
        mcComponents1[imc2] = temp;
      }
    }}}
    vector<TString> mcComponents;
    for (unsigned int j = 0; j < mcComponents1.size(); j++){
      cout << FSModeHistogram::formatMCComponent(mcComponents1[j].first,mcComponents1[j].second) << endl;
      if ((mcComponents1[j].second*100 > 0.1) && (j < 8)) 
        mcComponents.push_back(mcComponents1[j].first);
    }
    mcComponentsMap[subMode] = mcComponents;
    cout << "*****************************************************\n"
         << " DONE SETTING MC COMPONENTS FOR SUBMODE = " << getDescription(mi,modeCode3) << "\n"
         << "*****************************************************" << endl;
  }
}


void writeHistograms(TString treeFileName, TString histFileName){
  setModeCode3ParticlesEtc();
  bool isMC = isMCFromTreeFile(treeFileName);
  if (isMC) setMCComponentsFromTreeFile(treeFileName);
  TString treeName = FSTree::getTreeNameFromFile(treeFileName);
  TFile outFile(FSString::TString2string(histFileName).c_str(), "recreate"); outFile.cd();
  vector<TString> histList = getHistogramList(FSModeInfo(treeName),isMC);
  for (unsigned int i = 0; i < histList.size(); i++){
    TH1F* hist = getTH1FFromTreeFile(treeFileName,treeName,histList[i]);
    outFile.cd();
    hist->SetName(histList[i]);
    hist->Write();
  }
  outFile.Close();
}


TString fsCodeFromHistFile(TString histFileName){
  if (histFileNameToFSCodeMap.find(histFileName) != histFileNameToFSCodeMap.end())
    return histFileNameToFSCodeMap[histFileName];
  vector<TString> histograms;
  histograms = FSTree::getTObjNamesFromFile(histFileName,"TH1F","hist_FS_0_*_CUTS_011_T");
  if (histograms.size() == 0){ cout << "problem 1 in fsCodeFromHistFile" << endl; exit(0); }
  bool isMC = isMCFromHistFile(histFileName);
  TString fsCode("");
  if  (isMC) fsCode = FSString::captureParentheses(histograms[0],0,"FS_0_","_MC_");
  if (!isMC) fsCode = FSString::captureParentheses(histograms[0],0,"FS_0_","_CUTS_");
  if (fsCode == ""){ cout << "problem 2 in fsCodeFromHistFile" << endl; exit(0); }
  histFileNameToFSCodeMap[histFileName] = fsCode;
  return fsCode;
}


pair<TString,TString> makeMCFigure(TString histFileName, TString subMode, TString histType, TString outputFigures){
  clearMemCache();
  setModeCode3ParticlesEtc();
  if (!isMCFromHistFile(histFileName)) {cout << "problem 1 in makeMCFigure" << endl; exit(0);}
  setMCComponentsFromHistFile(histFileName);
  TString histIndexFS = "hist_FS_" + subMode + "_MC_0_0_0";
  cout << "******************************************************" << endl;
  cout << "MC FIGURES FOR " << histIndexFS << endl;
  cout << "histType = " << histType << endl;
  cout << "subMode = " << subMode << endl;
  cout << "******************************************************" << endl;
  TString caption("");
  TCanvas* c1;
  if (histType == "RFTIME"){
    c1 = new TCanvas("c1","c1",1000,1000);
    c1->Divide(1,2);
    c1->cd(1);
    pair<THStack*,TLegend*>
          mcstLowTSigChi2AllRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_110_RFTIME");
    TH1F* histLowTSigChi2AllRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_110_RFTIME");
    FSHistogram::setHistogramMaxima(histLowTSigChi2AllRF);
    histLowTSigChi2AllRF->Draw("");
    mcstLowTSigChi2AllRF.first->Draw("same");
    histLowTSigChi2AllRF->Draw("same");
    mcstLowTSigChi2AllRF.second->Draw("same");
    c1->cd(2);
    pair<THStack*,TLegend*>
          mcstAllTSigChi2AllRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_010_RFTIME");
    TH1F* histAllTSigChi2AllRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_010_RFTIME");
    FSHistogram::setHistogramMaxima(histAllTSigChi2AllRF);
    histAllTSigChi2AllRF->Draw("");
    mcstAllTSigChi2AllRF.first->Draw("same");
    histAllTSigChi2AllRF->Draw("same");
    mcstAllTSigChi2AllRF.second->Draw("same");
    caption = "Distributions of RF $\\Delta t$  broken up into MC components "
              "for the $\\chi^2/\\mathrm{dof}<5$ signal region. "
              "TOP is low $|t|$~($|t|<0.5$~GeV$^{2}$) "
                   "and high $E_{\\mathrm{beam}}$~($E_{\\mathrm{beam}}>8$~GeV); "
              "BOTTOM is all $|t|$ and all $E_{\\mathrm{beam}}$.";
  }
  if (histType == "CHI2DOF"){
    c1 = new TCanvas("c1","c1",1000,1000);
    c1->Divide(2,2);
    c1->cd(1);
    pair<THStack*,TLegend*>
          mcstLowTAllChi2SigRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_101_CHI2DOF");
    TH1F* histLowTAllChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_101_CHI2DOF");
    FSHistogram::setHistogramMaxima(histLowTAllChi2SigRF);
    histLowTAllChi2SigRF->Draw("");
    mcstLowTAllChi2SigRF.first->Draw("same");
    histLowTAllChi2SigRF->Draw("same");
    mcstLowTAllChi2SigRF.second->Draw("same");
    c1->cd(2);
    pair<THStack*,TLegend*>
          mcstLowTAllChi2SubRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_10S_CHI2DOF");
    TH1F* histLowTAllChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_10S_CHI2DOF");
    histLowTAllChi2SubRF->Draw("");
    mcstLowTAllChi2SubRF.first->Draw("same");
    histLowTAllChi2SubRF->Draw("same");
    mcstLowTAllChi2SubRF.second->Draw("same");
    drawZeroLine(histLowTAllChi2SubRF);
    c1->cd(3);
    pair<THStack*,TLegend*>
          mcstAllTAllChi2SigRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_001_CHI2DOF");
    TH1F* histAllTAllChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_001_CHI2DOF");
    FSHistogram::setHistogramMaxima(histAllTAllChi2SigRF);
    histAllTAllChi2SigRF->Draw("");
    mcstAllTAllChi2SigRF.first->Draw("same");
    histAllTAllChi2SigRF->Draw("same");
    mcstAllTAllChi2SigRF.second->Draw("same");
    c1->cd(4);
    pair<THStack*,TLegend*>
          mcstAllTAllChi2SubRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_00S_CHI2DOF");
    TH1F* histAllTAllChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_00S_CHI2DOF");
    histAllTAllChi2SubRF->Draw("");
    mcstAllTAllChi2SubRF.first->Draw("same");
    histAllTAllChi2SubRF->Draw("same");
    mcstAllTAllChi2SubRF.second->Draw("same");
    drawZeroLine(histAllTAllChi2SubRF);
    caption = "Distributions of $\\chi^2$/dof broken up into MC components.  "
              "TOP is low $|t|$~($|t|<0.5$~GeV$^{2}$) "
                   "and high $E_{\\mathrm{beam}}$~($E_{\\mathrm{beam}}>8$~GeV); "
              "BOTTOM is all $|t|$ and all $E_{\\mathrm{beam}}$. "
              "LEFT is in time; "
              "RIGHT is time subtracted.";
  }
  if (histType == "T"){
    c1 = new TCanvas("c1","c1",1000,1000);
    c1->Divide(2,2);
    c1->cd(1);
    pair<THStack*,TLegend*>
          mcstLowTSigChi2SigRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_111_T");
    TH1F* histLowTSigChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_111_T");
    FSHistogram::setHistogramMaxima(histLowTSigChi2SigRF);
    histLowTSigChi2SigRF->Draw("");
    mcstLowTSigChi2SigRF.first->Draw("same");
    histLowTSigChi2SigRF->Draw("same");
    mcstLowTSigChi2SigRF.second->Draw("same");
    c1->cd(2);
    pair<THStack*,TLegend*>
          mcstLowTSigChi2SubRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_11S_T");
    TH1F* histLowTSigChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_11S_T");
    FSHistogram::setHistogramMaxima(histLowTSigChi2SubRF);
    histLowTSigChi2SubRF->Draw("");
    mcstLowTSigChi2SubRF.first->Draw("same");
    histLowTSigChi2SubRF->Draw("same");
    mcstLowTSigChi2SubRF.second->Draw("same");
    drawZeroLine(histLowTSigChi2SubRF);
    c1->cd(3);
    pair<THStack*,TLegend*>
          mcstAllTSigChi2SigRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_011_T");
    TH1F* histAllTSigChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_011_T");
    FSHistogram::setHistogramMaxima(histAllTSigChi2SigRF);
    histAllTSigChi2SigRF->Draw("");
    mcstAllTSigChi2SigRF.first->Draw("same");
    histAllTSigChi2SigRF->Draw("same");
    mcstAllTSigChi2SigRF.second->Draw("same");
    c1->cd(4);
    pair<THStack*,TLegend*>
          mcstAllTSigChi2SubRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_01S_T");
    TH1F* histAllTSigChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_01S_T");
    FSHistogram::setHistogramMaxima(histAllTSigChi2SubRF);
    histAllTSigChi2SubRF->Draw("");
    mcstAllTSigChi2SubRF.first->Draw("same");
    histAllTSigChi2SubRF->Draw("same");
    mcstAllTSigChi2SubRF.second->Draw("same");
    drawZeroLine(histAllTSigChi2SubRF);
    caption = "Distributions of $-t$ broken up into MC components. "
              "TOP is high $E_{\\mathrm{beam}}$~($E_{\\mathrm{beam}}>8$~GeV); "
              "BOTTOM is all $E_{\\mathrm{beam}}$. "
              "LEFT is in time and "
              "RIGHT is time subtracted, "
              "all in the $\\chi^2/\\mathrm{dof}<5$ signal region.";
  }
  if (histType.Contains("MASS")){
    c1 = new TCanvas("c1","c1",1000,1000);
    c1->Divide(2,2);
    c1->cd(1);
    pair<THStack*,TLegend*>
          mcstLowTSigChi2SigRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_111_"+histType);
    TH1F* histLowTSigChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_111_"+histType);
    FSHistogram::setHistogramMaxima(histLowTSigChi2SigRF);
    histLowTSigChi2SigRF->Draw("");
    mcstLowTSigChi2SigRF.first->Draw("same");
    histLowTSigChi2SigRF->Draw("same");
    mcstLowTSigChi2SigRF.second->Draw("same");
    c1->cd(2);
    pair<THStack*,TLegend*>
          mcstLowTSigChi2SubRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_11S_"+histType);
    TH1F* histLowTSigChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_11S_"+histType);
    FSHistogram::setHistogramMaxima(histLowTSigChi2SubRF);
    histLowTSigChi2SubRF->Draw("");
    mcstLowTSigChi2SubRF.first->Draw("same");
    histLowTSigChi2SubRF->Draw("same");
    mcstLowTSigChi2SubRF.second->Draw("same");
    drawZeroLine(histLowTSigChi2SubRF);
    c1->cd(3);
    pair<THStack*,TLegend*>
          mcstAllTSigChi2SigRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_011_"+histType);
    TH1F* histAllTSigChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_011_"+histType);
    FSHistogram::setHistogramMaxima(histAllTSigChi2SigRF);
    histAllTSigChi2SigRF->Draw("");
    mcstAllTSigChi2SigRF.first->Draw("same");
    histAllTSigChi2SigRF->Draw("same");
    mcstAllTSigChi2SigRF.second->Draw("same");
    c1->cd(4);
    pair<THStack*,TLegend*>
          mcstAllTSigChi2SubRF = getTHStFromHistFile(histFileName,histIndexFS+"_CUTS_01S_"+histType);
    TH1F* histAllTSigChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_01S_"+histType);
    FSHistogram::setHistogramMaxima(histAllTSigChi2SubRF);
    histAllTSigChi2SubRF->Draw("");
    mcstAllTSigChi2SubRF.first->Draw("same");
    histAllTSigChi2SubRF->Draw("same");
    mcstAllTSigChi2SubRF.second->Draw("same");
    drawZeroLine(histAllTSigChi2SubRF);
    caption = "Mass distributions broken up into MC components.  "
              "TOP is low $|t|$~($|t|<0.5$~GeV$^{2}$) "
                   "and high $E_{\\mathrm{beam}}$~($E_{\\mathrm{beam}}>8$~GeV); "
              "BOTTOM is all $|t|$ and all $E_{\\mathrm{beam}}$. "
              "LEFT is in time and "
              "RIGHT is time subtracted, "
              "all in the $\\chi^2/\\mathrm{dof}<5$ signal region.";
  }
  c1->cd();
  TString pdfFileName = outputFigures+"/"+histIndexFS+"_"+histType+"_MC.pdf";
  c1->Print(pdfFileName);
  return pair<TString,TString>(pdfFileName,caption);
}


pair<TString,TString> makeFigure(TString histFileName, TString subMode, TString histType, TString outputFigures){
  clearMemCache();
  setModeCode3ParticlesEtc();
  bool isMC = isMCFromHistFile(histFileName);
  TString histIndexFS = "hist_FS_" + subMode;  if (isMC) histIndexFS += "_MC_0_0_0";
  cout << "******************************************************" << endl;
  cout << "FIGURES FOR " << histIndexFS << endl;
  cout << "histType = " << histType << endl;
  cout << "subMode = " << subMode << endl;
  cout << "******************************************************" << endl;
  TString caption("");
  TCanvas* c1;
  if (histType == "RFTIME"){
    c1 = new TCanvas("c1","c1",1000,1000);
    c1->Divide(1,2);
    c1->cd(1);
    TH1F* histLowTSigChi2AllRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_110_RFTIME");
    TH1F* histLowTOutChi2AllRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_120_RFTIME");
    FSHistogram::setHistogramMaxima(histLowTSigChi2AllRF,histLowTOutChi2AllRF);
    histLowTOutChi2AllRF->SetLineColor(kAzure-4);
    histLowTOutChi2AllRF->SetFillStyle(3002);
    histLowTOutChi2AllRF->SetFillColor(kAzure-4);
    histLowTSigChi2AllRF->Draw("");
    histLowTOutChi2AllRF->Draw("hist,same");
    histLowTSigChi2AllRF->Draw("same");
    c1->cd(2);
    TH1F* histAllTSigChi2AllRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_010_RFTIME");
    TH1F* histAllTOutChi2AllRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_020_RFTIME");
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
    TH1F* histLowTAllChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_101_CHI2DOF");
    TH1F* histLowTAllChi2OutRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_102_CHI2DOF");
    FSHistogram::setHistogramMaxima(histLowTAllChi2SigRF,histLowTAllChi2OutRF);
    histLowTAllChi2OutRF->SetLineColor(kRed-3);
    histLowTAllChi2OutRF->SetFillStyle(3002);
    histLowTAllChi2OutRF->SetFillColor(kRed-3);
    histLowTAllChi2SigRF->Draw("");
    histLowTAllChi2OutRF->Draw("hist,same");
    histLowTAllChi2SigRF->Draw("same");
    c1->cd(2);
    TH1F* histLowTAllChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_10S_CHI2DOF");
    histLowTAllChi2SubRF->Draw("");
    c1->cd(3);
    TH1F* histAllTAllChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_001_CHI2DOF");
    TH1F* histAllTAllChi2OutRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_002_CHI2DOF");
    FSHistogram::setHistogramMaxima(histAllTAllChi2SigRF,histAllTAllChi2OutRF);
    histAllTAllChi2OutRF->SetLineColor(kRed-3);
    histAllTAllChi2OutRF->SetFillStyle(3002);
    histAllTAllChi2OutRF->SetFillColor(kRed-3);
    histAllTAllChi2SigRF->Draw("");
    histAllTAllChi2OutRF->Draw("hist,same");
    histAllTAllChi2SigRF->Draw("same");
    c1->cd(4);
    TH1F* histAllTAllChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_00S_CHI2DOF");
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
    TH1F* histLowTSigChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_111_T");
    TH1F* histLowTSigChi2OutRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_112_T");
    FSHistogram::setHistogramMaxima(histLowTSigChi2SigRF,histLowTSigChi2OutRF);
    histLowTSigChi2OutRF->SetLineColor(kRed-3);
    histLowTSigChi2OutRF->SetFillStyle(3002);
    histLowTSigChi2OutRF->SetFillColor(kRed-3);
    histLowTSigChi2SigRF->Draw("");
    histLowTSigChi2OutRF->Draw("hist,same");
    histLowTSigChi2SigRF->Draw("same");
    c1->cd(2);
    TH1F* histLowTSigChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_11S_T");
    TH1F* histLowTOutChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_12S_T");
    FSHistogram::setHistogramMaxima(histLowTSigChi2SubRF,histLowTOutChi2SubRF);
    FSHistogram::setHistogramMinima(histLowTSigChi2SubRF,histLowTOutChi2SubRF);
    histLowTOutChi2SubRF->SetLineColor(kAzure-4);
    histLowTOutChi2SubRF->SetFillStyle(3002);
    histLowTOutChi2SubRF->SetFillColor(kAzure-4);
    histLowTSigChi2SubRF->Draw("");
    histLowTOutChi2SubRF->Draw("hist,same");
    histLowTSigChi2SubRF->Draw("same");
    c1->cd(3);
    TH1F* histAllTSigChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_011_T");
    TH1F* histAllTSigChi2OutRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_012_T");
    FSHistogram::setHistogramMaxima(histAllTSigChi2SigRF,histAllTSigChi2OutRF);
    histAllTSigChi2OutRF->SetLineColor(kRed-3);
    histAllTSigChi2OutRF->SetFillStyle(3002);
    histAllTSigChi2OutRF->SetFillColor(kRed-3);
    histAllTSigChi2SigRF->Draw("");
    histAllTSigChi2OutRF->Draw("hist,same");
    histAllTSigChi2SigRF->Draw("same");
    c1->cd(4);
    TH1F* histAllTSigChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_01S_T");
    TH1F* histAllTOutChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_02S_T");
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
    TH1F* histLowTSigChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_111_"+histType);
    TH1F* histLowTSigChi2OutRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_112_"+histType);
    FSHistogram::setHistogramMaxima(histLowTSigChi2SigRF,histLowTSigChi2OutRF);
    histLowTSigChi2OutRF->SetLineColor(kRed-3);
    histLowTSigChi2OutRF->SetFillStyle(3002);
    histLowTSigChi2OutRF->SetFillColor(kRed-3);
    histLowTSigChi2SigRF->Draw("");
    histLowTSigChi2OutRF->Draw("hist,same");
    histLowTSigChi2SigRF->Draw("same");
    c1->cd(2);
    TH1F* histLowTSigChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_11S_"+histType);
    TH1F* histLowTOutChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_12S_"+histType);
    FSHistogram::setHistogramMaxima(histLowTSigChi2SubRF,histLowTOutChi2SubRF);
    FSHistogram::setHistogramMinima(histLowTSigChi2SubRF,histLowTOutChi2SubRF);
    histLowTOutChi2SubRF->SetLineColor(kAzure-4);
    histLowTOutChi2SubRF->SetFillStyle(3002);
    histLowTOutChi2SubRF->SetFillColor(kAzure-4);
    histLowTSigChi2SubRF->Draw("");
    histLowTOutChi2SubRF->Draw("hist,same");
    histLowTSigChi2SubRF->Draw("same");
    c1->cd(3);
    TH1F* histAllTSigChi2SigRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_011_"+histType);
    TH1F* histAllTSigChi2OutRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_012_"+histType);
    FSHistogram::setHistogramMaxima(histAllTSigChi2SigRF,histAllTSigChi2OutRF);
    histAllTSigChi2OutRF->SetLineColor(kRed-3);
    histAllTSigChi2OutRF->SetFillStyle(3002);
    histAllTSigChi2OutRF->SetFillColor(kRed-3);
    histAllTSigChi2SigRF->Draw("");
    histAllTSigChi2OutRF->Draw("hist,same");
    histAllTSigChi2SigRF->Draw("same");
    c1->cd(4);
    TH1F* histAllTSigChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_01S_"+histType);
    TH1F* histAllTOutChi2SubRF = getTH1FFromHistFile(histFileName,histIndexFS+"_CUTS_02S_"+histType);
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
  setModeCode3ParticlesEtc();
  bool isMC = isMCFromHistFile(histFileName);
  if (isMC) setMCComponentsFromHistFile(histFileName);
  TString fsCode = fsCodeFromHistFile(histFileName);
  FSModeInfo miFS(fsCode);
  TString fsDescription(getDescription(miFS,0));
  outputDirectory = FSSystem::getAbsolutePath(outputDirectory,false);
  if (outputDirectory == ""){ cout << "problem with output directory" << endl; exit(0); }
  outputDirectory = outputDirectory + "/" + baseName;
  if (FSSystem::getAbsolutePath(outputDirectory,false) != "")
    { cout << "directory already exists: " << outputDirectory << endl; 
      cout << "WARNING, replacing it!" << endl;  system("rm -rf "+outputDirectory);}
  cout << "Creating PDF directory:     " << outputDirectory << endl;
  system("mkdir "+outputDirectory);
  TString outputFigures = outputDirectory + "/figures";
  cout << "Creating figures directory: " << outputFigures << endl;
  system("mkdir "+outputFigures);
  TString latexFile = outputDirectory + "/"+baseName+".tex";
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
  if (isMC) FSString::writeTStringToFile(latexFile,
    "The following plots are for MC (add more information).\n");
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

    if (isMC){
      histInfo = makeMCFigure(histFileName,subMode,"RFTIME",outputFigures);
      FSString::writeTStringToFile(latexFile,
        "\\newpage\n\n"
        "\\begin{figure}[h!]\n"
        "\\begin{center}\n"
        "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
        "\\end{center}\n"
        "\\caption{"+histInfo.second+"}\n"
        "\\end{figure}\n\n");
    }

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

    if (isMC){
      histInfo = makeMCFigure(histFileName,subMode,"CHI2DOF",outputFigures);
      FSString::writeTStringToFile(latexFile,
        "\\newpage\n\n"
        "\\begin{figure}[h!]\n"
        "\\begin{center}\n"
        "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
        "\\end{center}\n"
        "\\caption{"+histInfo.second+"}\n"
        "\\end{figure}\n\n");
    }

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

    if (isMC){
      histInfo = makeMCFigure(histFileName,subMode,"T",outputFigures);
      FSString::writeTStringToFile(latexFile,
        "\\newpage\n\n"
        "\\begin{figure}[h!]\n"
        "\\begin{center}\n"
        "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
        "\\end{center}\n"
        "\\caption{"+histInfo.second+"}\n"
        "\\end{figure}\n\n");
    }

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
      if (isMC){
        histInfo = makeMCFigure(histFileName,subMode,"MASS_"+massCombinations[j],outputFigures);
        FSString::writeTStringToFile(latexFile,
          "\\newpage\n\n"
          "\\begin{figure}[h!]\n"
          "\\begin{center}\n"
          "\\includegraphics[width=\\columnwidth]{"+histInfo.first+"}"
          "\\end{center}\n"
          "\\caption{"+histInfo.second+"}\n"
          "\\end{figure}\n\n");
      }
    }

  }
  FSString::latexCloser(latexFile);
  system("pdflatex -output-directory "+outputDirectory+" "+latexFile);
  system("pdflatex -output-directory "+outputDirectory+" "+latexFile);
  system("pdflatex -output-directory "+outputDirectory+" "+latexFile);
}


bool isMCFromTreeFile(TString treeFileName){
  TString FN(treeFileName);
  TString NT(FSTree::getTreeNameFromFile(FN));
  if (FSTree::getBranchNamesFromTree(FN,NT,"MCDecayCode1").size() == 0) return false;
  return true;
}

bool isMCFromHistFile(TString histFileName){
  if (FSTree::getTObjNamesFromFile(histFileName,"TH1F","*_MC_*").size() == 0) return false;
  return true;
}

void drawZeroLine(TH1F* hist){
  double x1 = hist->GetBinLowEdge(1);
  double x2 = hist->GetBinLowEdge(hist->GetNbinsX()) + hist->GetBinWidth(1);
  TLine* line = new TLine(x1,0.0,x2,0.0); line->Draw("same");
}
