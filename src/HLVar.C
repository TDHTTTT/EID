#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>     
#endif

//------------------------------------------------------------------------------ 
// notes: clean up float ->Float_t, etc

void plotHL(bool log, const char *YT, const char *XT, const char *imgName, TH1 *hbg, TH1 *hsig){
  TCanvas *c = new TCanvas(imgName,imgName);
  if(log==true){
    c->SetLogy();
  }
  hbg->SetYTitle(YT);
  hbg->SetXTitle(XT);
  hbg->Draw();
  c->Update();
  hsig->SetLineColor(kRed);
  hsig->Draw("same");
  TImage *img = TImage::Create();
  img->FromPad(c);
  char pngName[100];
  strcpy(pngName, imgName);
  strcat(pngName, ".png");
  img->WriteImage(pngName);

}

Float_t get_Rhad(TH2 *histEcal_ET, TH2 *histHcal_ET){
  Float_t Rhad = -999.;
  Float_t sumH_ET = 0.;
  Float_t sumE_ET = 0.;
  for(Int_t j=1; j<=31; j++)
  {
    for(Int_t i=1; i<=31; i++)
    {
      sumE_ET += histEcal_ET->GetBinContent(i,j);
    }
  }
  for(Int_t j=1; j<=32; j++)
  {
    for(Int_t i=1; i<=32; i++)
    {
      sumH_ET += histHcal_ET->GetBinContent(i,j);
    }
  }
  
  Rhad = sumH_ET/sumE_ET;
  return Rhad; 
}

Float_t get_wEta2(int Ecal_binCenter_x, int Ecal_binCenter_y, TH2 *histEcal_E){
  Float_t wEta2 = 0.;
  Float_t expEta2 = 0.;
  Float_t expEta = 0.;
  Float_t sumE = 0.;
  Float_t relEta = 0;
  TAxis *histEcal_E_xAxis = histEcal_E->GetXaxis();

  for(Int_t j=Ecal_binCenter_y-2; j<= Ecal_binCenter_y+2; j++)
  {
    for(Int_t i=Ecal_binCenter_x-1; i<=Ecal_binCenter_x+1; i++)
    {
      // NOTE: Assuming relative eta value of bin is what is used
      relEta = histEcal_E_xAxis->GetBinCenter(i);
    
      if(i == Ecal_binCenter_x){
        relEta = 0.;
      }
      sumE += histEcal_E->GetBinContent(i,j);
      expEta += histEcal_E->GetBinContent(i,j) * relEta; 
      expEta2 += histEcal_E->GetBinContent(i,j) * std::pow(relEta,2);
    }
  }
  wEta2 = std::sqrt( ( (expEta2/sumE) - std::pow((expEta/sumE), 2) ) );
  return wEta2;
}

Float_t get_Rphi(int Ecal_binCenter_x, int Ecal_binCenter_y, TH2 *histEcal_E){
  Float_t Rphi = 0.;
  Float_t sumE3x3 = 0.;      // Sum in (eta, phi) = (3x3) window
  Float_t sumE3x7 = 0.;      // Sum in (eta, phi) = (3x7) window
  
  for(Int_t j=Ecal_binCenter_y-1; j<= Ecal_binCenter_y+1; j++)
  {
    for(Int_t i=Ecal_binCenter_x-1; i<=Ecal_binCenter_x+1; i++)
    {
      sumE3x3  += histEcal_E->GetBinContent(i,j);
    }
  }
  for(Int_t j=Ecal_binCenter_y-3; j<= Ecal_binCenter_y+3; j++)
  {
    for(Int_t i=Ecal_binCenter_x-1; i<=Ecal_binCenter_x+1; i++)
    {
      sumE3x7 += histEcal_E->GetBinContent(i,j);
    }
  }

  Rphi = sumE3x3/sumE3x7;
  return Rphi;
}

Float_t get_Reta(int Ecal_binCenter_x, int Ecal_binCenter_y, TH2 *histEcal_E){
  Float_t Reta = 0.;
  Float_t sumE3x7 = 0.;      // Sum in (eta, phi) = (3x3) window
  Float_t sumE7x7 = 0.;      // Sum in (eta, phi) = (3x7) window

  for(Int_t j=Ecal_binCenter_y-3; j<= Ecal_binCenter_y+3; j++)
  {
    for(Int_t i=Ecal_binCenter_x-1; i<=Ecal_binCenter_x+1; i++)
    {
      sumE3x7  += histEcal_E->GetBinContent(i,j);
    }
  }
  for(Int_t j=Ecal_binCenter_y-3; j<= Ecal_binCenter_y+3; j++)
  {
    for(Int_t i=Ecal_binCenter_x-3; i<=Ecal_binCenter_x+3; i++)
    {
      sumE7x7 += histEcal_E->GetBinContent(i,j);
    }
  }

  Reta = sumE3x7/sumE7x7;
  return Reta;
}

