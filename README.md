# `Analysis/MssmHbb`

**Extended framework for the MSSM Hbb all-hadronic and semileptonic analyses in CMS**

**This package inherits from the [Analysis/Tools](https://github.com/desy-cms/analysis-tools) package**

* [Installation](#Installation)
* [Running the analysis](#Running-the-analysis)
  * [Main macro](#Main-macro)
  * [Configuration file](#Configuration-file)
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


## MssmHbbAnalyser class

:warning: To do




