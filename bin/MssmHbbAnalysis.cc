#include "boost/program_options.hpp"
#include "boost/algorithm/string.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

#include "TFile.h" 
#include "TFileCollection.h"
#include "TChain.h"
#include "TH1.h" 

#include "Analysis/Tools/interface/Analysis.h"
#include "Analysis/Tools/bin/macro_config.h"

using namespace std;
using namespace analysis;
using namespace analysis::tools;

float GetBTag(const Jet & jet, const std::string & algo);

// =============================================================================================   
int main(int argc, char * argv[])
{
   
   if ( macro_config(argc, argv) != 0 ) return -1;
   
   TH1::SetDefaultSumw2();  // proper treatment of errors when scaling histograms
   
   // Cuts
   float btagmin[3] = { jetsbtagmin_[0], jetsbtagmin_[1], jetsbtagmin_[2]};
   
   // Input files list
   Analysis analysis(inputlist_);
   
   // Jets
   analysis.addTree<Jet> ("Jets",jetsCol_);


   // Trigger path info
   analysis.triggerResults(triggerCol_);
   // Trigger objects
   for ( auto & obj : triggerObjects_ )
   {
      analysis.addTree<TriggerObject> (obj,Form("%s/%s",triggerObjDir_.c_str(),obj.c_str()));
   }
   

   // JSON for data   
   if( !isMC_ ) analysis.processJsonFile(json_);
   
   // output file
   TFile hout(outputRoot_.c_str(),"recreate");
   
   std::string sr_s = "SR";
   if ( ! signalregion_ ) sr_s = "CR";
   boost::algorithm::replace_last(outputRoot_, ".root", "_"+sr_s+".root"); 
   
   std::map<std::string, TH1F*> h1;
   h1["n"]        = new TH1F("n" , "" , 30, 0, 30);
   h1["n_btag"]    = new TH1F("n_btag" , "" , 30, 0, 30);
   h1["n_ptmin20"]= new TH1F("n_ptmin20" , "" , 30, 0, 30);
   h1["n_ptmin20_btag"] = new TH1F("n_ptmin20_btag" , "" , 30, 0, 30);
   for ( int i = 0 ; i < njetsmin_ ; ++i )
   {
      h1[Form("pt_%i",i)]         = new TH1F(Form("pt_%i",i) , "" , 100, 0, 1000);
      h1[Form("eta_%i",i)]        = new TH1F(Form("eta_%i",i) , "" , 100, -5, 5);
      h1[Form("phi_%i",i)]        = new TH1F(Form("phi_%i",i) , "" , 100, -4, 4);
      h1[Form("btag_%i",i)]       = new TH1F(Form("btag_%i",i) , "" , 500, 0, 1);
      
      h1[Form("pt_%i_btag",i)]     = new TH1F(Form("pt_%i_btag",i) , "" , 100, 0, 1000);
      h1[Form("eta_%i_btag",i)]    = new TH1F(Form("eta_%i_btag",i) , "" , 100, -5, 5);
      h1[Form("phi_%i_btag",i)]    = new TH1F(Form("phi_%i_btag",i) , "" , 100, -4, 4);
      h1[Form("btag_%i_btag",i)]   = new TH1F(Form("btag_%i_btag",i) , "" , 500, 0, 1);
   }
   h1["m12"]     = new TH1F("m12"     , "" , 50, 0, 1000);
   h1["m12_btag"] = new TH1F("m12_btag" , "" , 50, 0, 1000);
   
   
   double mbb;
   double weight;
   TTree *tree = new TTree("MssmHbb_13TeV","");
   tree->Branch("mbb",&mbb,"mbb/D");
   tree->Branch("weight",&weight,"weight/D");
   
   // Analysis of events
   std::cout << "This analysis has " << analysis.size() << " events" << std::endl;
   
   // Cut flow
   // 0: triggered events
   // 1: 3+ idloose jets
   // 2: kinematics
   // 3: matched to online
   // 4: delta R
   // 5: delta eta
   // 6: btag (bbnb)
   int nsel[10] = { };
   int nmatch[10] = { };

   if ( nevtmax_ < 0 ) nevtmax_ = analysis.size();
   for ( int i = 0 ; i < nevtmax_ ; ++i )
   {
      int njets = 0;
      int njets_btag = 0;
      bool goodEvent = true;
      
      if ( i > 0 && i%100000==0 ) std::cout << i << "  events processed! " << std::endl;
      
      analysis.event(i);
      if (! isMC_ )
      {
         if (!analysis.selectJson() ) continue; // To use only goodJSonFiles
      }
      
      int triggerFired = analysis.triggerResult(hltPath_);
      if ( !triggerFired ) continue;
      
      ++nsel[0];
      
      // match offline to online
      analysis.match<Jet,TriggerObject>("Jets",triggerObjects_,0.3);
      
      // Jets - std::shared_ptr< Collection<Jet> >
      auto slimmedJets = analysis.collection<Jet>("Jets");
      std::vector<Jet *> selectedJets;
      for ( int j = 0 ; j < slimmedJets->size() ; ++j )
      {
         if ( slimmedJets->at(j).idLoose() ) selectedJets.push_back(&slimmedJets->at(j));
      }
      if ( (int)selectedJets.size() < njetsmin_ ) continue;
      
      ++nsel[1];
      
      // Kinematic selection - 3 leading jets
      for ( int j = 0; j < njetsmin_; ++j )
      {
         Jet * jet = selectedJets[j];
         if ( jet->pt() < jetsptmin_[j] || fabs(jet->eta()) > jetsetamax_[j] )
         {
            goodEvent = false;
            break;
         }
      }
      
      if ( ! goodEvent ) continue;
      
      ++nsel[2];
      
      // delta_R selection
      for ( int j1 = 0; j1 < njetsmin_-1; ++j1 )
      {
         const Jet & jet1 = *selectedJets[j1];
         for ( int j2 = j1+1; j2 < njetsmin_; ++j2 )
         {
            const Jet & jet2 = *selectedJets[j2];
            if ( jet1.deltaR(jet2) < drmin_ ) goodEvent = false;
         }
      }
      
      if ( ! goodEvent ) continue;
      
      ++nsel[3];
      
      // delta_eta selection
      if ( fabs(selectedJets[0]->eta() - selectedJets[1]->eta()) > detamax_ ) continue;
      
      ++nsel[4];
      
      
      // Fill histograms of kinematic passed events
      for ( int j = 0 ; j < (int)selectedJets.size() ; ++j )
      {
         if ( selectedJets[j]->pt() < 20. ) continue;
         ++njets;
      }
      
      h1["n"] -> Fill(selectedJets.size());
      h1["n_ptmin20"] -> Fill(njets);
      for ( int j = 0; j < njetsmin_; ++j )
      {
         Jet * jet = selectedJets[j];
         h1[Form("pt_%i",j)]   -> Fill(jet->pt());
         h1[Form("eta_%i",j)]  -> Fill(jet->eta());
         h1[Form("phi_%i",j)]  -> Fill(jet->phi());
         h1[Form("btag_%i",j)] -> Fill(GetBTag(*jet,btagalgo_));
         
         if ( j < 2 && GetBTag(*jet,btagalgo_) < btagmin[j] )     goodEvent = false;
         if ( ! signalregion_ )
         {
            if ( j == 2 && GetBTag(*jet,btagalgo_) > nonbtagwp_ )    goodEvent = false; 
         }
         else
         {
            if ( j == 2 && GetBTag(*jet,btagalgo_) < btagmin[j] ) goodEvent = false; 
         }
      }
      
      h1["m12"] -> Fill((selectedJets[0]->p4() + selectedJets[1]->p4()).M());
      
      if ( ! goodEvent ) continue;
      
      ++nsel[5];

//            std::cout << "oioi" << std::endl;
      
      
      // Is matched?
      bool matched[10] = {true,true,true,true,true,true,true,true,true,true};
      for ( int j = 0; j < 2; ++j )
      {
         Jet * jet = selectedJets[j];
//         for ( auto & obj : triggerObjects_ )   matched = (matched && jet->matched(obj));
         for ( size_t io = 0; io < triggerObjects_.size() ; ++io )
         {       
            if ( ! jet->matched(triggerObjects_[io]) ) matched[io] = false;
         }
      }
      
      for ( size_t io = 0; io < triggerObjects_.size() ; ++io )
      {
         if ( matched[io] ) ++nmatch[io];
         goodEvent = ( goodEvent && matched[io] );
      }
      
      if ( ! goodEvent ) continue;
      
      ++nsel[6];
     
      // Fill histograms of passed bbnb btagging selection
      for ( int j = 0 ; j < (int)selectedJets.size() ; ++j )
      {
         if ( selectedJets[j]->pt() < 20. ) continue;
         ++njets_btag;
      }
      h1["n_btag"] -> Fill(selectedJets.size());
      h1["n_ptmin20_btag"] -> Fill(njets_btag);
      for ( int j = 0; j < njetsmin_; ++j )
      {
         Jet * jet = selectedJets[j];
         h1[Form("pt_%i_btag",j)]   -> Fill(jet->pt());
         h1[Form("eta_%i_btag",j)]  -> Fill(jet->eta());
         h1[Form("phi_%i_btag",j)]  -> Fill(jet->phi());
         h1[Form("btag_%i_btag",j)] -> Fill(GetBTag(*jet,btagalgo_));
      }
      mbb = (selectedJets[0]->p4() + selectedJets[1]->p4()).M();
      if ( !signalregion_ )
      { 
         h1["m12_btag"] -> Fill(mbb);
         weight = 1;
         tree -> Fill();
      }
         
   }
   
   for (auto & ih1 : h1)
   {
      ih1.second -> Write();
   }
   
   hout.Write();
   hout.Close();
   
// PRINT OUTS   
   
   // Cut flow
   // 0: triggered events
   // 1: 3+ idloose jets
   // 2: matched to online
   // 3: kinematics
   // 4: delta R
   // 5: delta eta
   // 6: btag (bbnb)
   
   double fracAbs[10];
   double fracRel[10];
   std::string cuts[10];
   cuts[0] = "Triggered";
   cuts[1] = "Triple idloose-jet";
   cuts[2] = "Triple jet kinematics";
   cuts[3] = "Delta R(i;j)";
   cuts[4] = "Delta eta(j1;j2)";
   cuts[5] = "btagged (bbnb)";
   if ( signalregion_ ) cuts[5] = "btagged (bbb)";
   cuts[6] = "Matched to online j1;j2";
   
   printf ("%-23s  %10s  %10s  %10s \n", std::string("Cut flow").c_str(), std::string("# events").c_str(), std::string("absolute").c_str(), std::string("relative").c_str() ); 
   for ( int i = 0; i < 7; ++i )
   {
      fracAbs[i] = double(nsel[i])/nsel[0];
      if ( i>0 )
         fracRel[i] = double(nsel[i])/nsel[i-1];
      else
         fracRel[i] = fracAbs[i];
      printf ("%-23s  %10d  %10.3f  %10.3f \n", cuts[i].c_str(), nsel[i], fracAbs[i], fracRel[i] ); 
   }
   // CSV output
   printf ("%-23s , %10s , %10s , %10s \n", std::string("Cut flow").c_str(), std::string("# events").c_str(), std::string("absolute").c_str(), std::string("relative").c_str() ); 
   for ( int i = 0; i < 7; ++i )
      printf ("%-23s , %10d , %10.3f , %10.3f \n", cuts[i].c_str(), nsel[i], fracAbs[i], fracRel[i] ); 

   // Trigger objects counts   
   std::cout << std::endl;
   printf ("%-40s  %10s \n", std::string("Trigger object").c_str(), std::string("# events").c_str() ); 
//   for ( size_t io = 0; io < triggerObjects_.size() ; ++io )
//   {
//      printf ("%-40s  %10d \n", triggerObjects_[io].c_str(), nmatch[io] ); 
//   }
   
   
   
   
      
   
} //end main

float GetBTag(const Jet & jet, const std::string & algo)
{
   float btag;
   if ( btagalgo_ == "csvivf" || btagalgo_ == "csv" )
   {
      btag = jet.btag("btag_csvivf");
   }
   else if ( btagalgo_ == "deepcsv" )
   {
      btag = jet.btag("btag_deepb") + jet.btag("btag_deepbb");
   }
   else if ( btagalgo_ == "deepbvsall" )
   {
      btag = jet.btag("btag_deepbvsall");
   }
   else
   {
      btag = -9999;
   }
   return btag;
}
