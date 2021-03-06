//
// Macro to subtract B feed-down from inclusive jet pT spectrum before unfolding
//
// Author: B.Trzeciak (barbara.antonina.trzeciak@cern.ch)
//

#include "config.h"

double plotmin, plotmax; //declared later
Int_t maxObsBinN;
Int_t fBin;



TH1* GetInputHist(TString inFile = "JetPtSpectra_effScale.root", TString histName = "hjetpt",TH1 *hh = nullptr);
TH1* GetInputSimHist(TString inFile, TH1 *hJetPt_B);

void ScaleHist(TH1 *hh, int full = 0);
void setHistoDetails(TH1 *hh, Color_t color, Style_t Mstyle, double Msize = 1.1, Width_t Lwidth = 2, Style_t Lstyle = 1);
void SaveCanvas(TCanvas *c, TString name = "tmp");

void subtractB_afterFolding(TString matrixFile,TH1D *hFD_central_binned,TH1D *hFD_up,TH1D *hFD_down,TH1D *hData_binned,TString outPlotDir,TString outSpectraDir, bool isSys);
void subtractB_beforeFolding(TH1D *hFD_central_binned,TH1D *hFD_up,TH1D *hFD_down,TH1D *hData_binned,TString outPlotDir,TString outSpectraDir, bool isSys);
TH1* GetUpSys(TH1D **hFD, const int nFiles = 11, TH1D *hFD_up = nullptr);
TH1* GetDownSys(TH1D **hFD, const int nFiles = 11, TH1D *hFD_down = nullptr);
TH1* foldB(TString matrixFile, TH1D *hFD, TH1D *folded );


