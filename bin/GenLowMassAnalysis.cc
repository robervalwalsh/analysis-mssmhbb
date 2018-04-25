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
#include "TH2.h" 

#include "Analysis/Tools/interface/Analysis.h"
#include "Analysis/Tools/interface/Utils.h"
#include "Analysis/Tools/bin/macro_config.h"

typedef std::map<std::string, TH1F*> th1s;
typedef std::map<std::string, TH2F*> th2s;

using namespace std;
using namespace analysis;
using namespace analysis::tools;


void CreateHistograms(th1s &, th2s &);

// =============================================================================================   
int main(int argc, char * argv[])
{
   
   if ( macro_config(argc, argv) != 0 ) return -1;
   
   TH1::SetDefaultSumw2();  // proper treatment of errors when scaling histograms
   
   // Input files list
   Analysis analysis(inputlist_);
   
   // Collections
   analysis.addTree<GenParticle> ("GenParticles",genParticleCol_);
   analysis.triggerResults(triggerCol_);
   
   
   // Output histograms
   TFile hout(outputRoot_.c_str(),"recreate");
   th1s h1;
   th2s h2;
   CreateHistograms(h1,h2);
   
   // Analysis of events
   std::cout << "This analysis has " << analysis.size() << " events" << std::endl;
   
   if ( nevtmax_ < 0 ) nevtmax_ = analysis.size();
   for ( int i = 0 ; i < nevtmax_ ; ++i )
   {
      if ( i > 0 && i%100000==0 ) std::cout << i << "  events processed! " << std::endl;
      
      analysis.event(i);
      
      if ( !hltPath_.empty() )
      {
         int triggerFired = analysis.triggerResult(hltPath_);
         if ( !triggerFired ) continue;
      }
      
      auto genParticles = analysis.collection<GenParticle>("GenParticles");
      std::vector <GenParticle> higgs_b;
      std::vector <GenParticle> assoc_b;
      
      // Identifying associated b quarks and Higgs daughters
      for ( int j = 0 ; j < genParticles->size() ; ++j )
      {
         GenParticle gp = genParticles->at(j);
         if ( abs(gp.pdgId()) == 5 && gp.status()==23 )
         {
            if ( gp.higgsDaughter() ) higgs_b.push_back(gp);
            else assoc_b.push_back(gp);
            if ( higgs_b.size() == 2 && assoc_b.size() == 2 ) break;
         }
      }
      std::sort(higgs_b.begin(), higgs_b.end(), greater_than_pt<GenParticle>());       
      std::sort(assoc_b.begin(), assoc_b.end(), greater_than_pt<GenParticle>());  
      
      // Angular selection, higgs daughters and leading associated must be within tracker acceptance
      if ( fabs(higgs_b[0].eta()) > etamax_[0] || fabs(higgs_b[1].eta()) > etamax_[1] ) continue;
      if ( fabs(assoc_b[0].eta()) > etamax_[2] ) continue;
      
      
      // kinematic variable without ranking in histo name indicates there is no topology selection
      h1["mhiggs"]      -> Fill((higgs_b[0].p4()+higgs_b[1].p4()).M());
      
      // Start building a topology
      std::vector<GenParticle> sel_bs;
      for ( auto & b : higgs_b ) sel_bs.push_back(b);
      for ( auto & b : assoc_b ) sel_bs.push_back(b);
      std::sort(sel_bs.begin(), sel_bs.end(), greater_than_pt<GenParticle>());  
      
      if ( sel_bs[0].higgsDaughter() )  // leading parton is a higgs daughter
      {
         if ( sel_bs[0].pt() > ptmin_[0] && sel_bs[1].pt() > ptmin_[1] && sel_bs[2].pt() > ptmin_[2] )
         {
            
            if ( ! sel_bs[1].higgsDaughter() && sel_bs[2].higgsDaughter() ) // Hb Ab Hb
            {
               h1["pt1_hah"] -> Fill(higgs_b[0].pt());
               h1["pt2_hah"] -> Fill(assoc_b[0].pt());
               h1["pt3_hah"] -> Fill(higgs_b[1].pt());
               h1["m23_hah"] -> Fill((sel_bs[1].p4()+sel_bs[2].p4()).M());
               h1["m12_hah"] -> Fill((sel_bs[0].p4()+sel_bs[1].p4()).M());
               
               h1["dphi12_hah"] -> Fill(fabs(sel_bs[0].p4().DeltaPhi(sel_bs[1].p4())));
               h1["dphi23_hah"] -> Fill(fabs(sel_bs[1].p4().DeltaPhi(sel_bs[2].p4())));
            }
            if ( sel_bs[1].higgsDaughter() && ! sel_bs[2].higgsDaughter() )  // Hb Hb Ab
            {
               h1["pt1_hha"] -> Fill(higgs_b[0].pt());
               h1["pt2_hha"] -> Fill(higgs_b[1].pt());
               h1["pt3_hha"] -> Fill(assoc_b[0].pt());
               h1["m23_hha"] -> Fill((sel_bs[1].p4()+sel_bs[2].p4()).M());
               h1["m12_hha"] -> Fill((sel_bs[0].p4()+sel_bs[1].p4()).M());
               h1["dphi12_hha"] -> Fill(fabs(sel_bs[0].p4().DeltaPhi(sel_bs[1].p4())));
               h1["dphi23_hha"] -> Fill(fabs(sel_bs[1].p4().DeltaPhi(sel_bs[2].p4())));
            }
            if ( ! sel_bs[1].higgsDaughter() && ! sel_bs[2].higgsDaughter() ) // Hb Ab Ab
            {
               h1["pt1_haa"] -> Fill(higgs_b[0].pt());
               h1["pt2_haa"] -> Fill(assoc_b[0].pt());
               h1["pt3_haa"] -> Fill(assoc_b[1].pt());
               h1["m23_haa"] -> Fill((sel_bs[1].p4()+sel_bs[2].p4()).M());
               h1["m12_haa"] -> Fill((sel_bs[0].p4()+sel_bs[1].p4()).M());
//               h1["dphi12_haa"] -> Fill(fabs(sel_bs[0].p4().DeltaPhi(sel_bs[1].p4())));
//               h1["dphi23_haa"] -> Fill(fabs(sel_bs[1].p4().DeltaPhi(sel_bs[2].p4())));
               
            }
            h1["m12"] -> Fill((sel_bs[0].p4()+sel_bs[1].p4()).M());

         }
      }
      else  // leading parton is an associated b quark - Ab Hb Hb
      {
//         h1["pt1_assoc_b"] -> Fill(assoc_b[0].pt());
         if ( sel_bs[0].pt() > ptmin_[0] && sel_bs[1].pt() > ptmin_[1] && sel_bs[2].pt() > ptmin_[2] )
         {
            if ( sel_bs[1].higgsDaughter() && sel_bs[2].higgsDaughter() ) // Ab Hb Hb
            {
               h1["pt1_ahh"] -> Fill(assoc_b[0].pt());
               h1["pt2_ahh"] -> Fill(higgs_b[0].pt());
               h1["pt3_ahh"] -> Fill(higgs_b[1].pt());
               h1["m23_ahh"] -> Fill((sel_bs[1].p4()+sel_bs[2].p4()).M());
               h1["m12_ahh"] -> Fill((sel_bs[0].p4()+sel_bs[1].p4()).M());
//               h1["dphi12_ahh"] -> Fill(fabs(sel_bs[0].p4().DeltaPhi(sel_bs[1].p4())));
//               h1["dphi23_ahh"] -> Fill(fabs(sel_bs[1].p4().DeltaPhi(sel_bs[2].p4())));
            }
            
            if ( sel_bs[1].higgsDaughter() && !sel_bs[2].higgsDaughter() ) // Ab Hb Ab
            {
               h1["pt1_aha"] -> Fill(assoc_b[0].pt());
               h1["pt2_aha"] -> Fill(higgs_b[0].pt());
               h1["pt3_aha"] -> Fill(assoc_b[1].pt());
               h1["m23_aha"] -> Fill((sel_bs[1].p4()+sel_bs[2].p4()).M());
               h1["m12_aha"] -> Fill((sel_bs[0].p4()+sel_bs[1].p4()).M());
//               h1["dphi12_aha"] -> Fill(fabs(sel_bs[0].p4().DeltaPhi(sel_bs[1].p4())));
//               h1["dphi23_aha"] -> Fill(fabs(sel_bs[1].p4().DeltaPhi(sel_bs[2].p4())));
            }
            
            if ( !sel_bs[1].higgsDaughter() && sel_bs[2].higgsDaughter() ) // Ab Ab Hb
            {
               h1["pt1_aah"] -> Fill(assoc_b[0].pt());
               h1["pt2_aah"] -> Fill(assoc_b[1].pt());
               h1["pt3_aah"] -> Fill(higgs_b[0].pt());
               h1["m23_aah"] -> Fill((sel_bs[1].p4()+sel_bs[2].p4()).M());
               h1["m12_aah"] -> Fill((sel_bs[0].p4()+sel_bs[1].p4()).M());
//               h1["dphi12_aah"] -> Fill(fabs(sel_bs[0].p4().DeltaPhi(sel_bs[1].p4())));
//               h1["dphi23_aah"] -> Fill(fabs(sel_bs[1].p4().DeltaPhi(sel_bs[2].p4())));
            }
            
            h1["eta1_assoc_b1"] -> Fill(assoc_b[0].eta());
            h1["eta2_higgs_b1"] -> Fill(higgs_b[0].eta());
            h1["eta3_higgs_b2"] -> Fill(higgs_b[1].eta());
            
            h2["pt1_vs_pt2"] -> Fill(assoc_b[0].pt(),higgs_b[0].pt());
            h1["m23"] -> Fill((higgs_b[0].p4()+higgs_b[1].p4()).M());
            h1["delta_phi12"] -> Fill(fabs(higgs_b[0].p4().DeltaPhi(assoc_b[0].p4())));
            h1["delta_phi23"] -> Fill(fabs(higgs_b[0].p4().DeltaPhi(higgs_b[1].p4())));
            h1["delta_eta12"] -> Fill(fabs(higgs_b[0].eta()-assoc_b[0].eta()));
            h1["delta_eta23"] -> Fill(fabs(higgs_b[0].eta()-higgs_b[1].eta()));
            h1["delta_r12"]   -> Fill(fabs(higgs_b[0].p4().DeltaR(assoc_b[0].p4())));
            h1["delta_r23"]   -> Fill(fabs(higgs_b[0].p4().DeltaR(higgs_b[1].p4())));
            
            h1["delta_pt12"]  -> Fill((assoc_b[0].pt()-higgs_b[0].pt())/assoc_b[0].pt());
            h1["delta_pt23"]  -> Fill((higgs_b[0].pt()-higgs_b[1].pt())/higgs_b[0].pt());
            
         }
      }
         
   } // end loop events
   
   for (auto & i : h1)
   {
      i.second -> Write();
   }
   for (auto & i : h2)
   {
      i.second -> Write();
   }
   
   hout.Write();
   hout.Close();
   
      
   
} //end main

