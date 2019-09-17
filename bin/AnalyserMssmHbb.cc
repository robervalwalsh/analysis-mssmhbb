#include "Analysis/MssmHbb/interface/MssmHbbAnalyser.h"

// this example has no selection
// for MC it will apply the generator weights

using namespace std;
using namespace analysis;
using namespace analysis::mssmhbb;

int main(int argc, char ** argv)
{
   MssmHbbAnalyser mssmhbb(argc,argv);
   auto config = mssmhbb.config();
   
// HISTOGRAMS definitions  
   // create some predefined jet histograms
   // if not defined, the number of jets is nJetMin from the configurations
   mssmhbb.jetHistograms("selection01");
   mssmhbb.jetHistograms("selection02");
   mssmhbb.jetHistograms("selection03");
   
   for ( int i = 0 ; i < mssmhbb.nEvents() ; ++i )
   {
      if ( ! mssmhbb.event(i)                  )   continue;
      
      // trigger selection
      if ( ! mssmhbb.selectionHLT()            )   continue;
      if ( ! mssmhbb.selectionL1 ()            )   continue;  // to be used in case of "OR" of seeds
      
   // JETS
      mssmhbb.actionApplyJER();                               // correction : jet energy resolution smearing
      mssmhbb.actionApplyBjetRegression();                    // correction : jet energy regression (for b jets)
      if ( ! mssmhbb.selectionJetId()          )   continue;  // selection  : jet identification 
      if ( ! mssmhbb.selectionJetPileupId()    )   continue;  // selection  : jet Pileup identification 
      if ( ! mssmhbb.selectionNJets()          )   continue;  // selection  : number of jets 
      
      // the lines below will only be executed if rank <= nJetsMin
      if ( ! mssmhbb.selectionJet(1)           )   continue;  // selection  : jet1 pt and eta 
      if ( ! mssmhbb.selectionJet(2)           )   continue;  // selection  : jet2 pt and eta 
      if ( ! mssmhbb.selectionJetDphi(1,2)     )   continue;  // selection  : delta_phi_jets (1,2) [or  MIN(neg): mssmhbb.selectionJetDphi(1,2,-2.0) / MAX(pos): mssmhbb.selectionJetDphi(1,2,+2.0)]
      // jets 1, 2 matching to online jets
      if ( ! mssmhbb.onlineJetMatching(1)      )   continue;
      if ( ! mssmhbb.onlineJetMatching(2)      )   continue;
      mssmhbb.fillJetHistograms("selection01");               // histograms : jets fill
      
   // BTAG: two leading jets
      if ( ! mssmhbb.selectionBJet(1)          )   continue;
      mssmhbb.actionApplyBtagSF(1);
      if ( ! mssmhbb.selectionBJet(2)          )   continue;
      mssmhbb.actionApplyBtagSF(2);
      // jets 1,2 matching to online btag objects
      if ( ! mssmhbb.onlineBJetMatching(1)     )   continue;
      if ( ! mssmhbb.onlineBJetMatching(2)     )   continue;
         
      mssmhbb.fillJetHistograms("selection02");               // histograms : jets fill
      
   // 3rd jet kinematic selection 
      if ( ! mssmhbb.selectionJet(3)           )   continue;
         
   // delta R jet selection
      if ( ! mssmhbb.selectionJetDr(1,2)       )   continue;
      if ( ! mssmhbb.selectionJetDr(1,3)       )   continue;
      if ( ! mssmhbb.selectionJetDr(2,3)       )   continue;
         
      // 3rd jet btag selection
      if ( ! mssmhbb.selectionBJet(3)          )   continue;
      mssmhbb.actionApplyBtagSF(3);

       mssmhbb.fillJetHistograms("selection03");               // histograms : jets fill
   // HISTOGRAMS
      
   }  //end event loop
   

} // end main
      