void subtractFD(
TString roounfoldpwd = "",
TString dataFile = "JetPtSpectra_SB_FASTwoSDD_eff_ptD3.root",
TString dataAnalysisFile = "",
TString simDir = "../simulations/POWHEG/out/",
TString comMatrixFile = "",
TString outSpectraDir = "out_FDPythia",
Int_t zBin=0,
TString listName = "",
bool isDptcut = 1,
bool fold = 1, TString outHistName = "ptSpectrumSim_",
bool isSys = 1, bool rebinned = 1,  bool isEff = 1,
bool oldCounter = 0)
{
    if(fObservable == kXsection){
      plotmin = fptbinsJetMeasA[0];
      plotmax = fptbinsJetMeasA[fptbinsJetMeasN];
      maxObsBinN = fptbinsJetMeasN;
    }
    if(fObservable == kFragmentation){
      plotmin = fzbinsJetMeasA[zBin-1][0];
      plotmax = fzbinsJetMeasA[zBin-1][fzbinsJetMeasN[zBin-1]];
      maxObsBinN = fzbinsJetMeasN[zBin-1];
    }
    fBin=zBin;

    gSystem->Load(Form("%s",roounfoldpwd.Data()));

    gStyle->SetOptStat(0000);

    TString outPlotDir = outSpectraDir;
    outPlotDir+="/plots";
    outPlotDir+=Form("%s",(zBin!=0)?Form("%d",zBin):"");
    gSystem->Exec(Form("mkdir %s",outSpectraDir.Data()));
    gSystem->Exec(Form("mkdir %s",outPlotDir.Data()));

    TFile *File = new TFile(dataAnalysisFile,"read");
    TDirectoryFile* dir;
    TList *histList;
    double nEv;
    
    if(oldCounter) {
      dir = dynamic_cast<TDirectoryFile*>(File->Get("DmesonsForJetCorrelations"));
      if(fDmesonSpecie) histList = dynamic_cast<TList*>(dir->Get("histosDStarMBN0"));
      else histList = dynamic_cast<TList*>(dir->Get("histosD0MBN0"));
      TH1F* hEvents = dynamic_cast<TH1F*>(histList->FindObject("hstat"));
      double nEvSel = hEvents->GetBinContent(2);
      double nEvAna = hEvents->GetBinContent(1);
      nEv = nEvScale*nEvSel;
    }
    else {
      dir = dynamic_cast<TDirectoryFile*>(File->Get(Form("PWG3_D2H_DmesonsForJetCorrelations%sMBN0",listName.Data())));
      AliNormalizationCounter *c = dynamic_cast<AliNormalizationCounter*>(dir->Get("NormalizationCounter"));
      nEv = c->GetNEventsForNorm();
    }

    double dataLum = nEv/(sigma_in*1000) ;//Luminosity in mbar
    double simScaling;
    if(fSystem){
      if(fDmesonSpecie) simScaling = BRDstar*APb*dataLum;
      else simScaling = BRDzero*APb*dataLum;
    }
    else {
      if(fDmesonSpecie) simScaling = BRDstar*dataLum;
      else simScaling = BRDzero*dataLum;
    }

    // ----------------- data eff corrected D-jet pT spectrum ---------------------
    TH1D *hData = nullptr;
    hData = dynamic_cast<TH1D*>(GetInputHist(dataFile, "hjetptspectrum", hData));
    TH1D *hData_binned =nullptr;
    if(fObservable == kXsection) hData_binned= dynamic_cast<TH1D*>(hData->Rebin(fptbinsJetMeasN,"hData_binned", fptbinsJetMeasA));
    if(fObservable == kFragmentation) hData_binned= dynamic_cast<TH1D*>(hData->Rebin(fzbinsJetMeasN[zBin-1],"hData_binned", fzbinsJetMeasA[zBin-1]));

    // ----------------- B->D simulation ---------------------
    int cent = 0;
    bool jet = 1; // this code is for jet spectra subtraction

    int simNr = 0; // 0 - central value
    int nFiles = fBsimN;
    TH1D *hFD[fBsimN];
    TH1D *hFD_binned[fBsimN];


    for (int nr=simNr; nr<fBsimN; nr++){
        TString file = simDir;
        file += "/JetPt_";
        file += fRunB[nr];
        if(isDptcut && fObservable == Observable::kXsection) { file += "_Dpt"; file += fptbinsDA[0]; file += "_"; file += fptbinsDA[fptbinsDN];  }
        if(fObservable == Observable::kFragmentation) {
            file += "_Dpt"; file += fzptbinsDA[zBin-1][0]; file += "_"; file += fzptbinsDA[zBin-1][fzptbinsDN[zBin-1]];
            file += "_Jetpt"; file += fzptJetMeasA[zBin-1]; file += "_"; file += fzptJetMeasA[zBin];
        }
        if(isEff) file += "_effScaled";
        if(fDmesonSpecie) file += "_Dstar";
        else file += "_Dzero";
        file += ".root";
        TH1D *htmp = nullptr;
        if(fObservable == kXsection){
            htmp = dynamic_cast<TH1D*>(GetInputHist(file, "hPt", htmp));
            htmp->GetYaxis()->SetTitle("d#sigma/dp_{T} (mb)");
            hFD[nr] = dynamic_cast<TH1D*>(htmp->Clone(Form("hFD_%d",nr)));
            hFD_binned[nr] = dynamic_cast<TH1D*>(htmp->Rebin(fptbinsJetTrueN,Form("hFD_binned_%d",nr),fptbinsJetTrueA));
        }
        if(fObservable == kFragmentation){
            htmp = dynamic_cast<TH1D*>(GetInputHist(file, "hz", htmp));
            htmp->GetYaxis()->SetTitle("d#sigma/dz (mb)");
            hFD[nr] = dynamic_cast<TH1D*>(htmp->Clone(Form("hFD_%d",nr)));
            hFD_binned[nr] = dynamic_cast<TH1D*>(htmp->Rebin(fzbinsJetMeasN[zBin-1],Form("hFD_binned_%d",nr),fzbinsJetMeasA[zBin-1]));
        }

        //htmp->Scale(sigma_c[nr]);



    }


    TH1D *htmp = dynamic_cast<TH1D*>(hFD[cent]->Clone("htmp"));
    TH1D *hFD_central = dynamic_cast<TH1D*>(htmp->Clone("hFD_central"));
    TH1D *hFD_central_binned = nullptr;
    if(fObservable == kXsection)hFD_central_binned=dynamic_cast<TH1D*>(htmp->Rebin(fptbinsJetTrueN,"hFD_central_binned",fptbinsJetTrueA));
    if(fObservable == kFragmentation)hFD_central_binned=dynamic_cast<TH1D*>(htmp->Rebin(fzbinsJetMeasN[zBin-1],"hFD_central_binned",fzbinsJetMeasA[zBin-1]));

    setHistoDetails(hFD_central,4,24);
    setHistoDetails(hFD_central_binned,4,24);

    hFD_central->Scale(simScaling);
    hFD_central_binned->Scale(simScaling);


    TH1D *hFD_up_binned = nullptr;
    TH1D *hFD_down_binned = nullptr;
    TH1D *hFD_up = nullptr;
    TH1D *hFD_down = nullptr;

    if(isSys){
        // ----------------- B->D syst. (rebinned)---------------------
        // get up unc
        hFD_up = dynamic_cast<TH1D*>(hFD_central->Clone("hFD_up"));
        //hFD_up = (TH1D*)GetUpSys(hFD,nFiles,hFD_up);
        setHistoDetails(hFD_up,4,24,0,2,2);
      //  hFD_up->Scale(simScaling);

        // get down unc
        hFD_down = dynamic_cast<TH1D*>(hFD_central->Clone("hFD_down"));
      //  hFD_down = (TH1D*)GetDownSys(hFD,nFiles,hFD_down);
        setHistoDetails(hFD_down,4,24,0,2,2);
      //  hFD_down->Scale(simScaling);

        hFD_up_binned = dynamic_cast<TH1D*>(hFD_central_binned->Clone("hFD_up_binned"));
        hFD_up_binned = dynamic_cast<TH1D*>(GetUpSys(hFD_binned,fBsimN,hFD_up_binned));
        setHistoDetails(hFD_up_binned,4,24,0,2,2);
        hFD_up_binned->Scale(simScaling);

        // get down unc
        hFD_down_binned = dynamic_cast<TH1D*>(hFD_central_binned->Clone("hFD_down_binned"));
        hFD_down_binned = dynamic_cast<TH1D*>(GetDownSys(hFD_binned,fBsimN,hFD_down_binned));
        setHistoDetails(hFD_down_binned,4,24,0,2,2);
        hFD_down_binned->Scale(simScaling);

    }


    if(fold){
         /////////////////////// fold B->D spectra ///////////////////////////////////
        if(rebinned) subtractB_afterFolding(comMatrixFile,hFD_central_binned,hFD_up_binned,hFD_down_binned,hData_binned,outPlotDir,outSpectraDir,isSys);
        else { std::cout << "!!!! DATA AND SIM NEED TO BE REBINED FOR FOLDING !!!!"; return; }
        //else subtractB_afterFolding(bkgRM,detRM,hFD_central,hFD_up,hFD_down,hData,outPlotDir,outSpectraDir,isSys);
    }
    else{
        if(rebinned) subtractB_beforeFolding(hFD_central_binned,hFD_up_binned,hFD_down_binned,hData_binned,outPlotDir,outSpectraDir,isSys);
        else subtractB_beforeFolding(hFD_central,hFD_up,hFD_down,hData,outPlotDir,outSpectraDir,isSys);
    }

    return;
}

