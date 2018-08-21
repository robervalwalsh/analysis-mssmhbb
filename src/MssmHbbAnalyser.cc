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

MssmHbbAnalyser::MssmHbbAnalyser(int argc, char ** argv) : Analyser(argc,argv)
{
   histograms("jet",config_->njetsmin_);
   
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


bool MssmHbbAnalyser::event(const int & i)
{
   if ( ! Analyser::event(i) ) return false;
   
   int n = config_->njetsmin_;
   
   for ( int j = 0; j < n; ++j )
   {
      h1_[Form("pt_jet%d",j+1)] -> Fill(selectedJets_[j]->pt());
      h1_[Form("eta_jet%d",j+1)] -> Fill(selectedJets_[j]->eta());
      h1_[Form("btag_jet%d",j+1)] -> Fill(btag(*selectedJets_[j],config_->btagalgo_));
      for ( int k = j+1; k < n && j < n; ++k )
      {
         float deltaR = selectedJets_[j]->deltaR(*selectedJets_[k]);
         h1_[Form("dr_jet%d%d",j+1,k+1)]    -> Fill(deltaR);
         float deltaEta = fabs(selectedJets_[j]->eta() - selectedJets_[k]->eta());
         h1_[Form("deta_jet%d%d",j+1,k+1)]  -> Fill(deltaEta);
         float m = (selectedJets_[j]->p4()+selectedJets_[k]->p4()).M();
         if ( !config_->signalregion_ )
         {
            h1_[Form("m_jet%d%d",j+1,k+1)]  -> Fill(m);
         }
      }
   }
   
   return true;
}

bool MssmHbbAnalyser::selectionJet()
{
   if ( ! Analyser::selectionJet() ) return false;
   
   bool isgood = true;
   
   // jet kinematics and btag
   std::map<std::string,bool> isOk;
   for ( int j = 0; j < config_->njetsmin_ ; ++j )
   {
      for ( int k = j+1; k < config_->njetsmin_ && j < config_->njetsmin_; ++k )
      {
         isOk[Form("dr%d%d",j,k)]   = true;
         isOk[Form("deta%d%d",j,k)] = true;
      }
   }
   // kinematic selection
   for ( int j = 0 ; j < config_->njetsmin_ ; ++j )
   {
      // delta R between jets
      for ( int k = j+1; k < config_->njetsmin_ && j < config_->njetsmin_; ++k )
         if ( selectedJets_[j]->deltaR(*selectedJets_[k]) < config_->drmin_ )                                            isOk[Form("dr%d%d",j,k)]   = false;
   }
   // delta eta 2 leading jets
   if ( config_->njetsmin_ > 1 )
      if ( fabs(selectedJets_[0]->eta() - selectedJets_[1]->eta()) > config_->detamax_ && !(config_->detamax_ < 0) )     isOk[Form("deta%d%d",0,1)] = false;
   
   for ( auto & ok : isOk )
      isgood = ( isgood && ok.second );
   
   return isgood;
}

bool MssmHbbAnalyser::selectionBJet()
{
   bool isgood = true;
   
   if ( config_->nbjetsmin_ < 3 )
   {
      std::cout << "*** warning ***" << std::endl;
      std::cout << "   This analysis require at least 3 b-tagged jets." << std::endl;
      std::cout << "   Event *not* selected!" << std::endl;
      return false;
   }
   
   if ( selectedJets_.size() == 0 ) isgood = (isgood && selectionJet());
   
   if ( !isgood || (int)selectedJets_.size() < config_->nbjetsmin_ ) return false;
   
   // jet kinematics and btag
   std::map<std::string,bool> isOk;
   for ( int j = 0; j < config_->nbjetsmin_ ; ++j )
   {
      isOk[Form("btag%d",j)]  = true; 
   }

   // kinematic and btag selection
   for ( int j = 0 ; j < config_->nbjetsmin_ ; ++j )
   {
      if ( ! config_->signalregion_ && j == (config_->nonbtagjet_-1) )
      {
         if ( btag(*selectedJets_[j],config_->btagalgo_) > config_->nonbtagwp_ ) isOk[Form("btag%d",j)]     = false;
         continue;
      }
      if ( btag(*selectedJets_[j],config_->btagalgo_) < config_->jetsbtagmin_[j]   && !(config_->jetsbtagmin_[j] < 0) ) isOk[Form("btag%d",j)]     = false;
   }
   
   for ( auto & ok : isOk )
      isgood = ( isgood && ok.second );
   
   if ( ! isgood ) return false;

   
   return isgood;
}


void MssmHbbAnalyser::histograms(const std::string & obj, const int & n)
{
   Analyser::histograms(obj,n);
   
}
