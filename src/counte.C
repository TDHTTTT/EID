#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include <vector>
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

void counte(const char *inputFile)
{
  gSystem->Load("libDelphes");

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile);

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();

  TClonesArray *branchElectron = treeReader->UseBranch("Electron");
  int total_e = 0;

  // Loop over all events
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  {
    treeReader->ReadEntry(entry);
    int enu = branchElectron->GetEntriesFast();
    total_e += enu;
  }
  log(5) << "Total electron #: " << total_e << std::endl;
}