void subtractB_afterFolding(TString matrixFile,TH1D *hFD_central_binned,TH1D *hFD_up,TH1D *hFD_down,TH1D *hData_binned,TString outPlotDir,TString outSpectraDir, bool isSys){
    gStyle->SetOptStat(0000);

    hData_binned->SetMinimum(1);
    hData_binned->SetMaximum(hData_binned->GetMaximum()*2);

    TH1D *hFD_central_binned_fold;
    hFD_central_binned_fold = dynamic_cast<TH1D*>(foldB(matrixFile, hFD_central_binned,hFD_central_binned_fold));
    hFD_central_binned_fold->SetName("hFD_central_binned_fold");
    setHistoDetails(hFD_central_binned_fold,4,24);

    TH1D *hData_binned_sub = dynamic_cast<TH1D*>(hData_binned->Clone("hData_binned_sub"));
    hData_binned_sub->Add(hFD_central_binned_fold ,-1);
    //hData_binned_sub->Add(hFD_central_binned ,-1);hh
    hData_binned_sub->SetMarkerStyle(20);

    TH1D *hFD_down_fold = nullptr;
    TH1D *hFD_up_fold = nullptr;
    TH1D *hData_binned_sub_up = nullptr;
    TH1D *hData_binned_sub_down = nullptr;
    TH1D *hFD_ratio_up = nullptr;
    TH1D *hFD_ratio_down = nullptr;

    if(isSys) {
    hData_binned_sub_up = dynamic_cast<TH1D*>(hData_binned->Clone("hData_binned_sub_up"));
    hFD_down_fold = dynamic_cast<TH1D*>(foldB(matrixFile,hFD_down,hFD_down_fold));
    hFD_down_fold->SetName("hFD_down_fold");
    hData_binned_sub_up->Add(hFD_down_fold ,-1);
    setHistoDetails(hFD_down_fold,4,20,0,2,2);
    setHistoDetails(hData_binned_sub_up,2,20,0,2,2);

    hData_binned_sub_down = dynamic_cast<TH1D*>(hData_binned->Clone("hData_binned_sub_down"));
    hFD_up_fold = dynamic_cast<TH1D*>(foldB(matrixFile,hFD_up,hFD_up_fold));
    hFD_up_fold->SetName("hFD_up_fold");
    hData_binned_sub_down->Add(hFD_up_fold ,-1);
    setHistoDetails(hFD_up_fold,4,20,0,2,2);
    setHistoDetails(hData_binned_sub_down,2,20,0,2,2);
    }

    //-------------- data to sim ratio - B feed-down fraction
    TH1D *hFD_ratio = dynamic_cast<TH1D*>(hFD_central_binned_fold->Clone("hFD_ratio"));
    hFD_ratio->Divide(hData_binned);
    hFD_ratio->GetYaxis()->SetTitle("FD raw sim/data");
    hFD_ratio->SetMinimum(0);
    setHistoDetails(hFD_ratio,8,20);


    TH1D *hFDUnc  = nullptr;

    if(isSys){
    hFD_ratio_up = dynamic_cast<TH1D*>(hFD_up_fold->Clone("hFD_ratio_up"));

    hFD_ratio_up->Divide(hData_binned);
    setHistoDetails(hFD_ratio_up,8,24,0,2,2);
    for(int j=1; j<=maxObsBinN; j++ ){
            hFD_ratio_up->SetBinError(j,0);
    }

    hFD_ratio_down = dynamic_cast<TH1D*>(hFD_down_fold->Clone("hFD_ratio_down"));
    hFD_ratio_down->Divide(hData_binned);
    setHistoDetails(hFD_ratio_down,8,24,0,2,2);
     for(int j=1; j<=maxObsBinN; j++ ){
            hFD_ratio_down->SetBinError(j,0);
    }

    //-------------- FD uncertanties
    hFDUnc = dynamic_cast<TH1D*>(hData_binned_sub->Clone("hDFUnc"));
    setHistoDetails(hFDUnc,kMagenta+2,20);
    hFDUnc->GetYaxis()->SetTitle("FD sys. unc");

    for(int j=1; j<=maxObsBinN; j++ ){
            double unc1 = hData_binned_sub_up->GetBinContent(j) - hData_binned_sub->GetBinContent(j);
            double unc2 = hData_binned_sub->GetBinContent(j) - hData_binned_sub_down->GetBinContent(j);
            double unc = 0;
            if(unc1>unc2) unc = unc1;
            else unc = unc2;
            if(hData_binned_sub->GetBinContent(j)>1E-20) unc /= hData_binned_sub->GetBinContent(j);
            hFDUnc->SetBinContent(j,unc);
            hFDUnc->SetBinError(j,0);

            //cout << "unc: " << unc << endl;
    }

    }

    //___________________________________drawing and saving
    TFile *ofile = new TFile(Form("%s/JetPtSpectrum_FDsub%s.root",outSpectraDir.Data(),(fBin!=0)?Form("%d",fBin):""),"RECREATE");
    hData_binned->Write();
    hFD_central_binned->Write();
    hFD_central_binned_fold->Write();
    hFD_ratio->Write();

    if(isSys){
    hData_binned_sub->Write();
    hData_binned_sub_up->Write();
    hData_binned_sub_down->Write();
    hFD_up_fold->Write();
    hFD_down_fold->Write();
    hFD_ratio_up->Write();
    hFD_ratio_down->Write();
    }
    ofile->Close();

     // ------------------ draw sim and data output ---------------

     TPaveText *pvEn= new TPaveText(0.2,0.80,0.8,0.85,"brNDC");
     pvEn->SetFillStyle(0);
     pvEn->SetBorderSize(0);
     pvEn->SetTextFont(42);
     pvEn->SetTextSize(0.045f);
     pvEn->SetTextAlign(11);
     pvEn->AddText(Form("%s",fSystemS.Data()));

     double shift = 0.09;
     double dshift = 0.06;
     Double_t zshiftx = 0;
     Double_t zshifty = 0;
     Double_t letsConfuseEveryoneWithTheseShifts = 0;
     if(fObservable==Observable::kFragmentation){ zshiftx=-0.3;
         zshifty=0.2;
         letsConfuseEveryoneWithTheseShifts=-0.1;
     }
     TPaveText *pvJet = new TPaveText(0.5+zshiftx,0.65-shift+zshifty,0.9+zshiftx,0.7-shift+zshifty,"brNDC");
     pvJet->SetFillStyle(0);
     pvJet->SetBorderSize(0);
     pvJet->SetTextFont(42);
     pvJet->SetTextSize(0.04f);
     pvJet->SetTextAlign(11);
     pvJet->AddText(Form("Charged Jets, Anti-#it{k}_{T}, #it{R} = 0.%d",Rpar));

     shift+=dshift;
     TPaveText *pvD = new TPaveText(0.5+zshiftx,0.65-shift+zshifty,0.9+zshiftx,0.7-shift+zshifty,"brNDC");
     pvD->SetFillStyle(0);
     pvD->SetBorderSize(0);
     pvD->SetTextFont(42);
     pvD->SetTextSize(0.04f);
     pvD->SetTextAlign(11);
     if(fDmesonSpecie) pvD->AddText("with D^{*+} #rightarrow D^{0}#pi^{+} and charge conj.");
     else pvD->AddText("with D^{0} #rightarrow K^{-}#pi^{+} and charge conj.");

     shift+=dshift;
     TPaveText *pvEta = new TPaveText(0.5+zshiftx,0.65-shift+zshifty,0.9+zshiftx,0.7-shift+zshifty,"brNDC");
     pvEta->SetFillStyle(0);
     pvEta->SetBorderSize(0);
     pvEta->SetTextFont(42);
     pvEta->SetTextSize(0.04f);
     pvEta->SetTextAlign(11);
     pvEta->AddText(Form("|#it{#eta}_{jet}| < 0.%d",9-Rpar));

     shift+=dshift;
     TPaveText *pv3 = new TPaveText(0.5+zshiftx,0.65-shift+zshifty+letsConfuseEveryoneWithTheseShifts,0.9+zshiftx,0.7-shift+zshifty,"brNDC");
     pv3->SetFillStyle(0);
     pv3->SetBorderSize(0);
     pv3->SetTextFont(42);
     pv3->SetTextSize(0.04f);
     pv3->SetTextAlign(11);
     pv3->AddText(Form("%d < p_{T,%s} < %d GeV/#it{c}",static_cast<Int_t>(fzptbinsDA[fBin-1][0]),fDmesonS.Data(),static_cast<Int_t>(fzptbinsDA[fBin-1][fzptbinsDN[fBin-1]])));
     if(fObservable==Observable::kFragmentation)pv3->AddText(Form("%d < p_{T,ch. jet} < %d GeV/#it{c}",static_cast<Int_t>(fzptJetMeasA[fBin-1]),static_cast<Int_t>(fzptJetMeasA[fBin])));

    setHistoDetails(hData_binned,kGreen+2,21,1.2,2,1);
    setHistoDetails(hFD_central_binned,kBlue+1,25,1.3,2,2);
     hData_binned->Scale(1,"width");
     hData_binned_sub->Scale(1,"width");
     hFD_central_binned->Scale(1,"width");
     hFD_central_binned_fold->Scale(1,"width");
     if(isSys){
       hFD_up->Scale(1,"width");
       hFD_down->Scale(1,"width");
       hFD_up_fold->Scale(1,"width");
       hFD_down_fold->Scale(1,"width");
       hData_binned_sub_up->Scale(1,"width");
       hData_binned_sub_down->Scale(1,"width");
     }
    // compare data and central sim with unc
    TCanvas *cSpectra = new TCanvas("cSpectra","cSpectra",1000,800);
    cSpectra->SetLogy();
    hData_binned->GetXaxis()->SetRangeUser(plotmin,plotmax);
    hData_binned->Draw();
    //hFD_central_binned ->Draw("same");
    hFD_central_binned_fold ->Draw("same");
    hData_binned_sub->Draw("same");

    if(fObservable == kFragmentation){
        hData_binned->GetYaxis()->SetRangeUser(1E2,1E9);
        if(fBin==4)hData_binned->GetYaxis()->SetRangeUser(1E1,1E8);
    }

    if(isSys){
        //hFD_up->Draw("same");
        //hFD_down->Draw("same");
        hFD_up_fold->Draw("same");
        hFD_down_fold->Draw("same");
        hData_binned_sub_up->Draw("same");
        hData_binned_sub_down->Draw("same");
    }

    pv3->Draw("same");
    pvEn->Draw("same");
    pvD->Draw("same");
    pvJet->Draw("same");
    pvEta->Draw("same");

    TLegend *leg1 = new TLegend(0.55,0.65,0.95,0.89);
    leg1->SetBorderSize(0);
    //leg1->SetStyle(0);
    leg1->AddEntry(hData_binned,"Uncorrected D-Jet yield","p");
    leg1->AddEntry(hFD_central_binned,"B Feed-Down (POWHEG+PYTHIA)","p");
    leg1->AddEntry(hData_binned_sub,"FD corrected yield","p");
    leg1->Draw("same");

    cSpectra->SaveAs(Form("%s/JetPtSpectra_FDsub.png",outPlotDir.Data()));
    cSpectra->SaveAs(Form("%s/JetPtSpectra_FDsub.pdf",outPlotDir.Data()));
    cSpectra->SaveAs(Form("%s/JetPtSpectra_FDsub.svg",outPlotDir.Data()));

    TH1D*JetPtSpectra_FDsubUn = dynamic_cast<TH1D*>(hData_binned_sub->Clone("JetPtSpectra_FDsubUn"));
    JetPtSpectra_FDsubUn->GetYaxis()->SetTitle("Rel. unc.");
    JetPtSpectra_FDsubUn->SetLineColor(kGreen+1);
    JetPtSpectra_FDsubUn->SetMarkerColor(kGreen+1);

    for(int j=1; j<=hData_binned_sub->GetNbinsX();j++){
                double err;
                if(hData_binned_sub->GetBinContent(j)) err = hData_binned_sub->GetBinError(j)/hData_binned_sub->GetBinContent(j);
                else err = 0;
                JetPtSpectra_FDsubUn->SetBinContent(j,err);
                JetPtSpectra_FDsubUn->SetBinError(j,0);
    }

    JetPtSpectra_FDsubUn->SetMaximum(JetPtSpectra_FDsubUn->GetMaximum()*1.2);
    JetPtSpectra_FDsubUn->SetMinimum(0);

    shift+=0.07;
    pv3 = new TPaveText(0.7,0.65-shift,0.9,0.7-shift,"brNDC");
    pv3->SetFillStyle(0);
    pv3->SetBorderSize(0);
    pv3->SetTextFont(42);
    pv3->SetTextSize(0.04f);
    pv3->SetTextAlign(11);
    pv3->AddText(Form("%d < p_{T,%s} < %d GeV/#it{c}",static_cast<Int_t>(fzptbinsDA[fBin][0]),fDmesonS.Data(),static_cast<Int_t>(fzptbinsDA[fBin-1][fzptbinsDN[fBin-1]])));
    if(fObservable==Observable::kFragmentation)pv3->AddText(Form("%d < p_{T,ch. jet} < %d GeV/#it{c}",static_cast<Int_t>(fzptJetMeasA[fBin-1]),static_cast<Int_t>(fzptJetMeasA[fBin-1])));

    TCanvas *cSpectrumRebinUnc = new TCanvas("cSpectrumRebinUnc","cSpectrumRebinUnc",800,500);
    JetPtSpectra_FDsubUn->Draw();
    pv3->Draw("same");

    cSpectrumRebinUnc->SaveAs(Form("%s/JetPtSpectra_FDsubUnc.png",outPlotDir.Data()));
    cSpectrumRebinUnc->SaveAs(Form("%s/JetPtSpectra_FDsubUnc.pdf",outPlotDir.Data()));
    cSpectrumRebinUnc->SaveAs(Form("%s/JetPtSpectra_FDsubUnc.svg",outPlotDir.Data()));

    TCanvas *cRatio = new TCanvas("cRatio","cRatio",1000,500);
    hFD_ratio->GetYaxis()->SetRangeUser(0,hFD_ratio->GetMaximum()*2);
    hFD_ratio->GetXaxis()->SetRangeUser(plotmin,plotmax);
    hFD_ratio->Draw();
    if(isSys) hFD_ratio_up->Draw("same");
    if(isSys) hFD_ratio_down->Draw("same");


    cRatio->SaveAs(Form("%s/FDratio.png",outPlotDir.Data()));
    cRatio->SaveAs(Form("%s/FDratio.pdf",outPlotDir.Data()));
    cRatio->SaveAs(Form("%s/FDratio.svg",outPlotDir.Data()));

    if(isSys){
        TCanvas *cFDUnc = new TCanvas("cFDUnc","cFDUnc",1000,500);
        hFDUnc->GetYaxis()->SetRangeUser(0,hFDUnc->GetMaximum()*1.5);
        hFDUnc->GetXaxis()->SetRangeUser(plotmin,plotmax);
        hFDUnc->Draw();
        cFDUnc->SaveAs(Form("%s/FDUnc_beforeUnf.png",outPlotDir.Data()));
        cFDUnc->SaveAs(Form("%s/FDUnc_beforeUnf.pdf",outPlotDir.Data()));
        cFDUnc->SaveAs(Form("%s/FDUnc_beforeUnf.svg",outPlotDir.Data()));
    }

return;
}

