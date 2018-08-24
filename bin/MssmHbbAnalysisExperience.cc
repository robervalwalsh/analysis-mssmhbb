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
   std::cout << "This analysis has " << mssmhbb.analysis()->size() << " events" << std::endl;
   
// 
   int counter = 0;
      
   int maxevt = mssmhbb.config()->nEventsMax();
   if ( maxevt < 0 ) maxevt = mssmhbb.analysis()->size();
   for ( int i = 0 ; i < maxevt ; ++i )
   {
      if ( i > 0 && i%100000==0 ) std::cout << i << "  events processed! " << std::endl;
      bool goodEvent = mssmhbb.event(i);
      if ( ! goodEvent ) continue;
      ++counter;  
   }
      
   mssmhbb.end();
   
   
   std::cout << counter << " events passed the selection" << std::endl;
   
} //end main

