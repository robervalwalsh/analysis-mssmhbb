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
      
//    
//    // Cuts
//    float btagmin[3] = { jetsbtagmin_[0], jetsbtagmin_[1], jetsbtagmin_[2]};
//    
//    TH1s h1;
//    
// //   CreateHistograms(h1);
//    
//    double mbb;
//    double weight;
//    TTree *tree = new TTree("MssmHbb_13TeV","");
//    tree->Branch("mbb",&mbb,"mbb/D");
//    tree->Branch("weight",&weight,"weight/D");
//    
   // Analysis of events
   std::cout << "This analysis has " << mssmhbb.analysis()->size() << " events" << std::endl;
//    
//    // Cut flow
//    // 0: triggered events
//    // 1: 3+ idloose jets
//    // 2: kinematics
//    // 3: matched to online
//    // 4: delta R
//    // 5: delta eta
//    // 6: btag (bbnb)
//    int nsel[10] = { };
//    int nmatch[10] = { };
// 
   int maxevt = mssmhbb.config()->nevtmax_;
   if ( maxevt < 0 ) maxevt = mssmhbb.analysis()->size();
   for ( int i = 0 ; i < maxevt ; ++i )
   {
//       int njets = 0;
//       int njets_btag = 0;
//       bool goodEvent = true;
//       
      if ( i > 0 && i%10000==0 ) std::cout << i << "  events processed! " << std::endl;
//       
      bool goodEvent = mssmhbb.event(i);
      if ( ! goodEvent ) continue;
            
      std::cout << "Event " << i << " has passed " << std::endl;
//       
//       // Kinematic selection - 3 leading jets
//       for ( int j = 0; j < njetsmin_; ++j )
//       {
//          Jet * jet = selectedJets[j];
//          if ( jet->pt() < jetsptmin_[j] || fabs(jet->eta()) > jetsetamax_[j] )
//          {
//             goodEvent = false;
//             break;
//          }
//       }
//       
//       if ( ! goodEvent ) continue;
//       
//       ++nsel[2];
//       
//       // delta_R selection
//       for ( int j1 = 0; j1 < njetsmin_-1; ++j1 )
//       {
//          const Jet & jet1 = *selectedJets[j1];
//          for ( int j2 = j1+1; j2 < njetsmin_; ++j2 )
//          {
//             const Jet & jet2 = *selectedJets[j2];
//             if ( jet1.deltaR(jet2) < drmin_ ) goodEvent = false;
//          }
//       }
//       
//       if ( ! goodEvent ) continue;
//       
//       ++nsel[3];
//       
//       // delta_eta selection
//       if ( fabs(selectedJets[0]->eta() - selectedJets[1]->eta()) > detamax_ ) continue;
//       
//       ++nsel[4];
//       
//       
//       // Fill histograms of kinematic passed events
//       for ( int j = 0 ; j < (int)selectedJets.size() ; ++j )
//       {
//          if ( selectedJets[j]->pt() < 20. ) continue;
//          ++njets;
//       }
//       
// //      h1["n"] -> Fill(selectedJets.size());
// //      h1["n_ptmin20"] -> Fill(njets);
//       for ( int j = 0; j < njetsmin_; ++j )
//       {
//          Jet * jet = selectedJets[j];
// //          h1[Form("pt_%i",j)]   -> Fill(jet->pt());
// //          h1[Form("eta_%i",j)]  -> Fill(jet->eta());
// //          h1[Form("phi_%i",j)]  -> Fill(jet->phi());
// //          h1[Form("btag_%i",j)] -> Fill(GetBTag(*jet,btagalgo_));
//          
//          if ( j < 2 && GetBTag(*jet,btagalgo_) < btagmin[j] )     goodEvent = false;
//          if ( ! signalregion_ )
//          {
//             if ( j == 2 && GetBTag(*jet,btagalgo_) > nonbtagwp_ )    goodEvent = false; 
//          }
//          else
//          {
//             if ( j == 2 && GetBTag(*jet,btagalgo_) < btagmin[j] ) goodEvent = false; 
//          }
//       }
//       
// //      h1["m12"] -> Fill((selectedJets[0]->p4() + selectedJets[1]->p4()).M());
//       
//       if ( ! goodEvent ) continue;
//       
//       ++nsel[5];
// 
//       // Is matched?
//       bool matched[10] = {true,true,true,true,true,true,true,true,true,true};
//       for ( int j = 0; j < 2; ++j )
//       {
//          Jet * jet = selectedJets[j];
// //         for ( auto & obj : triggerObjects_ )   matched = (matched && jet->matched(obj));
//          for ( size_t io = 0; io < triggerObjects_.size() ; ++io )
//          {       
//             if ( ! jet->matched(triggerObjects_[io]) ) matched[io] = false;
//          }
//       }
//       
//       for ( size_t io = 0; io < triggerObjects_.size() ; ++io )
//       {
//          if ( matched[io] ) ++nmatch[io];
//          goodEvent = ( goodEvent && matched[io] );
//       }
//       
//       if ( ! goodEvent ) continue;
//       
//       ++nsel[6];
//      
//       // Fill histograms of passed bbnb btagging selection
//       for ( int j = 0 ; j < (int)selectedJets.size() ; ++j )
//       {
//          if ( selectedJets[j]->pt() < 20. ) continue;
//          ++njets_btag;
//       }
//  //     h1["n_btag"] -> Fill(selectedJets.size());
//  //     h1["n_ptmin20_btag"] -> Fill(njets_btag);
//       for ( int j = 0; j < njetsmin_; ++j )
//       {
//          Jet * jet = selectedJets[j];
// //          h1[Form("pt_%i_btag",j)]   -> Fill(jet->pt());
// //          h1[Form("eta_%i_btag",j)]  -> Fill(jet->eta());
// //          h1[Form("phi_%i_btag",j)]  -> Fill(jet->phi());
// //          h1[Form("btag_%i_btag",j)] -> Fill(GetBTag(*jet,btagalgo_));
//          
// //         h2[Form("eta_phi_%d_btag",j)] -> Fill(jet->eta(),jet->phi());
//          
// //          if ( jet->pt() >= 100 && jet->pt() < 140 )   h1[Form("btag_%d_%d_btag",j,0)] -> Fill(GetBTag(*jet,btagalgo_));
// //          if ( jet->pt() >= 140 && jet->pt() < 200 )   h1[Form("btag_%d_%d_btag",j,1)] -> Fill(GetBTag(*jet,btagalgo_));
// //          if ( jet->pt() >= 200 && jet->pt() < 350 )   h1[Form("btag_%d_%d_btag",j,2)] -> Fill(GetBTag(*jet,btagalgo_));
// //          if ( jet->pt() >= 350                    )   h1[Form("btag_%d_%d_btag",j,3)] -> Fill(GetBTag(*jet,btagalgo_));
//          
//       }
//       mbb = (selectedJets[0]->p4() + selectedJets[1]->p4()).M();
//       if ( !signalregion_ )
//       { 
// //          h1["m12_btag"] -> Fill(mbb);
//          weight = 1;
//          tree -> Fill();
//       }
//          
   }