void subtractB_beforeFolding(TH1D *hFD_central_binned,TH1D *hFD_up,TH1D *hFD_down,TH1D *hData_binned,TString outPlotDir,TString outSpectraDir, bool isSys){

    gStyle->SetOptStat(0000);

    hData_binned->SetMinimum(1);
    hData_binned->SetMaximum(hData_binned->GetMaximum()*2);

    TH1D *hData_binned_sub = dynamic_cast<TH1D*>(hData_binned->Clone("hData_binned_sub"));
    hData_binned_sub->Add(hFD_central_binned ,-1);
    hData_binned_sub->SetMarkerStyle(24);

    TH1D *hData_binned_sub_up = nullptr;
    TH1D *hData_binned_sub_down = nullptr;

    if(isSys) {
    hData_binned_sub_up = dynamic_cast<TH1D*>(hData_binned->Clone("hData_binned_sub_up"));
    hData_binned_sub_up->Add(hFD_down ,-1);
    setHistoDetails(hData_binned_sub_up,2,24,0,2,2);


    hData_binned_sub_down = dynamic_cast<TH1D*>(hData_binned->Clone("hData_binned_sub_down"));
    hData_binned_sub_down->Add(hFD_up ,-1);
    setHistoDetails(hData_binned_sub_down,2,24,0,2,2);
    }

    //-------------- data to sim ratio - B feed-down fraction
    TH1D *hFD_ratio = dynamic_cast<TH1D*>(hFD_central_binned->Clone("hFD_ratio"));
    hFD_ratio->Divide(hData_binned);
    hFD_ratio->GetYaxis()->SetTitle("data/FD raw sim");
    hFD_ratio->SetMinimum(0);
    setHistoDetails(hFD_ratio,8,20);

    TH1D *hFD_ratio_up = nullptr;
    TH1D *hFD_ratio_down = nullptr;
    TH1D *hFDUnc = nullptr;

    if(isSys){
    hFD_ratio_up = dynamic_cast<TH1D*>(hFD_up->Clone("hFD_ratio_up"));
    hFD_ratio_up->Divide(hData_binned);
    setHistoDetails(hFD_ratio_up,8,24,0,2,2);
    for(int j=1; j<=maxObsBinN; j++ ){
            hFD_ratio_up->SetBinError(j,0);
    }

    hFD_ratio_down = dynamic_cast<TH1D*>(hFD_down->Clone("hFD_ratio_down"));
    hFD_ratio_down->Divide(hData_binned);
    setHistoDetails(hFD_ratio_down,8,24,0,2,2);
     for(int j=1; j<=maxObsBinN; j++ ){
            hFD_ratio_down->SetBinError(j,0);
    }

    //-------------- FD uncertanties
    hFDUnc = dynamic_cast<TH1D*>(hData_binned_sub->Clone("hDFUnc"));
    setHistoDetails(hFDUnc,kMagenta+2,20);
    hFDUnc->GetYaxis()->SetTitle("FD sys. unc");
    for(int j=1; j<=maxObsBinN; j++ ){
            double unc1 = hData_binned_sub_up->GetBinContent(j) - hData_binned_sub->GetBinContent(j);
            double unc2 = hData_binned_sub->GetBinContent(j) - hData_binned_sub_down->GetBinContent(j);
            double unc = 0;
            if(unc1>unc2) unc = unc1;
            else unc = unc2;
            unc /= hData_binned_sub->GetBinContent(j);
            hFDUnc->SetBinContent(j,unc);
            hFDUnc->SetBinError(j,0);
    }

    }

    //___________________________________drawing and saving
    TFile *ofile = new TFile(Form("%s/JetPtSpectrum_FDsub_beforeFolding%s.root",outSpectraDir.Data(),(fBin!=0)?Form("%d",fBin):""),"RECREATE");
    hData_binned->Write();
    hFD_central_binned->Write();
    hFD_ratio->Write();

    if(isSys){
    hData_binned_sub->Write();
    hData_binned_sub_up->Write();
    hData_binned_sub_down->Write();
    hFD_up->Write();
    hFD_down->Write();
    hFD_ratio_up->Write();
    hFD_ratio_down->Write();
    }
    ofile->Close();

     // ------------------ draw sim and data output ---------------

     hData_binned->Scale(1,"width");
     hFD_central_binned->Scale(1,"width");
     hData_binned_sub->Scale(1,"width");
     if(isSys){
     hFD_up->Scale(1,"width");
     hFD_down->Scale(1,"width");

     hData_binned_sub_up->Scale(1,"width");
     hData_binned_sub_down->Scale(1,"width");
     }
    // compare data and central sim with unc
    TCanvas *cSpectra = new TCanvas("cSpectra","cSpectra",1000,800);
    cSpectra->SetLogy();
    hData_binned->GetXaxis()->SetRangeUser(plotmin,plotmax);
    hData_binned->Draw();
    hFD_central_binned ->Draw("same");
    hData_binned_sub->Draw("same");

    if(isSys){
    hFD_up->Draw("same");
    hFD_down->Draw("same");
    hData_binned_sub_up->Draw("same");
    hData_binned_sub_down->Draw("same");
    }

    TLegend *leg1 = new TLegend(0.65,0.7,0.9,0.9);
    leg1->SetBorderSize(0);
    //leg1->SetStyle(0);
    leg1->AddEntry(hData_binned,"data","p");
    leg1->AddEntry(hData_binned_sub,"data-FD","p");
    leg1->AddEntry(hFD_central_binned,"raw sim FD","p");
    leg1->Draw("same");


    cSpectra->SaveAs(Form("%s/JetPtSpectra_FDsub_beforeFolding.png",outPlotDir.Data()));
    cSpectra->SaveAs(Form("%s/JetPtSpectra_FDsub_beforeFolding.pdf",outPlotDir.Data()));
    cSpectra->SaveAs(Form("%s/JetPtSpectra_FDsub_beforeFolding.svg",outPlotDir.Data()));


    TCanvas *cRatio = new TCanvas("cRatio","cRatio",1000,500);
    hFD_ratio->GetXaxis()->SetRangeUser(plotmin,plotmax);
    hFD_ratio->Draw();
    if(isSys) hFD_ratio_up->Draw("same");
    if(isSys) hFD_ratio_down->Draw("same");


    cRatio->SaveAs(Form("%s/FDratio_beforeFolding.png",outPlotDir.Data()));
    cRatio->SaveAs(Form("%s/FDratio_beforeFolding.pdf",outPlotDir.Data()));
    cRatio->SaveAs(Form("%s/FDratio_beforeFolding.svg",outPlotDir.Data()));

    if(isSys){
    TCanvas *cFDUnc = new TCanvas("cFDUnc","cFDUnc",1000,500);
    hFDUnc->GetXaxis()->SetRangeUser(plotmin,plotmax);
    hFDUnc->Draw();
    cFDUnc->SaveAs(Form("%s/FDUnc_beforeUnf_beforeFolding.png",outPlotDir.Data()));
    cFDUnc->SaveAs(Form("%s/FDUnc_beforeUnf_beforeFolding.pdf",outPlotDir.Data()));
    cFDUnc->SaveAs(Form("%s/FDUnc_beforeUnf_beforeFolding.svg",outPlotDir.Data()));
    }

return;
}


