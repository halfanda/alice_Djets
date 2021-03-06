//-----------------------------------------------------------------------
//  Author B.Trzeciak
//  Utrecht University
//  barbara.antonina.trzeciak@cern.ch
//-----------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include "TObject.h"
#include "TMath.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "TList.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TSystem.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "THnSparse.h"
#include "TDatabasePDG.h"
#include "AliNormalizationCounter.h"
#include "TStyle.h"
#include "AliHFInvMassFitter.h"
#include "TProfile.h"
#include "AliAnalysisTaskDmesonJets.h"
#include "THStack.h"
#include "TSVDUnfold.h"
#include "TGraphAsymmErrors.h"
#include "TLegendEntry.h"
#include "TPad.h"

//#include "AliHFInvMassFitter.h"

#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldBinByBin.h"

enum DMesonSpecies {kD0toKpi=0, kDStarD0pi=1};
enum Observable {kXsection=0, kFragmentation=1};

    // ========================== Prepare your config ============================================
    int           fSystem = 0;            //-----! 0: pp, 1: p-Pb, Pb-Pb -- set up system
    TString       fSystemS = "pp, #sqrt{#it{s}} = 13 TeV";
    DMesonSpecies fDmesonSpecie = DMesonSpecies(0);
    Observable    fObservable = kFragmentation;
    TString       fDmesonDsc = "Dzero";
    TString       fDmesonS = "D^{0}";
    const double  fRpar = 0.4;           //-----! jet R parameter for your studies (the one that you use in your jet finder!)
    const int     Rpar = 4;
    const int     ND = 4;                //-----!  change these numbers based on how many D mesons you analyse in data !
    const int     NDMC = 2;              //-----!  change these numbers based on how many D mesons you analyse in MC !

    //const double  sigma_in = 0.0512;       //-----! inelastic x-section in bars
    const double  sigma_in = 0.0578;
    const double  nEvScale = 1.063;      //-----! scaling factor from the number of selected events to the number of events needed for the nomrmalization, taken from the D2H normalization counter
    const double  BRDstar = 0.0257;
    const double  BRDzero = 0.0389;
    const int     APb = 208;

    //====== D pT bins ---- set up your D pT bins ======
    const int     fptbinsDN = 12;
    double        fptbinsDA[fptbinsDN+1] = {2,3,4,5,6,7,8,10,12,16,24,36};
    //for PWGJE
 //   const int     fptbinsDN = 7;
  //  double        fptbinsDA[fptbinsDN+1] = {2,4,5,6,7,8,12,24};
    //end PWGJE
    //====== jet pT bins ---- set up your jet pT bins ======
    const int     fptbinsJetTrueN = 8;
    double        fptbinsJetTrueA[fptbinsJetTrueN+1] = {5,6,8,10,12,14,20,30,50};
    const int     fptbinsJetMeasN = 11;
   double        fptbinsJetMeasA[fptbinsJetMeasN+1] = {2,3,4,5,6,8,10,12,14,20,30,50};
    //const int     fptbinsJetFinalN = 8;
    //double        fptbinsJetFinalA[fptbinsJetFinalN+1] = { 5,6,8,10,12,14,20,30,50};
   const int     fptbinsJetFinalN = 7;
   double        fptbinsJetFinalA[fptbinsJetFinalN+1] = {0.20,0.40,0.50,0.60,0.70,0.80,0.90,1.00};

   const int     fzptJetMeasN = 5;
   const int     fzbinsJetFinalN[fzptJetMeasN] = {6,7,7,7,7};
   double        fzbinsJetFinalA[fzptJetMeasN][10] = {{0.4,0.5,0.6,0.7,0.8,0.9,1.0},
                                                      {0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.0},
                                                      {0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.0},
                                                      {0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.0},
                                                      {0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.0}};

    double        fzptJetMeasA[fzptJetMeasN+1] = {2,5,7,10,15,50}; //this must be changed in run.csh as WELL !!!!
    const int     fzptJetTrueN = 5;
    double        fzptJetTrueA[fzptJetTrueN+1] = {0,5,7,10,15,50};
    const int     fzptbinsDN[fzptJetMeasN]={3,5,7,9,10};
    double        fzptbinsDA[fzptJetMeasN][12] = {{2,3,4,5},
                                                 {2,3,4,5,6,7},
                                                 {2,3,4,5,6,7,8,10},
                                                 {2,3,4,5,6,7,8,10,12,15},
                                                 {3,4,5,6,7,8,10,12,16,24,36}};
    const int     fzptbinsDPromptN[fzptJetMeasN]={3,5,7,9,10};
    double        fzptbinsDPromptA[fzptJetMeasN][12] = {{2,3,4,5},
                                                        {2,3,4,5,6,7},
                                                        {2,3,4,5,6,7,8,10},
                                                        {2,3,4,5,6,7,8,10,12,15},
                                                        {3,4,5,6,7,8,10,12,16,24,36},
                                                        };
    /*
   const int     fzptJetMeasN = 1;
   const int     fzbinsJetFinalN[fzptJetMeasN] = {9};
   double        fzbinsJetFinalA[fzptJetMeasN][10] = {{0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0}
                                                      };

    double        fzptJetMeasA[fzptJetMeasN+1] = {5,50}; //this must be changed in run.csh as WELL !!!!
    const int     fzptJetTrueN = 1;
    double        fzptJetTrueA[fzptJetTrueN+1] = {5,50};
    const int     fzptbinsDN[fzptJetMeasN]={7};
    double        fzptbinsDA[fzptJetMeasN][12] = {{2,4,5,6,7,8,12,24}};*/

    //====== z range ---- set up your min, max z ======
    double        zmin = -2, zmax = 2.; // for D-jet pT spectrum
    const int     fzbinsJetMeasN[fzptJetMeasN] = {6,7,7,7,7};
    double        fzbinsJetMeasA[fzptJetMeasN][10] = {
                                                      {0.4,0.5,0.6,0.7,0.8,0.9,1.02},
                                                      {0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.02},
                                                     { 0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.02},
                                                     { 0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.02},
                                                     { 0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.02}};
    const int     fzbinsJetTrueN[fzptJetMeasN] = {6,7,7,7,7};
    double        fzbinsJetTrueAPrompt[fzptJetMeasN][10] = {
                                                      {0.4,0.5,0.6,0.7,0.8,0.9,1.0},
                                                      {0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.0},
                                                     { 0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.0},
                                                     { 0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.0},
                                                     { 0.2,0.4,0.5,0.6,0.7,0.8,0.9,1.0}};

   /* double        fzbinsJetTrueAPrompt[fzptJetMeasN][10] = {{0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0}};
    const int     fzbinsJetMeasN[fzptJetMeasN] = {9};
    double        fzbinsJetMeasA[fzptJetMeasN][10] = {{0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.02}};*/

    //====== signal extraction config ======
    Bool_t        fUseRefl = 1;                      //-----! if to use reflections (for D0, you must have reflections files ready)
    Int_t         fbkgtype = 0;                      //-----! kExpo=0, kLin=1, kPol2=2, kNoBk=3, kPow=4, kPowEx=5
    Float_t       fsigmaSignal = 3;                  //-----! sigma for the signal region
    Float_t       fsigmaBkg[] = {-9,-4,4,9};         //-----! sigma for the SB region (both left and right side from the fit)
    Float_t       fDmass = 1.864, fDsigma = 0.010;   //-----! initial values for D mass and sigma
    Float_t       minf = 1.71, maxf = 2.1;           //-----! min/mass of the inv. mass distributions
    Int_t         fRebinMass = 2;                    //-----! rebining of the inv. mass distributions

    Int_t fColors[] = {1,2,8,4,kOrange-1,6,kGray+1,kCyan+1,kMagenta+2,kGreen+3,kViolet+5,kYellow+2};
    Int_t fMarkers[] = {20,21,22,23,24,25,26,27,28,29,30,32,33,34};


    ///============== POWHEG simulations ============================
    //======= set up here names of your simulation files =======
