/**\class MssmHbb MssmHbbAnalyser.cc Analysis/Tools/src/MssmHbbAnalyser.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Roberval Walsh Bastos Rangel
//         Created:  Mon, 20 Oct 2014 14:24:08 GMT
//
//

// system include files
#include <iostream>
// 
// user include files
#include "Analysis/MssmHbb/interface/MssmHbbAnalyser.h"


//
// class declaration
//

using namespace analysis;
using namespace analysis::tools;
using namespace analysis::mssmhbb;

//
// constructors and destructor
//
MssmHbbAnalyser::MssmHbbAnalyser()
{
}

MssmHbbAnalyser::MssmHbbAnalyser(int argc, char ** argv) : BaseAnalyser(argc,argv), Analyser(argc,argv)
{
//   histograms("cutflow");
//   histograms("jet",config_->nJetsMin());
   
}

MssmHbbAnalyser::~MssmHbbAnalyser()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//
// ------------ method called for each event  ------------


// bool MssmHbbAnalyser::event(const int & i)
// {
//    // parent function checks only json and run range validity
//    if ( ! Analyser::event(i) ) return false;
//    
//    return true;
// }
// 
// void MssmHbbAnalyser::histograms(const std::string & obj, const int & n)
// {
// //   Analyser::histograms(obj,n);
//    
// }
// 
// void MssmHbbAnalyser::end()
// {
//    Analyser::end();
//    
// }
// 
// void MssmHbbAnalyser::fillJetHistograms()
// {
//    Analyser::fillJetHistograms();
//    
// }
// 
bool MssmHbbAnalyser::muonJet(const bool & swap)
{
   // jet with ranking 1 is the muon jet, swap with jet 2 in case it has the leading muon 
   int r1 = 1;
   int r2 = 2;
   int j1 = r1-1;
   int j2 = r2-1;
   ++ cutflow_;
   if ( std::string(h1_["cutflow"] -> GetXaxis()-> GetBinLabel(cutflow_+1)) == "" ) 
   {
      if ( swap ) h1_["cutflow"] -> GetXaxis()-> SetBinLabel(cutflow_+1,"MSSMHbb Semileptonic: Jet-muon association -> Muon-Jet index 1");
      else        h1_["cutflow"] -> GetXaxis()-> SetBinLabel(cutflow_+1,"MSSMHbb Semileptonic: Jet-muon association");
   }
   
   auto jet1 = selectedJets_[j1];
   jet1 -> addMuon(selectedMuons_);
   auto jet2 = selectedJets_[j2];
   jet2 -> addMuon(selectedMuons_);
   
   if ( ! (jet1 -> muon() || jet2 -> muon()) ) return false;
   if ( !  jet1 -> muon() && swap ) this->jetSwap(r1,r2);
   
   h1_["cutflow"] -> Fill(cutflow_,weight_);
   return true;
   
}