TH1* GetUpSys(TH1D **hFD, const int nFiles, TH1D *hFD_up){


        double bin = 0, binerr = 0;
        double max = 0, maxerr = 0;


        for(int j=1; j<=maxObsBinN; j++ ){
            max = hFD[0]->GetBinContent(j);
            for(int i=1;i<nFiles;i++){
                if(hFD[i]->GetBinContent(j) > max){
                        max = hFD[i]->GetBinContent(j);
                        maxerr = hFD[i]->GetBinError(j);
                }

            }
            hFD_up->SetBinContent(j,max);
            hFD_up->SetBinError(j,0);

        }


    return hFD_up;
}

TH1* GetDownSys(TH1D **hFD, const int nFiles, TH1D *hFD_down){


        double bin = 0, binerr = 0;
        double max = 0, maxerr = 0;


        for(int j=1; j<=maxObsBinN; j++ ){
            max = hFD[0]->GetBinContent(j);
            for(int i=1;i<nFiles;i++){
                if(hFD[i]->GetBinContent(j) < max){
                        max = hFD[i]->GetBinContent(j);
                        maxerr = hFD[i]->GetBinError(j);
                }

            }
            hFD_down->SetBinContent(j,max);
            hFD_down->SetBinError(j,0);

        }


    return hFD_down;
}