//    
//    
//    WriteHistograms(h1);
// //   WriteHistograms(h2);
//    
//    hout.Write();
//    hout.Close();
//    
// PRINT OUTS   
   
   // Cut flow
   // 0: triggered events
   // 1: 3+ idloose jets
   // 2: matched to online
   // 3: kinematics
   // 4: delta R
   // 5: delta eta
   // 6: btag (bbnb)
   
//    double fracAbs[10];
//    double fracRel[10];
//    std::string cuts[10];
//    cuts[0] = "Triggered";
//    cuts[1] = "Triple idloose-jet";
//    cuts[2] = "Triple jet kinematics";
//    cuts[3] = "Delta R(i;j)";
//    cuts[4] = "Delta eta(j1;j2)";
//    cuts[5] = "btagged (bbnb)";
//    if ( signalregion_ ) cuts[5] = "btagged (bbb)";
//    cuts[6] = "Matched to online j1;j2";
//    
//    printf ("%-23s  %10s  %10s  %10s \n", std::string("Cut flow").c_str(), std::string("# events").c_str(), std::string("absolute").c_str(), std::string("relative").c_str() ); 
//    for ( int i = 0; i < 7; ++i )
//    {
//       fracAbs[i] = double(nsel[i])/nsel[0];
//       if ( i>0 )
//          fracRel[i] = double(nsel[i])/nsel[i-1];
//       else
//          fracRel[i] = fracAbs[i];
//       printf ("%-23s  %10d  %10.3f  %10.3f \n", cuts[i].c_str(), nsel[i], fracAbs[i], fracRel[i] ); 
//    }
//    // CSV output
//    printf ("%-23s , %10s , %10s , %10s \n", std::string("Cut flow").c_str(), std::string("# events").c_str(), std::string("absolute").c_str(), std::string("relative").c_str() ); 
//    for ( int i = 0; i < 7; ++i )
//       printf ("%-23s , %10d , %10.3f , %10.3f \n", cuts[i].c_str(), nsel[i], fracAbs[i], fracRel[i] ); 
// 
//    // Trigger objects counts   
//    std::cout << std::endl;
// //   printf ("%-40s  %10s \n", std::string("Trigger object").c_str(), std::string("# events").c_str() ); 
// //   for ( size_t io = 0; io < triggerObjects_.size() ; ++io )
// //   {
// //      printf ("%-40s  %10d \n", triggerObjects_[io].c_str(), nmatch[io] ); 
// //   }
//    
//    
   
   std::cout << "oioi" << std::endl;
   mssmhbb.end();
   

   
} //end main

