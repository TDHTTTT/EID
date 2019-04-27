#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include <algorithm>
#endif

void trim(const char *inputFile)
{
  gSystem->Load("libDelphes");

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile);

  // Create object of class ExRootTreeReader
  ExRootTreeReader* treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();

  // Get pointers to branches used in this analysis
  TClonesArray *branchElectron = treeReader->UseBranch("Electron");
  
  TFile* of = new TFile(inputFile);
  //TTree* ot;
  //of->GetObject("Delphes",ot);
  TTree *ot = (TTree*)of->Get("Delphes");
  TFile *nf = new TFile("trim.root","recreate");
  // Copying the header
  TTree *nt = ot->CloneTree(0);
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  {
    treeReader->ReadEntry(entry);
    int enu = branchElectron->GetEntriesFast();
    if(enu>0)
    {
        nt->CopyEntries(ot);
    }
  }
  nf->Write();
}