TH1* foldB(TString matrixFile, TH1D *hFD, TH1D *folded ){

    gStyle->SetOptStat(0000); //Mean and RMS shown

    if(fObservable == kXsection){
      folded = new TH1D("folded","folded",fptbinsJetMeasN,fptbinsJetMeasA);
    }
    if(fObservable == kFragmentation){
      folded = new TH1D("folded","folded",fzbinsJetMeasN[fBin-1],fzbinsJetMeasA[fBin-1]);
    }


    TFile *File = new TFile(matrixFile,"read");
    TH2D *combMatrix0 = dynamic_cast<TH2D*>(File->Get("Matrix"));
    //TH2D *combMatrix0 = (TH2D*)hRMDet0->Clone("combMatrix0");

    RooUnfoldResponse* RooUResp_reb = new RooUnfoldResponse();
    RooUResp_reb->Setup(nullptr,nullptr,combMatrix0);

    folded  = dynamic_cast<TH1D*>(RooUResp_reb->ApplyToTruth(hFD));

    folded->SetLineColor(2);
    folded->SetMarkerColor(2);

    TCanvas *cf = new TCanvas;
    cf->SetLogy();
    //proj[1]->Draw();
    //proj2[1]->Draw("same")
    folded->Draw("ep");
    hFD->Draw("same");
    //folded->Draw("same");

    combMatrix0->SetTitle("");
    TCanvas *cM = new TCanvas;
    cM->SetLogz();
    combMatrix0->Draw("colz");
    //cM->SaveAs("FDCombMatrix.png");

    return folded;

}


