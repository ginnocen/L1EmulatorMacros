#include <TTree.h>
#include <TFile.h>
#include <TChain.h>
#include <TMath.h>
#include <iostream>
#include <TNtuple.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TGraphAsymmErrors.h>
#include <TString.h>
#include <cmath>
#include <iostream>
using namespace std;


#include <TH1D.h>
#include <TCanvas.h>
#include <TFrame.h>

#include "L1EmulatorSimulator.h"

#define BIN_NUM 40;
const int MAXJETS = 8;
//const int nBins = 75;
const int maxPt = 300; // make sure that maxPt/nBins = 2.
const int nBins = maxPt / L1JETSCALE;


double find(TString infname, Double_t pTthes, Double_t effthes, int cent)
{
  TFile* inf = new TFile(infname);
  int i=0,j=0;
  Bool_t L1threshold_FOUND=false;		// "flag" renamed to "L1threshold_FOUND"
  TString ingname;
  for(i=((L1JETSCALE==4) ? 29 : 99);i>=0;i-=1)
  {
    ingname = Form("asymm_pt_%d_%d",i*(L1JETSCALE),cent);
    TGraphAsymmErrors* ga = (TGraphAsymmErrors*)inf->Get(ingname);
    if(!ga) break;
    Double_t vx,vy,intermin=1000000.;
    //////// Kaya's modificiation ////////
    //Double_t vx_selected=-1;
    //Bool_t eff4theRest  =true;	// true if the turn on curve stays 100% for
    // each offline pt larger than the given threshold
    //////// Kaya's modificiation - END ////////
    for(j=0;j<ga->GetN();j++)
    {
      ga->GetPoint(j,vx,vy);
      if(vx<=pTthes)	 //////// Kaya's modificiation ////////
      {
	//cout<<vy<<endl;
	if(vy>=effthes)
	{
	  //vx_selected=vx;
	  //eff4theRest=true;
	  L1threshold_FOUND=true;
	}
	//		  break;		//////// Kaya's modificiation ////////
      }
      if(L1threshold_FOUND)	// an L1 threshold for which turn on curve hits 100% eff. not later than "pTthes".
	// check the efficiency of this L1 threshold for the remaining offline pT as well.
      {
	if(vy<effthes)
	{
	  //eff4theRest=false;
	  L1threshold_FOUND=false;	// we want turn curve to stay at 100% once it hits 100%
	}
      }
      if(TMath::Abs(vx-pTthes)<=intermin)
      {
	intermin = TMath::Abs(vx-pTthes);
      }
    }
    if (L1threshold_FOUND)	// an L1 threshold has been found
    {
      // cout << "An L1 threshold has been found." << endl;
      // cout << "studied offline pT : " << pTthes << endl;
      // cout << "L1 threshold : " << (i*4) << endl;
      // cout << "100% eff. is reached at offline pT : " << vx_selected << endl;
      // cout << "100% eff. for each offline pt larger than the given threshold : " << eff4theRest << endl;

      // if(!flagx)
      // {
      // 	cout<<endl;
      // 	cout<<">>>> WARNING"<<endl;	//////// Kaya's modificiation ////////
      // 	cout<<">>>> Graph <"<<ingname<<"> has no point at "<<pTthes<<"GeV"<<endl;
      // 	cout<<">>>> The closest point is "<<interx<<endl;;
      // 	cout<<">>>> WARNING ENDS"<<endl;	//////// Kaya's modificiation ////////
      // 	cout<<endl;
      // }

      return i*(L1JETSCALE);
    }
  }
  // none of the L1 thresholds matched
  cout<<endl;
  cout<<">>>> ERROR"<<endl;
  cout<<"ERROR: File<"<<infname<<"> has no thredshold for "<<effthes*100<<"% at "<<pTthes<<" GeV/c"<<endl;
  cout<<">>>> ERROR ENDS"<<endl;
  cout<<endl;
  return -1;
}