Float_t get_sigmaEtaEta(int Ecal_binCenter_x, int Ecal_binCenter_y, TH2 *histEcal_E){
  Float_t sigmaEtaEta = 0.;
  Float_t wExpEta2 = 0;
  Float_t sumW = 0.;

  // Sum in (eta, phi) = (5x5) window
  for(Int_t j=Ecal_binCenter_y-2; j<= Ecal_binCenter_y+2; j++)
  {
    for(Int_t i=Ecal_binCenter_x-2; i<=Ecal_binCenter_x+2; i++)
    {
      if(histEcal_E->GetBinContent(i,j) == 0.) continue;

      sumW += std::abs(std::log(histEcal_E->GetBinContent(i,j)));
      wExpEta2 += std::abs(std::log(histEcal_E->GetBinContent(i,j))) * std::pow(float(i-Ecal_binCenter_x),2);
      // Formula is (eta_i - etabar)**2 in units of crystals(cells), so rel eta measured in # of cells = i-Ecal_binCenter_x
    }
  }
  sigmaEtaEta = std::sqrt( wExpEta2/sumW );
  return sigmaEtaEta;
}

std::vector<Float_t> get_DRs(TH2 *histEcal_E){
  Float_t DeltaR03 = 0.;
  Float_t DeltaR04 = 0.;
  Float_t dEta = 0.;
  Float_t dPhi = 0.;

  TAxis *histEcal_E_xAxis = histEcal_E->GetXaxis();
  TAxis *histEcal_E_yAxis = histEcal_E->GetYaxis();

  for(Int_t j=1; j<=31; j++)
  {
    for(Int_t i=1; i<=31; i++)
    {
      dEta = histEcal_E_xAxis->GetBinCenter(i);
      dPhi = histEcal_E_yAxis->GetBinCenter(j);
      if(std::sqrt(std::pow(dEta,2) + std::pow(dPhi,2)) < 0.3 )
      {
        DeltaR03 += histEcal_E->GetBinContent(i,j); 
      }
      if(std::sqrt(std::pow(dEta,2) + std::pow(dPhi,2)) < 0.4 )
      {
        DeltaR04 += histEcal_E->GetBinContent(i,j);
      }
    }
  }

  std::vector<Float_t> DRs = {DeltaR03, DeltaR04};
  return DRs;
}

