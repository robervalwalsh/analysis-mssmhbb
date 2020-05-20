#include "Analysis/MssmHbb/interface/MssmHbbAnalyser.h"
#include "Analysis/Tools/interface/Composite.h"

using namespace analysis;
using namespace analysis::tools;
using namespace analysis::mssmhbb;

int main(int argc, char ** argv)
{
    TH1::SetDefaultSumw2();  // proper treatment of errors when scaling histograms

    MssmHbbAnalyser mssmhbb(argc,argv);
    
    mssmhbb.mssmHbbTreeExtended();

    for ( int i = 0 ; i < mssmhbb.nEvents() ; ++i )
    {
        if ( ! mssmhbb.event(i)             )  continue;    // read event, run selection/json
        if ( ! mssmhbb.triggerSelection ()  )  continue;    // trigger
        if ( ! mssmhbb.preselection()       )  continue;    // preselection
        if ( ! mssmhbb.jetCorrections()     )  continue;    // jet corrections
        if ( ! mssmhbb.muonSelection()      )  continue;    // muon
        if ( ! mssmhbb.jetSelection()       )  continue;    // jets
        if ( ! mssmhbb.muonJet()            )  continue;    // muon-jet association
        if ( ! mssmhbb.btagSelection()      )  continue;    // btagging
        mssmhbb.endSelection();
        auto j = mssmhbb.selectedJets();
        Composite<Jet,Jet> c_12(*(j[0]),*(j[1]));
//        if ( ( c_12.m() < 0 || i == 107 || i == 804 || i == 2271 || i == 3650 || i == 4216 || i == 5413 || i == 6111 || i == 6411 || i == 6876 || i == 7133 ))
        if ( ( c_12.m() < 0 ))
        {
           std::cout << "Index = " << i << "  Event = " << mssmhbb.analysis()->event() << "   Run = " << mssmhbb.analysis()->run() << "   Lumi = " << mssmhbb.analysis()->lumiSection() << std::endl;
           std::cout << "   M12 = " << c_12.m() << std::endl;
           std::cout << "   Jet 1: pt = " << j[0]->pt() << "  eta = " << j[0]->eta() << "  phi = " << j[0]->phi() << "  m = " << j[0]->m() << "  corr factor = " << j[0]->bRegCorr() << std::endl;
           std::cout << "          px = " << j[0]->px() << "  py = " << j[0]->py() << "  pz = " << j[0]->pz() << "  m = " << j[0]->m() << std::endl;
           std::cout << "   Jet 2: pt = " << j[1]->pt() << "  eta = " << j[1]->eta() << "  phi = " << j[1]->phi() << "  m = " << j[1]->m() << "  corr factor = " << j[1]->bRegCorr() << std::endl;
           std::cout << "          px = " << j[1]->px() << "  py = " << j[1]->py() << "  pz = " << j[1]->pz() << "  m = " << j[1]->m() << std::endl;
           std::cout << "--------" << std::endl;
        }
    }
} // end main
     