void CreateHistograms(th1s & h1, th2s & h2)
{
//    h1["pt_assoc_b1"] = new TH1F("pt_assoc_b1","",100,0,1000);
//    h1["pt_assoc_b2"] = new TH1F("pt_assoc_b2","",100,0,1000);
//    h1["pt_higgs_b1"] = new TH1F("pt_higgs_b1","",100,0,1000);
//    h1["pt_higgs_b2"] = new TH1F("pt_higgs_b2","",100,0,1000);
//    
//    h1["pt1_b"] = new TH1F("pt1_b","",100,0,1000);
//    h1["pt1_higgs_b"] = new TH1F("pt1_higgs_b","",100,0,1000);
//    h1["pt1_assoc_b"] = new TH1F("pt1_assoc_b","",100,0,1000);
   
   // Ab Hb Hb
   h1["pt1_ahh"] = new TH1F("pt1_ahh","",100,0,1000);
   h1["pt2_ahh"] = new TH1F("pt2_ahh","",100,0,1000);
   h1["pt3_ahh"] = new TH1F("pt3_ahh","",100,0,1000);
   h1["m23_ahh"] = new TH1F("m23_ahh","",100,0,1000);
   h1["m12_ahh"] = new TH1F("m12_ahh","",100,0,1000);
   h1["dphi12_ahh"] = new TH1F("dphi12_ahh","",100,0,3.2);
   h1["dphi23_ahh"] = new TH1F("dphi23_ahh","",100,0,3.2);
   
   // Hb Ab Hb
   h1["pt1_hah"] = new TH1F("pt1_hah","",100,0,1000);
   h1["pt2_hah"] = new TH1F("pt2_hah","",100,0,1000);
   h1["pt3_hah"] = new TH1F("pt3_hah","",100,0,1000);
   h1["m23_hah"] = new TH1F("m23_hah","",100,0,1000);
   h1["m12_hah"] = new TH1F("m12_hah","",100,0,1000);
   h1["dphi12_hah"] = new TH1F("dphi12_hah","",100,0,3.2);
   h1["dphi23_hah"] = new TH1F("dphi23_hah","",100,0,3.2);
   
   // Hb Hb Ab
   h1["pt1_hha"] = new TH1F("pt1_hha","",100,0,1000);
   h1["pt2_hha"] = new TH1F("pt2_hha","",100,0,1000);
   h1["pt3_hha"] = new TH1F("pt3_hha","",100,0,1000);
   h1["m23_hha"] = new TH1F("m23_hha","",100,0,1000);
   h1["m12_hha"] = new TH1F("m12_hha","",100,0,1000);
   h1["dphi12_hha"] = new TH1F("dphi12_hha","",100,0,3.2);
   h1["dphi23_hha"] = new TH1F("dphi23_hha","",100,0,3.2);
   
   // Ab Ab Hb
   h1["pt1_aah"] = new TH1F("pt1_aah","",100,0,1000);
   h1["pt2_aah"] = new TH1F("pt2_aah","",100,0,1000);
   h1["pt3_aah"] = new TH1F("pt3_aah","",100,0,1000);
   h1["m23_aah"] = new TH1F("m23_aah","",100,0,1000);
   h1["m12_aah"] = new TH1F("m12_aah","",100,0,1000);
   h1["dphi12_aah"] = new TH1F("dphi12_aah","",100,0,3.2);
   h1["dphi23_aah"] = new TH1F("dphi23_aah","",100,0,3.2);
   
   // Hb Ab Ab
   h1["pt1_haa"] = new TH1F("pt1_haa","",100,0,1000);
   h1["pt2_haa"] = new TH1F("pt2_haa","",100,0,1000);
   h1["pt3_haa"] = new TH1F("pt3_haa","",100,0,1000);
   h1["m23_haa"] = new TH1F("m23_haa","",100,0,1000);
   h1["m12_haa"] = new TH1F("m12_haa","",100,0,1000);
   h1["dphi12_haa"] = new TH1F("dphi12_haa","",100,0,3.2);
   h1["dphi23_haa"] = new TH1F("dphi23_haa","",100,0,3.2);
   
   // Ab Hb Ab
   h1["pt1_aha"] = new TH1F("pt1_aha","",100,0,1000);
   h1["pt2_aha"] = new TH1F("pt2_aha","",100,0,1000);
   h1["pt3_aha"] = new TH1F("pt3_aha","",100,0,1000);
   h1["m23_aha"] = new TH1F("m23_aha","",100,0,1000);
   h1["m12_aha"] = new TH1F("m12_aha","",100,0,1000);
   h1["dphi12_aha"] = new TH1F("dphi12_aha","",100,0,3.2);
   h1["dphi23_aha"] = new TH1F("dphi23_aha","",100,0,3.2);

      ///
   h1["eta1_assoc_b1"] = new TH1F("eta1_assoc_b1","",100,-2.5,2.5);
   h1["eta2_higgs_b1"] = new TH1F("eta2_higgs_b1","",100,-2.5,2.5);
   h1["eta3_higgs_b2"] = new TH1F("eta3_higgs_b2","",100,-2.5,2.5);
   

   h2["pt1_vs_pt2"] = new TH2F("pt1_vs_pt2","",100,0,1000,100,0,1000);
   
   h1["m12"] = new TH1F("m12","",500,0,1000);
   h1["m23"] = new TH1F("m23","",100,0,200);
   h1["mhiggs"] = new TH1F("mhiggs","",500,0,1000);
   
   h1["delta_phi12"] = new TH1F("delta_phi12","",100,0,3.2);
   h1["delta_phi23"] = new TH1F("delta_phi23","",100,0,3.2);
   h1["delta_eta12"] = new TH1F("delta_eta12","",200,0,10);
   h1["delta_eta23"] = new TH1F("delta_eta23","",200,0,10);
   h1["delta_r12"] = new TH1F("delta_r12","",200,0,10);
   h1["delta_r23"] = new TH1F("delta_r23","",200,0,10);
   
   h1["delta_pt12"] = new TH1F("delta_pt12","",200,0,1);
   h1["delta_pt23"] = new TH1F("delta_pt23","",200,0,1);

}