void HLVar(){


  /*******************************************/
  /** Create csv files to store HL var data **/
  /*******************************************/
  std::ofstream ofHLVar;
  ofHLVar.open("HLVar_bg.csv");

  std::ofstream ofHLVarr;
  ofHLVarr.open("HLVar_sig.csv");


  /******************************************/
  /** Declare input csv files file streams **/
  /******************************************/

  // For reference: http://www.cplusplus.com/reference/iostream/ifstream/
  // Background:
  std::ifstream infEcal_E  ("/c/Users/tdhttt/workspace/hep/hpc-data/csv/bg/Ecal_E.csv");
  std::ifstream infEcal_ET ("/c/Users/tdhttt/workspace/hep/hpc-data/csv/bg/Ecal_ET.csv");
  std::ifstream infHcal_E  ("/c/Users/tdhttt/workspace/hep/hpc-data/csv/bg/Hcal_E.csv");
  std::ifstream infHcal_ET ("/c/Users/tdhttt/workspace/hep/hpc-data/csv/bg/Hcal_ET.csv");

  // Signal:
  std::ifstream infEcal_EE  ("/c/Users/tdhttt/workspace/hep/hpc-data/csv/sig/Ecal_E.csv");
  std::ifstream infEcal_ETT ("/c/Users/tdhttt/workspace/hep/hpc-data/csv/sig/Ecal_ET.csv");
  std::ifstream infHcal_EE  ("/c/Users/tdhttt/workspace/hep/hpc-data/csv/sig/Hcal_E.csv");
  std::ifstream infHcal_ETT ("/c/Users/tdhttt/workspace/hep/hpc-data/csv/sig/Hcal_ET.csv");

  // Define size of each row (number of elements)
  // Define an array to keep elements in         
  const Int_t sizeEcal = 31*31;                        
  const Int_t sizeHcal = 32*32;                        
  Float_t imEcal_E[sizeEcal];
  Float_t imEcal_ET[sizeEcal];
  Float_t imHcal_E[sizeHcal];
  Float_t imHcal_ET[sizeHcal];

  Float_t imEcal_EE[sizeEcal];
  Float_t imEcal_ETT[sizeEcal];
  Float_t imHcal_EE[sizeHcal];
  Float_t imHcal_ETT[sizeHcal];

  // Define iterators, line, and value variables 
  Int_t I    = 0;
  Int_t iee  = 0; // Ecal E
  Int_t ieet = 0; // Ecal ET
  Int_t ihe  = 0;
  Int_t ihet = 0;
  Int_t ieee  = 0; // Ecal E
  Int_t ieett = 0; // Ecal ET
  Int_t ihee  = 0;
  Int_t ihett = 0;
  string valEcal_E,  valEcal_ET,  valHcal_E,  valHcal_ET;
  string lineEcal_E, lineEcal_ET, lineHcal_E, lineHcal_ET;
  string valEcal_EE,  valEcal_ETT,  valHcal_EE,  valHcal_ETT;
  string lineEcal_EE, lineEcal_ETT, lineHcal_EE, lineHcal_ETT;


  /************************************************/
  /** Histograms for aggregate and single images **/
  /************************************************/
  // Ecal: 31x31 images cell size (eta, phi) = (y, x) = (0.025, pi/126)
  Float_t pi = 3.14159265358979323846;
  
  Float_t binWidth_EcalPhi =  pi/126;
  Float_t binWidth_EcalEta = 0.025;

  Float_t histLowEx  = -1.*(15.5*binWidth_EcalPhi);
  Float_t histHighEx = +1.*(15.5*binWidth_EcalPhi);
  Float_t histLowEy  = -1.*(15.5*binWidth_EcalEta);
  Float_t histHighEy = +1.*(15.5*binWidth_EcalEta);

  // Background:
  TH2 *histEcal_E = new TH2F("histEcal_E", "Single Event Energy in ECal", 31, histLowEx, histHighEx, 31, histLowEy, histHighEy);
  TH2 *histEcal_ET = new TH2F("histEcal_ET", "Single Event ET in ECal", 31, histLowEx, histHighEx, 31, histLowEy, histHighEy);

  // Signal:
  TH2 *histEcal_EE = new TH2F("histEcal_EE", "Single Event Energy in ECal", 31, histLowEx, histHighEx, 31, histLowEy, histHighEy);
  TH2 *histEcal_ETT = new TH2F("histEcal_ETT", "Single Event ET in ECal", 31, histLowEx, histHighEx, 31, histLowEy, histHighEy);

  Float_t binWidth_HcalPhi = pi/31.;
  Float_t binWidth_HcalEta = 0.1;

  Float_t histLowHx  = -1.*(4*binWidth_HcalEta);
  Float_t histHighHx = +1.*(4*binWidth_HcalEta);
  Float_t histLowHy  = -1.*(4*binWidth_HcalPhi);  
  Float_t histHighHy = +1.*(4*binWidth_HcalPhi);

  // Hcal ~same pixel size as Ecal: 32x32 images cell size (eta, phi) = (y, x) = 1/4 * (0.025, pi/126)                       
  // Background: 
  TH2 *histHcal_E = new TH2F("histHcal_E", "Single Event Energy in HCal 2", 32, histLowHx, histHighHx, 32, histLowHy, histHighHy);
  TH2 *histHcal_ET = new TH2F("histHcal_ET", "Single Event ET in HCal 2", 32, histLowHx, histHighHx, 32, histLowHy, histHighHy);

  // Signal:
  TH2 *histHcal_EE = new TH2F("histHcal_EE", "Single Event Energy in HCal 2", 32, histLowHx, histHighHx, 32, histLowHy, histHighHy);
  TH2 *histHcal_ETT = new TH2F("histHcal_ETT", "Single Event ET in HCal 2", 32, histLowHx, histHighHx, 32, histLowHy, histHighHy);

  // 1D histograms of variables
  TH1 *h1 = new TH1F("h1", "Rhad", 100, -1., 20.);
  TH1 *h11 = new TH1F("h11", "Rhad", 100, -1., 20.);
  TH1 *h2 = new TH1F("h2", "wEta2", 100, -0.001, 0.025);
  TH1 *h22 = new TH1F("h22", "wEta2", 100, -0.001, 0.025);
  TH1 *h3 = new TH1F("h3", "Rphi", 100, 0.0, 1.1);
  TH1 *h33 = new TH1F("h33", "Rphi", 100, 0.0, 1.1);
  TH1 *h4 = new TH1F("h4", "Reta", 100, 0.2, 1.1);
  TH1 *h44 = new TH1F("h44", "Reta", 100, 0.2, 1.1);
  TH1 *h5 = new TH1F("h5", "sigmaEtaEta", 100, 0.005, 1.5);
  TH1 *h55 = new TH1F("h55", "sigmaEtaEta", 100, 0.005, 1.5);
  TH1 *h6 = new TH1F("h6", "DelR03", 100, 0., 200.);
  TH1 *h66 = new TH1F("h66", "DelR03", 100, 0., 200.);
  TH1 *h7 = new TH1F("h7", "DelR04", 100, 0., 200.);
  TH1 *h77 = new TH1F("h77", "DelR04", 100, 0., 200.);

  
  /**********************/
  /** Go through files **/
  /**********************/
  // Reference: http://www.cplusplus.com/reference/ios/ios/good/
  while (infEcal_E.good() && infEcal_ET.good() && infHcal_E.good() && infHcal_ET.good()) 
  {
    // Reset iterators
    iee  = 0;
    ieet = 0;
    ihe  = 0;
    ihet = 0;

    ieee  = 0;
    ieett = 0;
    ihee  = 0;
    ihett = 0;
    // Reset arrays?

    // Get line from file (each line is a 31x31 (32x32) image => vector will have length 961 (1024))
    getline(infEcal_E, lineEcal_E);
    getline(infEcal_ET, lineEcal_ET);
    getline(infHcal_E, lineHcal_E);
    getline(infHcal_ET, lineHcal_ET);

    getline(infEcal_EE, lineEcal_EE);
    getline(infEcal_ETT, lineEcal_ETT);
    getline(infHcal_EE, lineHcal_EE);
    getline(infHcal_ETT, lineHcal_ETT);

    // Get string stream
    stringstream see(lineEcal_E);
    stringstream seet(lineEcal_ET);
    stringstream she(lineHcal_E);
    stringstream shet(lineHcal_ET);

    stringstream seee(lineEcal_EE);
    stringstream seett(lineEcal_ETT);
    stringstream shee(lineHcal_EE);
    stringstream shett(lineHcal_ETT);

    // Parse stream by commas, convert to floats and store in array 
    while (std::getline(see, valEcal_E, ',')) {
      imEcal_E[iee] = stof( valEcal_E );
      iee++;
    }
    while (std::getline(seet, valEcal_ET, ',')) {
      imEcal_ET[ieet] = stof( valEcal_ET );
      ieet++;
    }
    while (std::getline(she, valHcal_E, ',')) {
      imHcal_E[ihe] = stof( valHcal_E );
      ihe++;
    }
    while (std::getline(shet, valHcal_ET, ',')) {
      imHcal_ET[ihet] = stof( valHcal_ET );
      ihet++;
    }

    while (std::getline(seee, valEcal_EE, ',')) {
      imEcal_EE[ieee] = stof( valEcal_EE );
      ieee++;
    }
    while (std::getline(seett, valEcal_ETT, ',')) {
      imEcal_ETT[ieett] = stof( valEcal_ETT );
      ieett++;
    }
    while (std::getline(shee, valHcal_EE, ',')) {
      imHcal_EE[ihee] = stof( valHcal_EE );
      ihee++;
    }
    while (std::getline(shett, valHcal_ETT, ',')) {
      imHcal_ETT[ihett] = stof( valHcal_ETT );
      ihett++;
    }


    /*******************************/
    /** Store images in histogram **/
    /*******************************/
    I = 0;
    for(Int_t j=31; j>=1; j--)
    {
      for(Int_t i=1; i<=31; i++)
      {
        histEcal_E->SetBinContent(i,j, imEcal_E[I]);
        histEcal_ET->SetBinContent(i,j,imEcal_ET[I]);
        histEcal_EE->SetBinContent(i,j, imEcal_EE[I]);
        histEcal_ETT->SetBinContent(i,j,imEcal_ETT[I]);
        I++;
      }
    }

    I = 0;
    for(Int_t j=32; j>=1; j--)
    {
      for(Int_t i=1; i<=32; i++)
      {
        histHcal_E->SetBinContent(i,j, imHcal_E[I]);
        histHcal_ET->SetBinContent(i,j,imHcal_ET[I]);
        histHcal_EE->SetBinContent(i,j, imHcal_EE[I]);
        histHcal_ETT->SetBinContent(i,j,imHcal_ETT[I]);
        I++;
      }
    }


    /****************************/
    /** Calculate HL variables **/
    /****************************/
    // Background: 
    Float_t Rhad = -999.;
    Float_t wEta2 = -999.;
    Float_t Rphi = -999.;
    Float_t Reta = -999.;
    Float_t sigmaEtaEta = -999.;
    Float_t DeltaR03 = -999.;
    Float_t DeltaR04 = -999.;
    std::vector<Float_t> DRs = {-999., -999.};
    Float_t Ecal_binCenter_x = 16; // (31/2)+0.5
    Float_t Ecal_binCenter_y = Ecal_binCenter_x; // Since image is square
    Float_t ePT = -999.;

    Rhad = get_Rhad(histEcal_ET, histHcal_ET);
    wEta2 = get_wEta2(Ecal_binCenter_x, Ecal_binCenter_y, histEcal_E);
    Rphi = get_Rphi(Ecal_binCenter_x, Ecal_binCenter_y, histEcal_E);
    Reta = get_Reta(Ecal_binCenter_x, Ecal_binCenter_y, histEcal_E);
    sigmaEtaEta = get_sigmaEtaEta(Ecal_binCenter_x, Ecal_binCenter_y, histEcal_E);
    DRs = get_DRs(histEcal_E);
    DeltaR03 = DRs.at(0);
    DeltaR04 = DRs.at(1);


    h1->Fill(Rhad);
    h2->Fill(wEta2);
    h3->Fill(Rphi);
    h4->Fill(Reta);
    h5->Fill(sigmaEtaEta);
    h6->Fill(DeltaR03);
    h7->Fill(DeltaR04);

    // Singal: 
    Float_t Rhadd = -999.;
    Float_t wEta22 = -999.;
    Float_t Rphii = -999.;
    Float_t Retaa = -999.;
    Float_t sigmaEtaEtaa = -999.;
    Float_t DeltaR033 = -999.;
    Float_t DeltaR044 = -999.;
    std::vector<Float_t> DRss = {-999., -999.};
    Float_t ePTT = -999.;

    Rhadd = get_Rhad(histEcal_ETT, histHcal_ETT);
    wEta22 = get_wEta2(Ecal_binCenter_x, Ecal_binCenter_y, histEcal_EE);
    Rphii = get_Rphi(Ecal_binCenter_x, Ecal_binCenter_y, histEcal_EE);
    Retaa = get_Reta(Ecal_binCenter_x, Ecal_binCenter_y, histEcal_EE);
    sigmaEtaEtaa = get_sigmaEtaEta(Ecal_binCenter_x, Ecal_binCenter_y, histEcal_EE);
    DRss = get_DRs(histEcal_EE);
    DeltaR033 = DRss.at(0);
    DeltaR044 = DRss.at(1);
    
    h11->Fill(Rhadd);
    h22->Fill(wEta22);
    h33->Fill(Rphii);
    h44->Fill(Retaa);
    h55->Fill(sigmaEtaEtaa);
    h66->Fill(DeltaR033);
    h77->Fill(DeltaR044);
    

    /************************/
    /** Store HL variables **/
    /************************/
    ofHLVar << Rhad <<","<< wEta2 <<","<< Rphi <<","<< Reta <<","<< sigmaEtaEta <<","<< DeltaR03 <<","<< DeltaR04 <<"\n";
    ofHLVarr << Rhadd <<","<< wEta22 <<","<< Rphii <<","<< Retaa <<","<< sigmaEtaEtaa <<","<< DeltaR033 <<","<< DeltaR044 <<"\n";
   
  // hs7.Add(h77);
  // Reset everything (histograms and variables)
    histEcal_E->Reset("ICESM");
    histEcal_ET->Reset("ICESM");
    histHcal_E->Reset("ICESM");
    histHcal_ET->Reset("ICESM");

    histEcal_EE->Reset("ICESM");
    histEcal_ETT->Reset("ICESM");
    histHcal_EE->Reset("ICESM");
    histHcal_ETT->Reset("ICESM");
  }


  /**********************************/
  /** Draw Histograms of Variables **/
  /**********************************/
  gStyle->SetOptStat(kFALSE);
  plotHL(true, "Entries", "", "Rhad", h1, h11);
  plotHL(true, "Entries", "", "wEta2", h2, h22);
  plotHL(true, "Entries", "", "Rphi", h3, h33);
  plotHL(true, "Entries", "", "Reta", h4, h44);
  plotHL(true, "Entries", "", "sigmaEtaEta", h5, h55);
  plotHL(true, "Entries", "", "DelR03", h6, h66);
  plotHL(true, "Entries", "", "DelR04", h7, h77);

}
