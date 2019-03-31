#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include <vector>
#include <algorithm>
#endif

//------------------------------------------------------------------------------

void finde(const char *inputFile)
{
  gSystem->Load("libDelphes");

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile);

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();

  // Get pointers to branches used in this analysis
  TClonesArray *branchECal = treeReader->UseBranch("ECalTowers");
  TClonesArray *branchHCal = treeReader->UseBranch("HCalTowers");
  TClonesArray *branchElectron = treeReader->UseBranch("Electron");

  Electron *electron, *highe, *fe;
  float curr_ePT = -999999.0;
  Tower *ECal;
  Tower *HCal;

  // Book histograms
  TH2 *histE = new TH2F("E", "ECal", 100, -0.4, 0.4, 100, -0.4, 0.4);
  TH2 *histH = new TH2F("H", "HCal", 100, -0.4, 0.4, 100, -0.4, 0.4);
 
  std::vector<float> ephi, eeta;
  std::vector<float> hphi, heta;

  // Loop over all events
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  {
    treeReader->ReadEntry(entry);
    std::cout << "Event#" << entry << ": " << std::endl;
    int enu = branchElectron->GetEntriesFast();
    std::cout << "  " << enu << " e in total" << std::endl;

    fe = (Electron*) branchElectron->At(0);
    // Selecting the electron with the highest PT
    // Though it seems that the highe will be the 1st element in branchElectron
    for(Int_t i = 0; i < enu; ++i)
    {
      electron = (Electron*) branchElectron->At(i);
      if (electron->PT >= fe->PT)
      {
        curr_ePT = electron->PT;
        highe = electron;
      }
      std::cout << "  e#" << i << ", PT=" << electron->PT << ", currPT:" << curr_ePT << std::endl;
    }

    // For events with at least one electron
    if (enu > 0)
    {
      //std::cout << "Event#: " << entry << std::endl;
      int enu = branchElectron->GetEntriesFast();
      //std::cout << "  e#: " << enu << std::endl;
      for(Int_t j = 0; j < branchECal->GetEntriesFast(); ++j)
      {
        ECal = (Tower*) branchECal->At(j);
        std::cout << "    Ecal#" << j << ", Ecal.ET: " << ECal->ET << ", Ecal.E: " << ECal->E <<  std::endl;
        for (Int_t ii = 0 ; ii < 4; ++ii)
        {
          std::cout << "      Edge#" << ii << ": " << ECal->Edges[ii];
          
        }
        std::cout << std::endl;
        //if (ECal->E == highe->PT)
        //{
        //  std::cout << "!!!!!!" << j << std::endl; 
        //}
        //std::cout << "  ECal.Phi-e.Phi: " << ECal->Phi - highe->Phi << std::endl;
        //std::cout << "  ECal.Eta-e.Eta: " << ECal->Eta - highe->Eta << std::endl;
        if (std::sqrt(std::pow(ECal->Eta-highe->Eta,2) + std::pow(ECal->Phi-highe->Phi,2)) < 0.4)
        {
          ephi.push_back(ECal->Phi - highe->Phi);
          eeta.push_back(ECal->Eta - highe->Eta);
          histE->Fill(ECal->Eta - highe->Eta, ECal->Phi - highe->Phi, ECal->ET);
        }
      }

      for(Int_t k = 0; k < branchHCal->GetEntriesFast(); ++k)
      {
        HCal = (Tower*) branchHCal->At(k);
        if (std::sqrt(std::pow(HCal->Eta-highe->Eta,2) + std::pow(HCal->Phi-highe->Phi,2)) < 0.4)
        {
          hphi.push_back(HCal->Phi - highe->Phi);
          heta.push_back(HCal->Eta - highe->Eta);
          histH->Fill(HCal->Eta - highe->Eta, HCal->Phi - highe->Phi, HCal->ET);
        }
      }
    }
  }
    
    //Stats for min max in histogram
    float maxephi = *std::max_element(std::begin(ephi), std::end(ephi));
    float minephi = *std::min_element(std::begin(ephi), std::end(ephi));
    std::cout << "Max ephi: " << maxephi << ", Min ephi: " << minephi << std::endl;
  
    float maxeeta = *std::max_element(std::begin(eeta), std::end(eeta));
    float mineeta = *std::min_element(std::begin(eeta), std::end(eeta));
    std::cout << "Max eeta: " << maxeeta << ", Min eeta: " << mineeta << std::endl;

    float maxhphi = *std::max_element(std::begin(hphi), std::end(hphi));
    float minhphi = *std::min_element(std::begin(hphi), std::end(hphi));
    std::cout << "Max hphi: " << maxhphi << ", Min hphi: " << minhphi << std::endl;
  
    float maxheta = *std::max_element(std::begin(heta), std::end(heta));
    float minheta = *std::min_element(std::begin(heta), std::end(heta));
    std::cout << "Max heta: " << maxheta << ", Min heta: " << minheta << std::endl;
    

    //Edit axis and save png
    TCanvas *c1 = new TCanvas;

    histE->GetXaxis()->SetTitle("ecal.Eta-e.Eta");
    histE->GetYaxis()->SetTitle("ecal.Phi-e.Phi");
    histE->Draw("colz");
    
    TImage *img1 = TImage::Create();
    img1->FromPad(c1);
    img1->WriteImage("hie.png");

    TCanvas *c2 = new TCanvas;

    histH->GetXaxis()->SetTitle("hcal.Eta-e.Eta");
    histH->GetYaxis()->SetTitle("hcal.Phi-e.Phi");
    histH->Draw("colz");

    TImage *img2 = TImage::Create();
    img2->FromPad(c2);
    img2->WriteImage("hih.png");

}