TH1* GetInputHist(TString inFile, TString histName, TH1 *hh){

	TFile *jetPtFile = new TFile(inFile,"read");
  hh = dynamic_cast<TH1*>(jetPtFile->Get(histName));

  return hh;

}

void ScaleHist(TH1 *hh, int full){

    if(full){
        hh->Scale(1,"width");
        hh->GetYaxis()->SetTitle("d^{2}#sigma/d#etadp_{T} (mb #it{c}/GeV)");
    }
    else {
        hh->Scale(1,"width");
        hh->GetYaxis()->SetTitle("dN/dp_{T} (#it{c}/GeV)");
    }

}

void setHistoDetails(TH1 *hh, Color_t color, Style_t Mstyle, double Msize, Width_t Lwidth, Style_t Lstyle){

    hh->SetMarkerColor(color);
    hh->SetMarkerStyle(Mstyle);;
    hh->SetLineColor(color);
    hh->SetLineWidth(Lwidth);
    hh->SetMarkerSize(Msize);
    hh->SetLineStyle(Lstyle);
   // hh->SetName(name.c_str());

    hh->SetTitle("");
    if(fObservable == kXsection){
      hh->GetXaxis()->SetTitle("p_{T,ch,jet} (GeV/c)");
    }
    if(fObservable == kFragmentation){
      hh->GetXaxis()->SetTitle("z_{#parallel}");
    }


}

void SaveCanvas(TCanvas *c, TString name){

    c->SaveAs(Form("%s.png",name.Data()));
    c->SaveAs(Form("%s.pdf",name.Data()));
    c->SaveAs(Form("%s.svg",name.Data()));

}
