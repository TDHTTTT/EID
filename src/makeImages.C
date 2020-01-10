#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#endif

void makeImages(const char *inputFile)
{

  /******************************************/
  /** Create csv files to store image data **/
  /******************************************/

  std::ofstream fEcal_E;
  fEcal_E.open("Ecal_E.csv");

  std::ofstream fEcal_ET;
  fEcal_ET.open("Ecal_ET.csv");

  std::ofstream fHcal_E;
  fHcal_E.open("Hcal_E.csv");

  std::ofstream fHcal_ET;
  fHcal_ET.open("Hcal_ET.csv");

  std::ofstream fePT;
  fePT.open("ePT.csv");

  /*********************************/
  /** Delphes preliminary actions **/
  /*********************************/

  // Load Delphes functions
  gSystem->Load("libDelphes");

  // Create chain of root trees from files
  TChain chain("Delphes");
  chain.Add(inputFile);

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);

  // Get number of entries in tree of whole file chain (number of total events in all files combined) 
  Long64_t numberOfEntries = treeReader->GetEntries();

  // Get pointers to branches used in this analysis
  TClonesArray *branchECal = treeReader->UseBranch("ECalTowers");
  TClonesArray *branchHCal = treeReader->UseBranch("HCalTowers");
  TClonesArray *branchElectron = treeReader->UseBranch("Electron");

  // Define object classes used in this analysis
  Electron *electron, *highe;
  Tower *ECal;
  Tower *HCal;

  /***********************************************************/
  /** Define all other objects to be used in the event loop **/
  /***********************************************************/

  // Constant pi
  Float_t pi = 3.14159265358979323846;

  /**---------------------------**/
  /** Histogram related objects **/
  /**---------------------------**/

  // Binning variables for ECal and HCal
  Int_t nBins_EcalPhi = 252;
  Int_t nBins_EcalEta = 200;
  Int_t tot = 0;
  Float_t binWidth_EcalPhi = pi/126.;
  Float_t binWidth_EcalEta = 0.025;

  Int_t nBins_HcalPhi = 62;
  Int_t nBins_HcalEta = 50;
  Float_t binWidth_HcalPhi = pi/31.;
  Float_t binWidth_HcalEta = 0.1;

  // Dummy Histograms: (1) Ecal for finding high PT electron's cell, (2)  Ecal filled for that event (3) Hcal filled for that event
  // x axis is eta, y is phi
  TH2 *hEle_d  = new TH2F("hEle_d",  "", nBins_EcalEta, -2.5, 2.5, nBins_EcalPhi, -pi, pi);
  TAxis *hEle_d_xAxis = hEle_d->GetXaxis();
  TAxis *hEle_d_yAxis = hEle_d->GetYaxis();

  TH2 *hECal_d = new TH2F("hECal_d", "", nBins_EcalEta, -2.5, 2.5, nBins_EcalPhi, -pi, pi);
  TAxis *hECal_d_xAxis = hECal_d->GetXaxis();
  TAxis *hECal_d_yAxis = hECal_d->GetYaxis();

  TH2 *hHCal_d = new TH2F("hHCal_d", "", nBins_HcalEta, -2.5, 2.5, nBins_HcalPhi, -pi, pi);
  TAxis *hHCal_d_xAxis = hHCal_d->GetXaxis();
  TAxis *hHCal_d_yAxis = hHCal_d->GetYaxis();

  /** Histograms for aggregate and single images **/
  // Ecal: 31x31 images cell size (eta, phi) = (x, y) = (0.025, pi/126)
  float histLowEx  = -1.*(15.5*binWidth_EcalEta);
  float histHighEx = +1.*(15.5*binWidth_EcalEta);
  float histLowEy  = -1.*(15.5*binWidth_EcalPhi);  
  float histHighEy = +1.*(15.5*binWidth_EcalPhi); 

  TH2 *histEcal_E_a = new TH2F("histEcal_E_a", "Aggregate Energy in ECal", 31, histLowEx, histHighEx, 31, histLowEy, histHighEy);
  TH2 *histEcal_ET_a = new TH2F("histEcal_ET_a", "Aggregate ET in ECal", 31, histLowEx, histHighEx, 31, histLowEy, histHighEy);
  TH2 *histEcal_E_s = new TH2F("histEcal_E_s", "Single Event Energy in ECal", 31, histLowEx, histHighEx, 31, histLowEy, histHighEy);
  TH2 *histEcal_ET_s = new TH2F("histEcal_ET_s", "Single Event ET in ECal", 31, histLowEx, histHighEx, 31, histLowEy, histHighEy);

  // Hcal realistic binning: 8x8 images cell size (eta, phi) = (x, y) = (0.1,5 pi/31)
  float histLowHx  = -1.*(4*binWidth_HcalEta);
  float histHighHx = +1.*(4*binWidth_HcalEta);
  float histLowHy  = -1.*(4*binWidth_HcalPhi);  
  float histHighHy = +1.*(4*binWidth_HcalPhi);  

  TH2 *histHcal_E_a = new TH2F("histHcal_E_a", "Aggregate Energy in HCal", 8, histLowHx, histHighHx, 8, histLowHy, histHighHy);
  TH2 *histHcal_ET_a = new TH2F("histHcal_ET_a", "Aggregate ET in HCal", 8, histLowHx, histHighHx, 8, histLowHy, histHighHy);
  TH2 *histHcal_E_s = new TH2F("histHcal_E_s", "Single Event Energy in HCal", 8, histLowHx, histHighHx, 8, histLowHy, histHighHy);
  TH2 *histHcal_ET_s = new TH2F("histHcal_ET_s", "Single Event ET in HCal", 8, histLowHx, histHighHx, 8, histLowHy, histHighHy);

  // Hcal ~same pixel size as Ecal: 32x32 images cell size (eta, phi) = (x, y) = 1/4 * (0.025, pi/126)
  TH2 *histHcal_E_a2 = new TH2F("histHcal_E_a2", "Aggregate Energy in HCal 2", 32, histLowHx, histHighHx, 32, histLowHy, histHighHy);
  TH2 *histHcal_ET_a2 = new TH2F("histHcal_ET_a2", "Aggregate ET in HCal 2", 32, histLowHx, histHighHx, 32, histLowHy, histHighHy);
  TH2 *histHcal_E_s2 = new TH2F("histHcal_E_s2", "Single Event Energy in HCal 2", 32, histLowHx, histHighHx, 32, histLowHy, histHighHy);
  TH2 *histHcal_ET_s2 = new TH2F("histHcal_ET_s2", "Single Event ET in HCal 2", 32, histLowHx, histHighHx, 32, histLowHy, histHighHy);

  /**-----------------**/
  /** Other variables **/
  /**-----------------**/

  //Global and cartesian bin identifiers for searching through histograms
  Int_t ng, nx, ny, nz = -999; 
  Int_t ng1, nx1, ny1, nz1 = -999; 
  Int_t newnx, newny = -999;

  // Number of electrons in an event
  Int_t enu = -999; 

  // Variable used to find high PT electron
  Float_t curr_ePT = -999.0;

  // High PT electron's Eta and Phi, corresponding ECal cell's ET and PT
  Float_t highET    = -999.0;
  Float_t highe_phi = -999.0;
  Float_t highe_eta = -999.0;
  Float_t highe_PT = -999.0;


  // Variables for finding highest ECal ET cell in region (+-8 cells) of electron
  Float_t currET = -999.0;
  Int_t eta_l = -999; 
  Int_t eta_h = -999; 
  Int_t phi_l = -999; 
  Int_t phi_h = -999; 
  Int_t I     = -999; 

  // High ET cell in region (+-8 cells) of electron
  Float_t newhighe_phi = -999.0;
  Float_t newhighe_eta = -999.0;
  Float_t newhighET    = -999.0;

  // Center of HCal cell where Ecal high ET cell falls
  Float_t eta0 = -999.0;
  Float_t phi0 = -999.0;

  // Variables for creating ECal and HCal images
  Float_t dPhi = -999.0;
  Float_t dEta = -999.0;

  Int_t quad = -999;
  Float_t highEtaRange = -999.0;
  Float_t lowEtaRange  = -999.0;
  Float_t highPhiRange = -999.0;
  Float_t lowPhiRange  = -999.0;

  /**************************/
  /** Loop over all events **/
  /**************************/
  std::cout << "Creating images for " << numberOfEntries << " events..." << std::endl;
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  {

    treeReader->ReadEntry(entry);
    //std::cout << "Event#: " << entry << std::endl;
    enu = branchElectron->GetEntriesFast();
    //std::cout << "  e#: " << enu << std::endl;

    /*---------------------------------------------*/
    /*-- Select the electron with the highest PT --*/
    /*---------------------------------------------*/

    // Reassign curr_ePT to have ridiculously low value after each event, so search works properly
    curr_ePT = -999.0; 

    for(Int_t i = 0; i < enu; ++i)
    {
      electron = (Electron*) branchElectron->At(i);
      if (electron->PT >= curr_ePT)
      {
        curr_ePT = electron->PT;
        highe = electron;
      }
    }
    highe_PT = curr_ePT;

    /*-------------------------------------------*/
    /*-- For events with at least one electron --*/
    /*-------------------------------------------*/

    if (enu > 0)
    {
      /*-- Find highest ET cell around highe's position --*/

      // Reset bin identifiers
      ng = 0;
      nx = 0;
      ny = 0;
      nz = 0;
      ng1 = 0;
      nx1 = 0;
      ny1 = 0;
      nz1 = 0;
      newnx = 0;
      newny = 0;
      tot += 1;

      // First we need highe's cell
      // Therefore, assign highe_eta and highe_phi to be the value of the nearest cell's center
      ng = hEle_d->Fill(highe->Eta, highe->Phi, 0);
      hEle_d->GetBinXYZ(ng, nx, ny, nz);

      highe_eta = hEle_d_xAxis->GetBinCenter(nx);
      highe_phi = hEle_d_yAxis->GetBinCenter(ny);


      // Check that this is within range of the search for the highest ET cell in that region
      // Continue otherwise
      // We search in a +-0.2 (0.4x0.4 window centered on the cell) => 17x17 cell window centered on cell (8+1+8 x 8+1+8)
      // This means we must be at least 8 cells away from edge
      // |Eta| must be less than 2.5-8.*0.025
      if((nx < (8+1)) || (nx > nBins_EcalEta - (8 + 1))) 
      {
        // std::cout<<"Eta too close to edge, cannot search for highest ET cell."<<std::endl;
        hECal_d->Reset("ICESM");
        continue; 
      }


      // Find the ET of this cell in the Ecal
      // Want ET of cell that has center highe_eta, highe_phi
      // Loop over ECal and fill dummy histogram with ET, then get content of bin with center highe_eta, highe_phi

      // std::cout << "Begin searching for high ET cell " << std::endl;
      for(Int_t k = 0; k < branchECal->GetEntriesFast(); ++k)
      {
        ECal = (Tower*) branchECal->At(k);
        hECal_d ->Fill(ECal->Eta, ECal->Phi, ECal->ET);
      }

      highET = hECal_d->GetBinContent(nx, ny);


      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // NOTE on finding difference between phi coordinates
      // Dealing with modulo nature of angles
      // Ex. if one has phi=pi-epsilon the other phi= -pi + epsilon, these are actually 2epsilon apart but a normal difference would report 2pi - 2epsilon
      // Solu. Want the smaller angle difference => pi - std::abs( std::abs(ECal1.Phi - highe_phi) - pi)
      // https://gamedev.stackexchange.com/questions/4467/comparing-angles-and-working-out-the-difference
      // To preserve the correct sign of the difference, utilize trig functions
      // atan2(sin(Phi - phi_center), cos(Phi - phi_center))
      // https://stackoverflow.com/questions/1878907/the-smallest-difference-between-2-angles
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


      // Find highest ET cell in the region
      // Assign newhighe_eta and newhighe_phi to be the value of this cell's center
      // Search histogram entries +-8bins,
      newhighe_eta = highe_eta;
      newhighe_phi = highe_phi;
      newhighET = highET;
      currET = -999.;
      eta_l = nx-8;
      eta_h = nx+8;
      phi_l = ny-8;
      phi_h = ny+8;
      I = -999;

      for(Int_t i = phi_l; i <= phi_h; i++)
      {
        for(Int_t j = eta_l; j <= eta_h; j++) 
        {
          // If phi is out of bounds, wrap around because modular
          if(i < 1)
          {
            I = i + nBins_EcalPhi;
          }
          else if(i > nBins_EcalPhi)
          {
            I = i - nBins_EcalPhi;
          }
          else
          {
            I = i;
          }

          currET = hECal_d->GetBinContent(j, I);

          if(currET > newhighET)
          {
            newhighET = currET;
            newhighe_eta = hECal_d_xAxis->GetBinCenter(j);
            newhighe_phi = hECal_d_yAxis->GetBinCenter(I);
            newny = I;
            newnx = j;
          }
        }
      } 

      // Check that the new eta is within range
      // Continue otherwise
      if((newnx < (15+1)) || (newnx > nBins_EcalEta - (15 + 1)))
      {
        // std::cout<<"Eta too close to edge, cannot create image."<<std::endl;
        hECal_d->Reset("ICESM");
        continue;
      }

      /** Now (finally) create the ECal and HCal images **/
      dPhi = -999.;
      dEta = -999.;

      // Loop over Ecal Tower entries
      for(Int_t j = 0; j < branchECal->GetEntriesFast(); ++j)
      {
        ECal = (Tower*) branchECal->At(j);
        // std::cout << "  Ecal#: " << j << std::endl;
        // std::cout << "  Ecal.ET: " << ECal->ET << std::endl;
        // std::cout << "  e.PT: " << highe->PT << std::endl;
        // std::cout << "  ECal.Phi-e.Phi: " << ECal->Phi - highe->Phi << std::endl;
        // std::cout << "  ECal.Eta-e.Eta: " << ECal->Eta - highe->Eta << std::endl;

        dEta = ECal->Eta - newhighe_eta;
        dPhi = atan2(sin(ECal->Phi - newhighe_phi), cos(ECal->Phi - newhighe_phi));

        // Check Eta is within image range
        if(std::abs(dEta) < (15.5*binWidth_EcalEta))
        {

          // Check Phi is within image range
          if(std::abs(dPhi) < (15.5*binWidth_EcalPhi))
          {
            histEcal_E_a->Fill( dEta, dPhi, ECal->E);
            histEcal_E_s->Fill( dEta, dPhi, ECal->E);
            histEcal_ET_a->Fill(dEta, dPhi, ECal->ET);
            histEcal_ET_s->Fill(dEta, dPhi, ECal->ET);
          }
        }
      }

      // Loop over Hcal Tower entries
      for(Int_t k = 0; k < branchHCal->GetEntriesFast(); ++k)
      {
        HCal = (Tower*) branchHCal->At(k);


        // Find which cell newhighe_eta and newhighe_phi are near
        // Fill dummy histo and get bins
        // Note: Really only need to do this once, fix this later
        ng1 = hHCal_d->Fill(newhighe_eta, newhighe_phi, 0);
        hHCal_d->GetBinXYZ(ng1, nx1, ny1, nz1);

        eta0 = hHCal_d_xAxis->GetBinCenter(nx1);
        phi0 = hHCal_d_yAxis->GetBinCenter(ny1);

        // Figure out which quadrant it's in to figure out which HCal cells to include
        // Assign maximum allowed deviation from phi0, eta0
        //
        //          +phi
        //            |
        //      4     |    1
        //            |
        //-eta---(phi0,eta0)--- +eta
        //            |
        //      3     |    2
        //            |
        //          -phi
        //
        quad = -999;
        highEtaRange = -999.;
        lowEtaRange = -999.;
        highPhiRange = -999.;
        lowPhiRange = -999.;

        if (highe_eta>eta0 && highe_phi>phi0)
        {
          quad = 1;

          highPhiRange = + 4.5*binWidth_HcalPhi;
          lowPhiRange  = - 3.5*binWidth_HcalPhi;

          highEtaRange = + 4.5*binWidth_HcalEta;
          lowEtaRange  = - 3.5*binWidth_HcalEta;

        }
        if (highe_eta>eta0 && highe_phi<phi0)
        {
          quad = 2;

          highPhiRange = + 3.5*binWidth_HcalPhi;
          lowPhiRange  = - 4.5*binWidth_HcalPhi;

          highEtaRange = + 4.5*binWidth_HcalEta;
          lowEtaRange  = - 3.5*binWidth_HcalEta;

        }
        if (highe_eta<eta0 && highe_phi<phi0)
        {
          quad = 3;

          highPhiRange = + 3.5*binWidth_HcalPhi;
          lowPhiRange  = - 4.5*binWidth_HcalPhi;

          highEtaRange = + 3.5*binWidth_HcalEta;
          lowEtaRange  = - 4.5*binWidth_HcalEta;

        }
        if (highe_eta<eta0 && highe_phi>phi0)
        {
          quad = 4;

          highPhiRange = + 4.5*binWidth_HcalPhi;
          lowPhiRange  = - 3.5*binWidth_HcalPhi;

          highEtaRange = + 3.5*binWidth_HcalEta;
          lowEtaRange  = - 4.5*binWidth_HcalEta;

        }


        dEta = HCal->Eta - eta0;
        dPhi = atan2(sin(HCal->Phi - phi0), cos(HCal->Phi - phi0));

        // Check Eta is within image range
        if(dEta < highEtaRange && dEta > lowEtaRange)
        {
          // Check Phi is within image range
          if(dPhi < highPhiRange && dPhi > lowPhiRange)
          {
            histHcal_E_a->Fill( dEta, dPhi, HCal->E);
            histHcal_E_s->Fill( dEta, dPhi, HCal->E);
            histHcal_ET_a->Fill(dEta, dPhi, HCal->ET);
            histHcal_ET_s->Fill(dEta, dPhi, HCal->ET);
          }
        }
      }

      /*-- Convert Hcal histogram to have same pixel size --*/

      // Original size is 8x8, want to turn this into 32x32 but keep same content
      // Loop over new histogram cells, fill each 8x8 block witih 1/16th the value from histH. 
      // Thus each new pixel has 1/16th the value that the old larger pixel did so the sum of energy and ET is the same
      // Other option would be to fill each cell with the original content 
      // Note bin=0 is underflow, bin = 33 is overflow
      for(Int_t i=1; i<=32; i++)
      {
        for(Int_t j=1; j<=32; j++)
        {
          histHcal_E_a2->SetBinContent( i, j,  (1./16.)*histHcal_E_a->GetBinContent( (int(i-1)/4 + 1), (int(j-1)/4 + 1) ) );
          histHcal_E_s2->SetBinContent( i, j,  (1./16.)*histHcal_E_s->GetBinContent( (int(i-1)/4 + 1), (int(j-1)/4 + 1) ) );
          histHcal_ET_a2->SetBinContent( i, j, (1./16.)*histHcal_ET_a->GetBinContent( (int(i-1)/4 + 1), (int(j-1)/4 + 1) ) );
          histHcal_ET_s2->SetBinContent( i, j, (1./16.)*histHcal_ET_s->GetBinContent( (int(i-1)/4 + 1), (int(j-1)/4 + 1) ) );
        }
      }

      /*-- Write single images to csv files and clear histogram content for next event --*/
      // ECal images
      for(Int_t j=31; j>=1; j--)
      {
        for(Int_t i=1; i<=31; i++)
        {
          fEcal_E << histEcal_E_s->GetBinContent(i,j);
          fEcal_ET << histEcal_ET_s->GetBinContent(i,j);

          // For all entries except the one at the very end of the line, namely j=1, i=31, add a comma and a space to separate
          // if( ( j!=1 ) || (i != 31) ) // Logical inverse of (j==1 and i==31) = (j!=1 or i!=31) one of DeMorgan's laws
          if( !(i==31 && j==1) )
          {
            fEcal_E <<",";
            fEcal_ET <<",";
          }

        }
      }

      // Go to new line and clear single histogram content
      fEcal_E << "\n";
      fEcal_ET << "\n";
      fePT << highe_PT << "\n";
      histEcal_E_s->Reset("ICESM");
      histEcal_ET_s->Reset("ICESM");

      // HCal images
      for(Int_t j=32; j>=1; j--)
      {
        for(Int_t i=1; i<=32; i++)
        {
          fHcal_E << histHcal_E_s2->GetBinContent(i,j); 
          fHcal_ET << histHcal_ET_s2->GetBinContent(i,j);

          // For all entries except the one at the very end of the line, namely j=1, i=32, add a comma and a space to separate                                      
          //if( (j!=1) || (i!=32) ) // Logical inverse of (j==1 and i==32) = (j!=1 or i!=32) one of DeMorgan's laws        
          if( !(i==32 && j==1) )
          {
            fHcal_E <<",";
            fHcal_ET <<",";
          }
        }
      }
      // Go to new line in file (new event) and clear single histogram content
      fHcal_E << "\n";
      fHcal_ET << "\n";

      histHcal_E_s->Reset("ICESM");
      histHcal_ET_s->Reset("ICESM");
      histHcal_E_s2->Reset("ICESM"); 
      histHcal_ET_s2->Reset("ICESM"); 

      // Clear content of histogram used in highET cell search
      hECal_d->Reset("ICESM");

    } // End of if number of electrons>0
  } // End event loop


  /*----------------------------------------------*/
  /* Plot Histograms and Save Histogram content --*/
  /*----------------------------------------------*/

  /** Close files **/
  fEcal_E.close();
  fEcal_ET.close();
  fHcal_E.close();
  fHcal_ET.close();
  fePT.close();

  /** Edit axis and save aggregate png **/

  // Ecal_E
  TCanvas *cEcal_E = new TCanvas;
  histEcal_E_a->GetXaxis()->SetTitle("Ecal Eta - High ET Eta");
  histEcal_E_a->GetYaxis()->SetTitle("Ecal Phi - High ET Phi");
  histEcal_E_a->Draw("colz");

  TImage *imgEcal_E = TImage::Create();
  imgEcal_E->FromPad(cEcal_E);
  imgEcal_E->WriteImage("Ecal_E.png");


  // Ecal_ET
  TCanvas *cEcal_ET = new TCanvas;
  histEcal_ET_a->GetXaxis()->SetTitle("Ecal Eta - High ET Eta");
  histEcal_ET_a->GetYaxis()->SetTitle("Ecal Phi - High ET Phi");
  histEcal_ET_a->Draw("colz");

  TImage *imgEcal_ET = TImage::Create();
  imgEcal_ET->FromPad(cEcal_ET);
  imgEcal_ET->WriteImage("Ecal_ET.png");


  // Hcal_E                             
  TCanvas *cHcal_E = new TCanvas;
  histHcal_E_a->GetXaxis()->SetTitle("Hcal Eta - High ET Eta");
  histHcal_E_a->GetYaxis()->SetTitle("Hcal Phi - High ET Phi");
  histHcal_E_a->Draw("colz");

  TImage *imgHcal_E = TImage::Create();
  imgHcal_E->FromPad(cHcal_E);
  imgHcal_E->WriteImage("Hcal_E.png");


  // Hcal_ET
  TCanvas *cHcal_ET = new TCanvas;
  histHcal_ET_a->GetXaxis()->SetTitle("Hcal Eta - High ET Eta");
  histHcal_ET_a->GetYaxis()->SetTitle("Hcal Phi - High ET Phi");
  histHcal_ET_a->Draw("colz");

  TImage *imgHcal_ET = TImage::Create();
  imgHcal_ET->FromPad(cHcal_ET);
  imgHcal_ET->WriteImage("Hcal_ET.png");

  std::cout << "Done! There are " << tot << " electron events."<< std::endl;
}
