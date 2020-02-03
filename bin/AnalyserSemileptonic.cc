#include "Analysis/MssmHbb/interface/MssmHbbAnalyser.h"

using namespace std;
using namespace analysis;
using namespace analysis::tools;
using namespace analysis::mssmhbb;


// =============================================================================================   
int main(int argc, char ** argv)
{
   TH1::SetDefaultSumw2();  // proper treatment of errors when scaling histograms
   
   MssmHbbAnalyser mssmhbb(argc,argv);
   
   mssmhbb.jetHistograms("final_selection");
   
   // you can alway retrieve the old analysis object
   auto analysis = mssmhbb.analysis();
   
   // the configuration is different from the old case, the parameters from the config are dealt 
   // within the methods
   // see documentation (in progress) at 
   // http://www.desy.de/~walsh/doxygen/analysis-tools/v3.3/classanalysis_1_1tools_1_1Config.html
   // http://www.desy.de/~walsh/doxygen/analysis-tools/v3.3/Config_8cc_source.html
   auto config = mssmhbb.config();
   
   for ( int i = 0 ; i < mssmhbb.nEvents() ; ++i )
   {
      if ( ! mssmhbb.event(i)                 )   continue;

      // trigger selection
      if ( ! mssmhbb.selectionHLT()           )   continue;
      if ( ! mssmhbb.selectionL1 ()           )   continue;  // useful in case of OR logics of L1 seeds
   
      // jet energy resolution smearing (MC) and b energy regression (data and MC)
      mssmhbb.actionApplyJER();
      mssmhbb.actionApplyBjetRegression();
      
      // objects identifications
      if ( ! mssmhbb.selectionMuonId()        )   continue;
      if ( ! mssmhbb.selectionJetId()         )   continue;
      if ( ! mssmhbb.selectionJetPileupId()   )   continue;
      
      // objects amounts
      if ( ! mssmhbb.selectionNMuons()        )   continue;
      if ( ! mssmhbb.selectionNJets()         )   continue;
      
      // muon  kinematic selection
      if ( ! mssmhbb.selectionMuons()         )   continue;
      if ( ! mssmhbb.onlineMuonMatching()     )   continue;
            
         
      // Muon jet association, a muon can be in either of the two leading jets.
      // If argument is false, the jet label follows the jet ranking, regardless of where the muon is
      // If argument is true(default), the jet label 1 refers to the muon-jet, this way one can apply 
      // muon SF to jet 1 only
      if ( ! mssmhbb.muonJet()                )   continue;
         
      //  1st and 2nd jet kinematic selection
      if ( ! mssmhbb.selectionJet(1)          )   continue;
      if ( ! mssmhbb.selectionJet(2)          )   continue;
      
      // jet delta eta 1,2 selection
      if ( ! mssmhbb.selectionJetDeta(1,2)    )   continue;
      
      // jets 1, 2 matching to online jets
      if ( ! mssmhbb.onlineJetMatching(1)     )   continue;
      if ( ! mssmhbb.onlineJetMatching(2)     )   continue;
      
      // btag of two leading jets
      if ( ! mssmhbb.selectionBJet(1)         )   continue;
      mssmhbb.actionApplyBtagSF(1);
      if ( ! mssmhbb.selectionBJet(2)         )   continue;
      mssmhbb.actionApplyBtagSF(2);
      
      // jets 1,2 matching to online btag objects
      if ( ! mssmhbb.onlineBJetMatching(1)    )   continue;
      if ( ! mssmhbb.onlineBJetMatching(2)    )   continue;
      
      // 3rd jet kinematic selection
      if ( ! mssmhbb.selectionJet(3)          )   continue;
      
      // delta R jet selection
      if ( ! mssmhbb.selectionJetDr(1,2)      )   continue;
      if ( ! mssmhbb.selectionJetDr(1,3)      )   continue;
      if ( ! mssmhbb.selectionJetDr(2,3)      )   continue;
      mssmhbb.actionApplyBtagSF(3);
      
      // 3rd jet btag selection
      if ( ! mssmhbb.selectionBJet(3)         )   continue;

      mssmhbb.fillJetHistograms("final_selection");
      
   }
   
} //end main

