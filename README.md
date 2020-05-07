# `Analysis/MssmHbb`

**Extended framework for the MSSM Hbb all-hadronic and semileptonic analyses in CMS**

**This package inherits from the [Analysis/Tools](https://github.com/desy-cms/analysis-tools) package**

* [Installation](#Installation)
* [Running the analysis](#Running-the-analysis)
  * [Main macro](#Main-macro)
  * [Configuration file](#Configuration-file)
* [Workflow](#Workflow)
* [MssmHbbAnalyser class](#MssmHbbAnalyser-class)

## Installation

The codes as well as the ntuples are independent of CMSSW. However, in order to compile it uses `scram`. So the latest version in any architecture should be fine.


```bash
cmsrel CMSSW_X_Y_Z
cd CMSSW_X_Y_Z/src
cmsenv

git clone https://github.com/desy-cms/analysis-tools.git Analysis/Tools
git clone https://github.com/desy-cms/analysis-mssmhbb.git Analysis/MssmHbb
git clone https://github.com/desy-cms/analysis-calibrations.git Analysis/Tools/data/calibrations
git clone https://github.com/desy-cms/analysis-ntuples.git Analysis/MssmHbb/test/ntuples

scram b -j4 USER_CXXFLAGS="-Wno-misleading-indentation"
hash -r
```

:zzz: The `USER_CXXFLAGS="-Wno-misleading-indentation"` prevents a large number of warnings from misleading identation in modules of the boost library.

:zzz: More information about the additional packages, `calibration`, `ntuples` etc can be found in the `Analysis/Tools` [README.md](https://github.com/desy-cms/analysis-tools/blob/develop/README.md).


## Running the analysis

A macro with full analysis workflow can be found in [Analysis/MssmHbb/bin/MssmHbbAnalysis.cc](bin/MssmHbbAnalysis.cc) with the  configuration file in [Analysis/MssmHbb/test/mssmhbb_semilep_2017.cfg](test/mssmhbb_semilep_2017.cfg).


### Main macro
The structure of the macro is as follows:

```cpp
#include "Analysis/MssmHbb/interface/MssmHbbAnalyser.h"

using namespace analysis;
using namespace analysis::tools;
using namespace analysis::mssmhbb;

int main(int argc, char ** argv)
{
    TH1::SetDefaultSumw2();  // proper treatment of errors when scaling histograms

    MssmHbbAnalyser mssmhbb(argc,argv);

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
    }
} // end main
```

Compared to the Analysis/Tools [example](https://github.com/desy-cms/analysis-tools/blob/develop/README.md#example) this is much simplified, for the individual selections were grouped and included in methods of the `MssmHbbAnalyser` class. However some flexibility to re-order the workflow is still allowed. Of course, the user can also replace any of the `if` statements above and enable more detailed steps of the selections, or add other steps to the workflow.

### Configuration file

The structure of the configuration file is defined by the `Analysis/Tools` package. More details can be found in the `Analysis/Tools` [README.md](https://github.com/desy-cms/analysis-tools/blob/develop/README.md#detailed-description).


## Workflow

The workflow of the main macro for DATA is

```
+----------------------------------------------------------------------------------------------+
| Workflow DATA                                                                                |
+----------------------------------------------------------------------------------------------+
|  0 - Total events read                                                                       |
|  1 - Certified data: Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt            |
|  2 - Using Jet collection: MssmHbb/Events/updatedPatJets                                     |
|  3 - Using Muon collection: MssmHbb/Events/slimmedMuons                                      |
|  4 - HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33_v                               |
|  5 - L1_Mu12er2p3_Jet40er2p3_dR_Max0p4_DoubleJet40er2p3_dEta_Max1p6                          |
|  6 - MuonId: tight                                                                           |
|  7 - JetId: tight                                                                            |
|  8 - JetPileupId: loose                                                                      |
|  9 - b jet energy regression                                                                 |
| 10 - NMuons >= 1                                                                             |
| 11 - Muons selected: pt >  13.0 GeV and |eta| < 2.2                                          |
| 12 - Muons selected: Online muon matching (L1 and L3)                                        |
| 13 - NJets >= 3                                                                              |
| 14 - Jet 1: pt >  60.0 GeV and |eta| < 2.2                                                   |
| 15 - Jet 1: online jet match                                                                 |
| 16 - Jet 2: pt >  50.0 GeV and |eta| < 2.2                                                   |
| 17 - Jet 2: online jet match                                                                 |
| 18 - Jet 3: pt >  30.0 GeV and |eta| < 2.2                                                   |
| 19 - MSSMHbb Semileptonic: Jet-muon association                                              |
| 20 - Jet 1: deepflavour btag > 0.3033 (medium)                                               |
| 21 - Jet 1: online b jet match                                                               |
| 22 - Jet 2: deepflavour btag > 0.3033 (medium)                                               |
| 23 - Jet 2: online b jet match                                                               |
| 24 - Jet 3: deepflavour btag < 0.0521 (loose) [reverse btag]                                 |
| 25 - *** Filling jets histograms - final_selection                                           |
| 26 - Fill MssmHbb Histograms                                                                 |
| 27 - Fill MssmHbb tree                                                                       |
+----------------------------------------------------------------------------------------------+

```

where as for the Monte Carlo it is

```
+----------------------------------------------------------------------------------------------+
| Workflow Monte Carlo                                                                         |
+----------------------------------------------------------------------------------------------+
|  0 - Total events read                                                                       |
|  1 - Generated weighted events (sign of weights)                                             |
|  2 - Using GenParticles collection: MssmHbb/Events/prunedGenParticles                        |
|  3 - Using GenJets collection: MssmHbb/Events/slimmedGenJets                                 |
|  4 - Using Jet collection: MssmHbb/Events/updatedPatJets                                     |
|  5 - Using Muon collection: MssmHbb/Events/slimmedMuons                                      |
|  6 - Pileup weight (PileupWeight_Run2017_Mix_2017.root)                                      |
|  7 - HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33_v                               |
|  8 - L1_Mu12er2p3_Jet40er2p3_dR_Max0p4_DoubleJet40er2p3_dEta_Max1p6                          |
|  9 - MuonId: tight                                                                           |
| 10 - JetId: tight                                                                            |
| 11 - JetPileupId: loose                                                                      |
| 12 - b jet energy regression                                                                 |
| 13 - JER smearing (Fall17_V3_MC_PtResolution_AK4PFchs.txt,Fall17_V3_MC_SF_AK4PFchs.txt)      |
| 14 - NMuons >= 1                                                                             |
| 15 - Muons selected: pt >  13.0 GeV and |eta| < 2.2                                          |
| 16 - Muons selected: Online muon matching (L1 and L3)                                        |
| 17 - NJets >= 3                                                                              |
| 18 - Jet 1: pt >  60.0 GeV and |eta| < 2.2                                                   |
| 19 - Jet 1: online jet match                                                                 |
| 20 - Jet 2: pt >  50.0 GeV and |eta| < 2.2                                                   |
| 21 - Jet 2: online jet match                                                                 |
| 22 - Jet 3: pt >  30.0 GeV and |eta| < 2.2                                                   |
| 23 - MSSMHbb Semileptonic: Jet-muon association                                              |
| 24 - Jet 1: deepflavour btag > 0.3033 (medium)                                               |
| 25 - Jet 1: btag SF applied (deepflavour medium WP)                                          |
| 26 - Jet 1: online b jet match                                                               |
| 27 - Jet 2: deepflavour btag > 0.3033 (medium)                                               |
| 28 - Jet 2: btag SF applied (deepflavour medium WP)                                          |
| 29 - Jet 2: online b jet match                                                               |
+----------------------------------------------------------------------------------------------+

```

## MssmHbbAnalyser class

:warning: To do




