#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include <vector>
#include <algorithm>
#include <math.h>
#endif

// didn't find good tcl lib
//------------------------------------------------------------------------------
int verbose = 3;
class mystreambuf: public std::streambuf
{
};
mystreambuf nostreambuf;
std::ostream nocout(&nostreambuf);
#define log(x) ((x >= verbose)? std::cout : nocout)


void printV(std::vector<float> v)
{
    log(1) << "length=" << v.size() << ": ";
    for(int i=0; i<v.size(); ++i)
    {
        log(1) << v[i] << ' ';
    }
    log(1) << endl;
}

int findV(std::vector<float> v, float e)
{
    for(int i=0; i<v.size()-1; ++i)
    {
        // what if electron's eta/phi on the edges of cells?
        // since they are not in the center as Tower
        if(e>v[i] && e<v[i+1])
        {
            return i;
        }
    }
    // -1 means not found
    return -1;
}

void finde(const char *inputFile)
{
  std::ofstream fePT;
  fePT.open("ePT.csv");

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

  Electron *electron, *highe;
  
  //float curr_ePT = -999999.0;
  int jj = -1;
  int tt = 0;
  
  Tower *ECal;
  Tower *HCal;

  // Book histograms
  TH2 *histE = new TH2F("E", "ECal", 100, -0.4, 0.4, 100, -0.4, 0.4);
  TH2 *histH = new TH2F("H", "HCal", 100, -0.4, 0.4, 100, -0.4, 0.4);
 
  std::vector<float> ephi, eeta;
  std::vector<float> hphi, heta;
  // b = barrell
  std::vector<float> bephi, beeta;
  std::vector<float> bhphi, bheta;
  int total_e = 0;
  int far_e = 0;
  // add pre-configured barrell cells
  for(Int_t i=-100; i<=100; i++)
  {
    beeta.push_back(i*0.025);
  }
  for(Int_t i=-126; i<=126; i++)
  {
    bephi.push_back(i*(M_PI)/126);
  }
  for(Int_t i=-25; i<=25; i++)
  {
    bheta.push_back(i*0.1);
  }
  for(Int_t i=-31; i<=31; i++)
  {
    bhphi.push_back(i*(M_PI)/31);
  }
  printV(beeta);
  printV(bephi);
  printV(bheta);
  printV(bhphi);

  // Loop over all events
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  // for(Int_t entry = 0; entry < 2; ++entry)
  {
    treeReader->ReadEntry(entry);
    log(2) << "Event#" << entry << ": " << std::endl;
    int enu = branchElectron->GetEntriesFast();
    // total_e += enu;
    log(2) << "  " << enu << " e in total" << std::endl;

    highe = (Electron*) branchElectron->At(0);
    // Selecting the electron with the highest PT
    // Though it seems that the highe will be the 1st element in branchElectron
    for(Int_t i = 0; i < enu; ++i)
    {
      electron = (Electron*) branchElectron->At(i);
      if (electron->PT >= highe->PT)
      {
        //curr_ePT = electron->PT;
        highe = electron;
      }
      log(2) << "  e#" << i << ", PT=" << electron->PT << ", HighePT:" << highe->PT << ", Eta: " << electron->Eta << ", Phi: " << electron->Phi << std::endl;
      fePT << electron->PT << "\n";
    }
    int ii = findV(beeta,electron->Eta);
    //std::cout << "Position of e: " << ii << std::endl;
    if(ii > beeta.size()-15 || ii < 15)
    {
        far_e += 1;
        //std::cout << "The electron falls too far!!" << std::endl;
    }

    // For events with at least one electron
    if (enu > 0)
    {
      total_e += 1;
      //Initialize highest ET in tower to be the first
      Tower *highECal;
      float highECalET = -99999.0;
      for(Int_t j = 0; j < branchECal->GetEntriesFast(); ++j)
      {
        ECal = (Tower*) branchECal->At(j);
        log(1) << "    Ecal#" << j << ", Ecal.ET: " << ECal->ET << ", Ecal.E: " << ECal->E  <<  std::endl;
        for (Int_t ii = 0 ; ii < 4; ++ii)
        {
          log(1) << "      Edge#" << ii << ": " << ECal->Edges[ii];
          
        }
        log(1) << std::endl;
        log(1) <<  "      Random: " << "Ecal.Eta: " << ECal->Eta << ", Ecal.Phi: " << ECal->Phi << std::endl;

        float eceta = 0.5*(ECal->Edges[0]+ECal->Edges[1]);
        float ecphi = 0.5*(ECal->Edges[2]+ECal->Edges[3]); 
        log(1) <<  "      Center: " << "Ecal.Eta: " << eceta  << ", Ecal.Phi: " << ecphi << std::endl;
        
        // Make sure if ecal.eta is within 15 cells in the barrel
        if (15 < findV(beeta,eceta) && findV(beeta,eceta) < beeta.size()-15)
        {
            // Look for the highest ET cell
            if (ECal->ET > highECalET)
            {
                highECal = ECal;
                highECalET = ECal->ET;
                jj = findV(beeta,eceta);
            }
        }
        
        //if (ECal->E == highe->PT)
        //{
        //  std::cout << "!!!!!!" << j << std::endl; 
        //}
        //std::cout << "  ECal.Phi-e.Phi: " << ECal->Phi - highe->Phi << std::endl;
        //std::cout << "  ECal.Eta-e.Eta: " << ECal->Eta - highe->Eta << std::endl;
        
        
        if (std::sqrt(std::pow(eceta-highe->Eta,2) + std::pow(ecphi-highe->Phi,2)) < 0.4)
        {
          eeta.push_back(eceta - highe->Eta);
          ephi.push_back(ecphi - highe->Phi);
          histE->Fill(eceta - highe->Eta, ecphi - highe->Phi, ECal->ET);
        }
      }
      if(jj)
      {
        tt++;
      }
      log(3) << "High ECal: " << jj << ", eta: " << highECal->Eta << ", phi:" << highECal->Phi << ", ET:" << highECal->ET << ", Total: " << tt << std::endl;

      for(Int_t k = 0; k < branchHCal->GetEntriesFast(); ++k)
      {
        HCal = (Tower*) branchHCal->At(k);
        float hceta = 0.5*(HCal->Edges[0]+HCal->Edges[1]);
        float hcphi = 0.5*(HCal->Edges[2]+HCal->Edges[3]); 
        if (std::sqrt(std::pow(hceta-highe->Eta,2) + std::pow(hcphi-highe->Phi,2)) < 0.4)
        {
          heta.push_back(hceta - highe->Eta);
          hphi.push_back(hcphi - highe->Phi);
          histH->Fill(hceta - highe->Eta, hcphi - highe->Phi, HCal->ET);
        }
      }
    }
  }
    
    log(5) << "Electron: Total: " << total_e << ", Far: " << far_e << std::endl;
    
    //Stats for min max in histogram
    float maxephi = *std::max_element(std::begin(ephi), std::end(ephi));
    float minephi = *std::min_element(std::begin(ephi), std::end(ephi));
    log(5) << "Max ephi: " << maxephi << ", Min ephi: " << minephi << std::endl;
  
    float maxeeta = *std::max_element(std::begin(eeta), std::end(eeta));
    float mineeta = *std::min_element(std::begin(eeta), std::end(eeta));
    log(5) << "Max eeta: " << maxeeta << ", Min eeta: " << mineeta << std::endl;

    float maxhphi = *std::max_element(std::begin(hphi), std::end(hphi));
    float minhphi = *std::min_element(std::begin(hphi), std::end(hphi));
    log(5) << "Max hphi: " << maxhphi << ", Min hphi: " << minhphi << std::endl;
  
    float maxheta = *std::max_element(std::begin(heta), std::end(heta));
    float minheta = *std::min_element(std::begin(heta), std::end(heta));
    log(5) << "Max heta: " << maxheta << ", Min heta: " << minheta << std::endl;

    fePT.close();
    

    //Edit axis and save png
   // TCanvas *c1 = new TCanvas;

   // histE->GetXaxis()->SetTitle("ecal.Eta-e.Eta");
   // histE->GetYaxis()->SetTitle("ecal.Phi-e.Phi");
   // histE->Draw("colz");
   // 
   // TImage *img1 = TImage::Create();
   // img1->FromPad(c1);
   // img1->WriteImage("hie.png");

   // TCanvas *c2 = new TCanvas;

   // histH->GetXaxis()->SetTitle("hcal.Eta-e.Eta");
   // histH->GetYaxis()->SetTitle("hcal.Phi-e.Phi");
   // histH->Draw("colz");

   // TImage *img2 = TImage::Create();
   // img2->FromPad(c2);
   // img2->WriteImage("hih.png");

}

