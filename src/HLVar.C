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

void HLVar(){

  /*******************************************/
  /** Create csv files to store HL var data **/
  /*******************************************/

  std::ofstream ofHLVar;
  ofHLVar.open("HLVar.csv");

  /******************************************/
  /** Declare input csv files file streams **/
  /******************************************/

  // For reference: http://www.cplusplus.com/reference/iostream/ifstream/
  std::ifstream infEcal_E  ("Ecal_E.csv");
  std::ifstream infEcal_ET ("Ecal_ET.csv");
  std::ifstream infHcal_E  ("Hcal_E.csv");
  std::ifstream infHcal_ET ("Hcal_ET.csv");

  // Define size of each row (number of elements)
  // Define an array to keep elements in         
  const Int_t sizeEcal = 31*31;                        
  const Int_t sizeHcal = 32*32;                        
  Float_t imEcal_E[sizeEcal];
  Float_t imEcal_ET[sizeEcal];
  Float_t imHcal_E[sizeHcal];
  Float_t imHcal_ET[sizeHcal];

  // Define iterators, line, and value variables 
  Int_t I    = 0;
  Int_t iee  = 0; // Ecal E
  Int_t ieet = 0; // Ecal ET
  Int_t ihe  = 0;
  Int_t ihet = 0;
  string valEcal_E,  valEcal_ET,  valHcal_E,  valHcal_ET;
  string lineEcal_E, lineEcal_ET, lineHcal_E, lineHcal_ET;

  // Define HL variables
  Float_t Rhad = -999.;
  Float_t wEta2 = -999.;
  Float_t Rphi = -999.;
  Float_t Reta = -999.;
  Float_t sigmaEtaEta = -999.;
  Float_t DeltaR03 = -999.;
  Float_t DeltaR04 = -999.;

  // Define misc. variables used in HL variable calculations
  Float_t sumH_ET = -999.;
  Float_t sumH_E = -999.;
  Float_t sumE_ET = -999;
  Int_t   Ecal_binCenter_x = -999;
  Int_t   Ecal_binCenter_y = -999;
  Float_t expEta2 = -999;
  Float_t expEta = -999;
  Float_t sumE = -999;
  Float_t relEta = -999;  
  Float_t sumE3x3 = -999;
  Float_t sumE3x7 = -999;
  Float_t sumE7x7 = -999;
  Float_t wExpEta2 = -999;
  Float_t sumW = -999;
  Float_t dEta = -999;
  Float_t dPhi = -999;

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

  TH2 *histEcal_E = new TH2F("histEcal_E", "Single Event Energy in ECal", 31, histLowEx, histHighEx, 31, histLowEy, histHighEy);
  TAxis *histEcal_E_xAxis = histEcal_E->GetXaxis();
  TAxis *histEcal_E_yAxis = histEcal_E->GetYaxis();
  
  TH2 *histEcal_ET = new TH2F("histEcal_ET", "Single Event ET in ECal", 31, histLowEx, histHighEx, 31, histLowEy, histHighEy);
  TAxis *histEcal_ET_xAxis = histEcal_ET->GetXaxis();
  TAxis *histEcal_ET_yAxis = histEcal_ET->GetYaxis();

  Float_t binWidth_HcalPhi = pi/31.;
  Float_t binWidth_HcalEta = 0.1;

  Float_t histLowHx  = -1.*(4*binWidth_HcalEta);
  Float_t histHighHx = +1.*(4*binWidth_HcalEta);
  Float_t histLowHy  = -1.*(4*binWidth_HcalPhi);  
  Float_t histHighHy = +1.*(4*binWidth_HcalPhi);

  // Hcal ~same pixel size as Ecal: 32x32 images cell size (eta, phi) = (y, x) = 1/4 * (0.025, pi/126)                       
  TH2 *histHcal_E = new TH2F("histHcal_E", "Single Event Energy in HCal 2", 32, histLowHx, histHighHx, 32, histLowHy, histHighHy);
  TH2 *histHcal_ET = new TH2F("histHcal_ET", "Single Event ET in HCal 2", 32, histLowHx, histHighHx, 32, histLowHy, histHighHy);

  // 1D histograms of variables
  TH1 *h1 = new TH1F("h1", "Rhad", 100, 0., 100.);
  TH1 *h2 = new TH1F("h2", "wEta2", 100, 0., 0.03);
  TH1 *h3 = new TH1F("h3", "Rphi", 100, 0., 1.2);
  TH1 *h4 = new TH1F("h4", "Reta", 100, 0., 1.2);
  TH1 *h5 = new TH1F("h5", "sigmaEtaEta", 100, 0., 20.);
  TH1 *h6 = new TH1F("h6", "DelR03", 100, 0., 200.);
  TH1 *h7 = new TH1F("h7", "DelR04", 100, 0., 200.);

  /**********************/
  /** Go through files **/
  /**********************/
  // Reference: http://www.cplusplus.com/reference/ios/ios/good/
  while ( infEcal_E.good() && infEcal_ET.good() && infHcal_E.good() && infHcal_ET.good()) 
    {
      // Reset iterators
      iee  = 0;
      ieet = 0;
      ihe  = 0;
      ihet = 0;

      // Reset arrays?

      // Get line from file (each line is a 31x31 (32x32) image => vector will have length 961 (1024))
      getline(infEcal_E, lineEcal_E);
      getline(infEcal_ET, lineEcal_ET);
      getline(infHcal_E, lineHcal_E);
      getline(infHcal_ET, lineHcal_ET);

      // Get string stream
      stringstream see(lineEcal_E);
      stringstream seet(lineEcal_ET);
      stringstream she(lineHcal_E);
      stringstream shet(lineHcal_ET);

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

      /**---------------------------**/
      /** Store images in histogram **/
      /**---------------------------**/
      I = 0;
      for(Int_t j=31; j>=1; j--)
	{
	  for(Int_t i=1; i<=31; i++)
	    {
	      histEcal_E->SetBinContent(i,j, imEcal_E[I]);
	      histEcal_ET->SetBinContent(i,j,imEcal_ET[I]);
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
	      I++;
            }
        }

      /**------------------------**/
      /** Calculate HL variables **/
      /**------------------------**/

      /**  Rhad **/
      Rhad = -999.;
      sumH_ET = 0.;
      sumH_E = 0.; 
      sumE_ET = 0.;
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
	      sumH_E += histHcal_E->GetBinContent(i,j);
	    }
	}
      
      Rhad = sumH_ET/sumE_ET;
      
      /*----------------------------------------------------------------------------------*/
      
      /** Get center bin of Ecal for next few variables **/
      Ecal_binCenter_x = 16; // (31/2)+0.5
      Ecal_binCenter_y = Ecal_binCenter_x; // Since image is square
      
      /*----------------------------------------------------------------------------------*/

      /**  wEta2 **/
      wEta2 = 0.;
      expEta2 = 0.;
      expEta = 0.;
      sumE = 0.;
      relEta = 0;
      
      // Sum in (eta, phi) = (3x5) window
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
	
      /*----------------------------------------------------------------------------------*/

      /**  Rphi **/
      Rphi = 0.;
      sumE3x3 = 0.;      // Sum in (eta, phi) = (3x3) window
      sumE3x7 = 0.;      // Sum in (eta, phi) = (3x7) window
      
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
	
      /*----------------------------------------------------------------------------------*/

      /**  Reta **/
      Reta = 0.;
      sumE3x7 = 0.;      // Sum in (eta, phi) = (3x3) window
      sumE7x7 = 0.;      // Sum in (eta, phi) = (3x7) window
      
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
	
      /*----------------------------------------------------------------------------------*/

      /**  sigmaEtaEta **/
      sigmaEtaEta = 0.;
      wExpEta2 = 0;
      sumW = 0.;
	
      // Sum in (eta, phi) = (5x5) window
      for(Int_t j=Ecal_binCenter_y-2; j<= Ecal_binCenter_y+2; j++)
	{
	  for(Int_t i=Ecal_binCenter_x-2; i<=Ecal_binCenter_x+2; i++)
	    {
	      
	      if(histEcal_E->GetBinContent(i,j) == 0.) continue;
	      
	      sumW += std::abs(std::log(histEcal_E->GetBinContent(i,j)));
	      wExpEta2 += std::abs(std::log(histEcal_E->GetBinContent(i,j))) * std::pow(float(i),2); 
	      // Formula is (eta_i - etabar)**2 in units of crystals(cells), so rel eta measured in # of cells = i  (assuming mean is center)
	    }
	}
      sigmaEtaEta = std::sqrt( wExpEta2/sumW );
	       
      /*----------------------------------------------------------------------------------*/
      
      /**  DeltaR **/
      DeltaR03 = 0.;
      DeltaR04 = 0.;
      dEta = 0.;
      dPhi = 0.;
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
	
      /**--------------------**/
      /** Store HL variables **/
      /**--------------------**/
      ofHLVar << Rhad <<","<< wEta2 <<","<< Rphi <<","<< Reta <<","<< sigmaEtaEta <<","<< DeltaR03 <<","<< DeltaR04 <<"\n";
      
      // Fill variable histograms
      h1->Fill(Rhad);
      h2->Fill(wEta2);
      h3->Fill(Rphi);
      h4->Fill(Reta);
      h5->Fill(sigmaEtaEta);
      h6->Fill(DeltaR03);
      h7->Fill(DeltaR04);
      
      // Reset everything (histograms and variables)
      histEcal_E->Reset("ICESM");
      histEcal_ET->Reset("ICESM");
      histHcal_E->Reset("ICESM");
      histHcal_ET->Reset("ICESM");
    }

  /**********************************/
  /** Draw Histograms of Variables **/
  /**********************************/
  TCanvas *c1 = new TCanvas();
  h1->Draw();
  TCanvas *c2 = new TCanvas();
  h2->Draw();
  TCanvas *c3 = new TCanvas();
  h3->Draw();
  TCanvas *c4 = new TCanvas();
  h4->Draw();
  TCanvas *c5 = new TCanvas();
  h5->Draw();
  TCanvas *c6 = new TCanvas();
  h6->Draw();
  TCanvas *c7 = new TCanvas();
  h7->Draw();
}