// float GetBTag(const Jet & jet, const std::string & algo)
// {
//    float btag;
//    if ( btagalgo_ == "csvivf" || btagalgo_ == "csv" )
//    {
//       btag = jet.btag("btag_csvivf");
//    }
//    else if ( btagalgo_ == "deepcsv" )
//    {
//       btag = jet.btag("btag_deepb") + jet.btag("btag_deepbb");
//    }
//    else if ( btagalgo_ == "deepbvsall" )
//    {
//       btag = jet.btag("btag_deepbvsall");
//    }
//    else
//    {
//       btag = -9999;
//    }
//    return btag;
// }
// 
// 
// void CreateHistograms(TH1s & h, const int & n)
// {
//    for ( int j = 0; j < n; ++j )
//    {
//       h[Form("pt_jet%d",j)]       = new TH1F(Form("pt_jet%d",j)     , "" ,100 , 0   , 1000  );
//       h[Form("eta_jet%d",j)]      = new TH1F(Form("eta_jet%d",j)    , "" , 60 , -3, 3 );
//       h[Form("btag_jet%d",j)]     = new TH1F(Form("btag_jet%d",j)   , "" , 100 , 0, 1 );
//       for ( int k = j+1; k < njetsmin_ && j < njetsmin_; ++k )
//       {
//          h[Form("dr_jet%d%d",j,k)]     = new TH1F(Form("dr_jet%d%d",j,k)     , "" , 50 , 0, 5 );
//          h[Form("deta_jet%d%d",j,k)]   = new TH1F(Form("deta_jet%d%d",j,k)   , "" ,100 , 0,10 );
//       }
//    }
//    
// }
// 
// void CreateHistograms(TH2s & h, const int & n)
// {
//    
// }
// 
// template <typename T> 
// void WriteHistograms(T & h)
// {
//    // Write histograms to output
//    for (auto & ih : h)
//    {
//       ih.second -> Write();
//    }
// }
// 