void findthes(TString inFileName = "Hydjet502_JetResults_zeroWalls.root",TString infn = "hist_Hydjet502_zeroWalls.root",TString outfile = "rate_Hydjet502_zeroWalls", double REQUIRED_EFF_PER_BIN = 1.0, int centrality=0)
{
  TH1::SetDefaultSumw2();

  TFile *inFile = TFile::Open(inFileName);
  TTree *inTree;
  inTree = (TTree*)inFile->Get("L1UpgradeTree");

  Int_t l1_pt[MAXJETS];
  inTree->SetBranchAddress("jet_pt",l1_pt);

  TH1D *counts = new TH1D("counts","counts;Leading L1 Jet p_{T};Count",nBins,0,maxPt);

  long long entries = inTree->GetEntries();
  for(long long i = 0; i < entries; ++i)
  {
    inTree->GetEntry(i);

    double maxl1pt = 0;
    double maxCenJet = l1_pt[0];
    double maxForJet = l1_pt[4];
    maxl1pt = std::max(maxCenJet, maxForJet);

    counts->Fill(maxl1pt);
  }

  TH1D *rate;
  rate = new TH1D("rate",";L1 p_{T};Rate",nBins,0,maxPt);
  double total_integral = counts->Integral();

  //std::cout << "Trigger Value \t Rate @ 30kHz" << std::endl;
  for(int i = 0; i < nBins; i++)
  {
    double j = (double)i*(double)maxPt/(double)nBins;
    double integral = counts->Integral(i+1, nBins);
    rate->Fill(j, (double)integral/total_integral);
    //std::cout << "L1_SingleJet" << j << "\t" << integral/total_integral*30000 << std::endl;
  }

  const int Nthresholds=12;
  double offlinethresholds[Nthresholds]={30,40,50,60,70,80,90,100,110,120,130,140};
  double L1thresholds[Nthresholds]={-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.,-1.};
  double rates[Nthresholds]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};


  std::cout << "Offline Threshold. L1 Threshold. Rate." << std::endl;
  for(int m=0; m<Nthresholds;m++){
    L1thresholds[m]=find(infn, offlinethresholds[m], REQUIRED_EFF_PER_BIN,centrality);
    rates[m]=rate->GetBinContent(int(L1thresholds[m]/(L1JETSCALE))+1)*30000;
    //std::cout<<"offline threshold="<<offlinethresholds[m]<<", L1 threshold="<<L1thresholds[m]<<", rate="<<rates[m]<<std::endl << std::endl;
    std::cout << offlinethresholds[m] << " " << L1thresholds[m] << " " << rates[m] << std::endl;
  }
  TCanvas* c1 = new TCanvas("c1","A Simple Graph with assymetric error bars",200,10,700,500);
  c1->SetFillColor(42);
  c1->SetGrid();
  c1->GetFrame()->SetFillColor(21);
  c1->GetFrame()->SetBorderSize(12);

  Double_t exl[Nthresholds] ={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
  Double_t eyl[Nthresholds] ={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
  Double_t exh[Nthresholds] ={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
  Double_t eyh[Nthresholds] ={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};

  TGraphAsymmErrors *gr = new TGraphAsymmErrors(Nthresholds,offlinethresholds,rates,exl,exh,eyl,eyh);
  gr->SetTitle("TGraphAsymmErrors Example");
  gr->SetMarkerColor(4);
  gr->SetMarkerStyle(21);
  gr->Draw("ALP");

  TFile*foutput=new TFile(Form("%s_cent%d.root",outfile.Data(),centrality),"recreate");
  foutput->cd();
  gr->Write();
  //////// Kaya's modificiation ////////
  rate->Write();
  //////// Kaya's modificiation - END ////////
  foutput->Close();


}

int main(int argc, char **argv)
{
  if(argc == 6)
  {
    findthes(argv[1], argv[2], argv[3], atof(argv[4]), atoi(argv[5]));
    return 0;
  }else  {
    return 1;
  }
}