/*
    TString fRunB[] = {
      "AnalysisResults_FastSim_powheg+pythia6_beauty_150593961473",
      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504284947",
      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504259653",
      "AnalysisResults_FastSim_powheg+pythia6_beauty_1506803374"
    };
*/
    TString fRunB[] = {
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1549024771",	//central
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1549277129",	//mb4.5
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1550062899",	//mb5
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1550137218",	//R05F1
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1549640561",	//R1F05
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1549973280",	//R125
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1554379646",	//R2F1
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1549618884",	//R0.5F0.5
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1554711712",	//R2F2
      "AnalysisResults_FastSim_powheg+pythia6+evtgen_beauty_1554808125",	//pdf 21200
      "AnalysisResults_FastSim_powheg+pythia6_beauty_1504199953"	//pdf 10800
    };

/*
    TString fDescB[] = {
      "central",
      "muR=1,muF=0.5" ,
      "muR=0.5,muF=1",
      "muR=2,muF=1"
    };
*/
    TString fDescB[] = {
      "central",
      "m_{b}=4.5",
      "m_{b}=5",
      "muR=0.5,muF=1",
      "muR=1,muF=0.5" ,
      "muR=1,muF=2",
      "muR=2,muF=1",
      "muR=0.5,muF=0.5",
      "muR=2,muF=2",
      "PDF 21200",
      "PDF 10800"
    };


    TString fRunC[] = {
      "AnalysisResults_FastSim_powheg+pythia6_charm_1549014986",
      "AnalysisResults_FastSim_powheg+pythia6_charm_1556529849",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1556531679",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557130640",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557131019",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557314618",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557315004",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557483108",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557488527",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557603973",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557651402",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557694660",
    "AnalysisResults_FastSim_powheg+pythia6_charm_1557829599",
    };
    TString fDescC[] = {
      "central",
      "m_{c}=1.3",
      "m_{c}=1.7",
      "muR=0.5,muF=1",
      "muR=1,muF=0.5",
      "muR=1,muF=2",
      "muR=2,muF=1",
      "muR=0.5,muF=0.5",
      "muR=2,muF=2",
      "PDF=21200 1",
         "PDF=21200 2",
         "PDF=21200 3",
         "PDF=21200 4",
    };
