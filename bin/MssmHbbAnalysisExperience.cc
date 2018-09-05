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

#include "Analysis/MssmHbb/interface/MssmHbbAnalyser.h"

using namespace std;
using namespace analysis;
using namespace analysis::tools;
using namespace analysis::mssmhbb;

using TH1s = std::map<std::string, TH1F*>;
using TH2s = std::map<std::string, TH2F*>;
     

// float GetBTag(const Jet & jet, const std::string & algo);
// void CreateHistograms(TH1s &, const int & n = 3);
// void CreateHistograms(TH2s &, const int & n = 3);
// template <typename T>
// void WriteHistograms(T & );


// =============================================================================================   
int main(int argc, char ** argv)
{
   TH1::SetDefaultSumw2();  // proper treatment of errors when scaling histograms
   
   MssmHbbAnalyser mssmhbb(argc,argv);
   
   // Analysis of events
   std::cout << "The sample size is " << mssmhbb.analysis()->size() << " events" << std::endl;
   
// 
   for ( int i = 0 ; i < mssmhbb.nEvents() ; ++i )
   {
      if ( i > 0 && i%100000==0 ) std::cout << i << "  events processed! " << std::endl;
      bool goodEvent = mssmhbb.event(i);
      if ( ! goodEvent ) continue;
      
      // histograms
      auto h_cut = mssmhbb.H1F("cutflow");
         
   // trigger selection
      if ( ! mssmhbb.selectionTrigger() )          continue;
      h_cut -> Fill(0);
         
   // jet identification selection
      if ( ! mssmhbb.selectionJetId() )            continue;
      if ( ! mssmhbb.selectionJetPileupId() )      continue;
      if ( ! mssmhbb.selectionNJets() )            continue;
      h_cut -> Fill(1);
      
   //  jet kinematic selection
      if ( ! mssmhbb.selectionJet(1) )             continue;
      if ( ! mssmhbb.selectionJet(2) )             continue;
      h_cut -> Fill(2);
      
   // jet delta eta selection
      if ( ! mssmhbb.selectionJetDeta(1,2,1.5) )   continue;
      h_cut -> Fill(3);
      
   // matching to online jets
      if ( ! mssmhbb.onlineJetMatching(1) )        continue;
      if ( ! mssmhbb.onlineJetMatching(2) )        continue;
      h_cut -> Fill(4);
      
   // btag of two leading jets
      if ( ! mssmhbb.selectionBJet(1) )            continue;
      if ( ! mssmhbb.selectionBJet(2) )            continue;
      h_cut -> Fill(5);
      
   // matching to online btag objects
      if ( ! mssmhbb.onlineBJetMatching(1) )       continue;
      if ( ! mssmhbb.onlineBJetMatching(2) )       continue;
      h_cut -> Fill(6);
      
   // 3rd jet kinematic selection
      if ( ! mssmhbb.selectionJet(3) )             continue;
      h_cut -> Fill(7);
      
      // delta R jet selection
      if ( ! mssmhbb.selectionJetDr(1,2,-1.) )     continue;
      if ( ! mssmhbb.selectionJetDr(1,3,-1.) )     continue;
      if ( ! mssmhbb.selectionJetDr(2,3,-1.) )     continue;
      h_cut -> Fill(8);
      
   // 3rd jet btag selection
      if ( mssmhbb.config()->signalRegion() )
      {
         if ( ! mssmhbb.selectionBJet(3) )         continue;
      }
      else
      {
         if ( ! mssmhbb.selectionNonBJet(3) )      continue;
      }
      h_cut -> Fill(9);
      
      mssmhbb.fillJetHistograms();
      
   }
   mssmhbb.end();
   
} //end main

