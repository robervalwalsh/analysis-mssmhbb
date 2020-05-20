/**\class MssmHbb MssmHbbAnalyser.cc Analysis/Tools/src/MssmHbbAnalyser.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Roberval Walsh
//         Created:  Mon, 20 Oct 2014 14:24:08 GMT
//
//

// system include files
#include <iostream>
// 
// user include files
#include "Analysis/MssmHbb/interface/MssmHbbAnalyser.h"
#include "Analysis/Tools/interface/Composite.h"


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
   do_tree_ = config_->doTree();
   if ( do_tree_ ) this -> mssmHbbTree();
   do_ext_tree_ = false;
   mbb_ = -1.;
   this->jetHistograms(config_->nJetsMin(),"final_selection");
   h1_["mssmhbb_mbb"] = std::make_shared<TH1F>("mbb","MSSM Hbb mbb", 30000,0,3000);   
}

MssmHbbAnalyser::~MssmHbbAnalyser()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
   
   if ( do_tree_ ) mssmhbb_tree_ -> Write();
}


//
// member functions
//
// ------------ method called for each event  ------------
bool MssmHbbAnalyser::event(const int & i)
{
   // parent function checks only json and run range validity
   if ( ! Analyser::event(i) ) return false;
   
// PILEUP RE-WEIGHT
   this->actionApplyPileupWeight(); 
   
   return true;
}
bool MssmHbbAnalyser::preselection()
{
// IDENTIFICATIONS
      if ( ! this->selectionMuonId()         )   return false;
      if ( ! this->selectionJetId()          )   return false;
      if ( ! this->selectionJetPileupId()    )   return false;
      return true;
      
}
bool MssmHbbAnalyser::jetCorrections()
{
// CORRECTIONS
   // b energy regression
      if ( this->config()->bRegression() )  this->actionApplyBjetRegression();
   // Jet energy resolution smearing
      this->actionApplyJER();
      
      return true;
      
}
bool MssmHbbAnalyser::triggerSelection()
{
// TRIGGER selection
      if ( ! this->selectionHLT()           )   return false;      
      if ( ! this->selectionL1 ()           )   return false;  // to be used mainly in case of "OR" of seeds
      return true;
      
}
bool MssmHbbAnalyser::jetSelection()
{
      if ( ! this->selectionNJets()         )   return false;
      for ( int i = 1; i <= config_->nJetsMin(); ++i )
      {
         // kinematics selection
         if ( ! this->selectionJet(i)          )   return false;
         if ( i <= 2  && (! this->onlineJetMatching(i))     )   return false;
         // delta R selection
         for ( int j = i+1; j <= config_->nJetsMin(); ++j )
               if ( ! this->selectionJetDr(i,j)      )   return false;
      }
      // delta eta jet selection
      if ( ! this->selectionJetDeta(1,2)    )   return false;
      
      return true;
      
}
bool MssmHbbAnalyser::btagSelection()
{
      for ( int i = 1; i <= config_->nBJetsMin(); ++i )
      {
         if ( ! this->selectionBJet(i)      )   return false;
         this->actionApplyBtagSF(i);
         if ( i<= 2 && ! this->onlineBJetMatching(i)    )   return false;
      }
      
      return true;
      
}
bool MssmHbbAnalyser::muonSelection()
{
      if ( ! this->selectionNMuons()        )   return false;
      if ( ! this->selectionMuons()         )   return false;
      if ( ! this->onlineMuonMatching()     )   return false;
      return true;
      
}
bool MssmHbbAnalyser::endSelection()
{
      this->fillJetHistograms("final_selection");
      this->fillMssmHbbHistograms();
      this->fillMssmHbbTree();
      return true;
      
}

bool MssmHbbAnalyser::muonJet(const bool & swap)
{
   // jet 1 is the muon jet, swap with jet 2 in case jet1 does not have a muon 
   // Muon jet association, a muon can be in either of the two leading jets.
   // If argument is false(default, the jet label follows the jet ranking, regardless of where the muon is
   // If argument is true, the jet label 1 refers to the muon-jet: *** the order of the workflow may matter!!! ***
   if ( ! muonsanalysis_ || ! jetsanalysis_ ) return true;  // will skip this selection
   
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
   
   if ( selectedJets_.size() < 2 ||  selectedMuons_.size() < 1 ) return false;
   
   auto jet1 = selectedJets_[j1];
   jet1 -> addMuon(selectedMuons_);
   auto jet2 = selectedJets_[j2];
   jet2 -> addMuon(selectedMuons_);
   
   if ( ! (jet1 -> muon() || jet2 -> muon()) ) return false;
   if ( !  jet1 -> muon() && swap ) this->jetSwap(r1,r2);
   
   h1_["cutflow"] -> Fill(cutflow_,weight_);
   return true;
   
}

void MssmHbbAnalyser::fillMssmHbbTree()
{
   if ( ! do_tree_ ) return;
   ++ cutflow_;
   if ( std::string(h1_["cutflow"] -> GetXaxis()-> GetBinLabel(cutflow_+1)) == "" ) 
      h1_["cutflow"] -> GetXaxis()-> SetBinLabel(cutflow_+1,"Fill MssmHbb tree");
   
   Composite<Jet,Jet> c_12(*(selectedJets_[0]),*(selectedJets_[1]));
   if ( config_->isMC() || !config_->signalRegion() ) mbb_ = c_12.m();
   
   mbbw_ = weight_;
   
   if ( do_ext_tree_ )
   {
      ptbb_ = c_12.pt();
      etabb_ = c_12.eta();
      phibb_ = c_12.phi();
      ebb_ = c_12.e();
      
      ptj1_ = selectedJets_[0]->pt();
      etaj1_ = selectedJets_[0]->eta();
      phij1_ = selectedJets_[0]->phi();
      ej1_ = selectedJets_[0]->e();
      mj1_ = selectedJets_[0]->m();
     
      ptj2_ = selectedJets_[1]->pt();
      etaj2_ = selectedJets_[1]->eta();
      phij2_ = selectedJets_[1]->phi();
      ej2_ = selectedJets_[1]->e();
      mj2_ = selectedJets_[1]->m();
     
      if ( selectedJets_.size() > 2 )
      {
         ptj3_ = selectedJets_[2]->pt();
         etaj3_ = selectedJets_[2]->eta();
         phij3_ = selectedJets_[2]->phi();
         ej3_ = selectedJets_[2]->e();
         mj3_ = selectedJets_[2]->m();
      }
      
      
      if ( selectedJets_.size() > 3 )
      {
         ptj4_ = selectedJets_[3]->pt();
         etaj4_ = selectedJets_[3]->eta();
         phij4_ = selectedJets_[3]->phi();
         ej4_ = selectedJets_[3]->e();
         mj4_ = selectedJets_[3]->m();
      }
   }
  
   
   mssmhbb_tree_ -> Fill();
   h1_["cutflow"] -> Fill(cutflow_,weight_);

}

void MssmHbbAnalyser::mssmHbbTree()
{
//   do_tree_ = true;
   this->output()->cd();
   mssmhbb_tree_ = std::make_shared<TTree>("mssmhbb","TTree with mbb and weight for FitModel");
   mssmhbb_tree_ -> Branch("mbb",&mbb_,"mbb/D");   
   mssmhbb_tree_ -> Branch("weight",&mbbw_,"weight/D");
}

void MssmHbbAnalyser::mssmHbbTreeExtended()
{
   if ( ! do_tree_ ) return;
   do_ext_tree_ = true;
   this->output()->cd();
   mssmhbb_tree_ -> Branch("ptbb",&ptbb_,"ptbb/D");
   mssmhbb_tree_ -> Branch("etabb",&etabb_,"etabb/D");
   mssmhbb_tree_ -> Branch("phibb",&phibb_,"phibb/D");
   mssmhbb_tree_ -> Branch("ebb",&ebb_,"ebb/D");
//
   mssmhbb_tree_ -> Branch("mj1",&mj1_,"mj1/D");
   mssmhbb_tree_ -> Branch("ptj1",&ptj1_,"ptj1/D");
   mssmhbb_tree_ -> Branch("etaj1",&etaj1_,"etaj1/D");
   mssmhbb_tree_ -> Branch("phij1",&phij1_,"phij1/D");
   mssmhbb_tree_ -> Branch("ej1",&ej1_,"ej1/D");
//
   mssmhbb_tree_ -> Branch("mj2",&mj2_,"mj2/D");
   mssmhbb_tree_ -> Branch("ptj2",&ptj2_,"ptj2/D");
   mssmhbb_tree_ -> Branch("etaj2",&etaj2_,"etaj2/D");
   mssmhbb_tree_ -> Branch("phij2",&phij2_,"phij2/D");
   mssmhbb_tree_ -> Branch("ej2",&ej2_,"ej2/D");
//
   mssmhbb_tree_ -> Branch("mj3",&mj3_,"mj3/D");
   mssmhbb_tree_ -> Branch("ptj3",&ptj3_,"ptj3/D");
   mssmhbb_tree_ -> Branch("etaj3",&etaj3_,"etaj3/D");
   mssmhbb_tree_ -> Branch("phij3",&phij3_,"phij3/D");
   mssmhbb_tree_ -> Branch("ej3",&ej3_,"ej3/D");
//
   mssmhbb_tree_ -> Branch("mj4",&mj4_,"mj4/D");
   mssmhbb_tree_ -> Branch("ptj4",&ptj4_,"ptj4/D");
   mssmhbb_tree_ -> Branch("etaj4",&etaj4_,"etaj4/D");
   mssmhbb_tree_ -> Branch("phij4",&phij4_,"phij4/D");
   mssmhbb_tree_ -> Branch("ej4",&ej4_,"ej4/D");
   
}

void MssmHbbAnalyser::fillMssmHbbHistograms()
{
   ++ cutflow_;
   if ( std::string(h1_["cutflow"] -> GetXaxis()-> GetBinLabel(cutflow_+1)) == "" ) 
      h1_["cutflow"] -> GetXaxis()-> SetBinLabel(cutflow_+1,"Fill MssmHbb Histograms");
   
   Composite<Jet,Jet> c_12(*(selectedJets_[0]),*(selectedJets_[1]));
   float mbb = 0.;
   if ( config_->isMC() || !config_->signalRegion() ) mbb = c_12.m();
   
   h1_["mssmhbb_mbb"] -> Fill(mbb,weight_);
   h1_["cutflow"] -> Fill(cutflow_,weight_);

}


