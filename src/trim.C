#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include <algorithm>
#endif

void trim(const char *inputFile)
{
  gSystem->Load("libDelphes");
  TChain chain("Delphes");
  chain.Add(inputFile);

  TFile *nf = new TFile("trim.root","recreate");
  TTree *nt = chain.CopyTree("Electron.fUniqueID == 0");
  nf->Write();
}
