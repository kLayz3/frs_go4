#include "TFOOTAnlProc.h"
#include <algorithm>
#include "TFOOTSortEvent.h"
#include "TFOOTCalibrEvent.h"
#include "TFRSCalibrEvent.h"
#include "TFRSSortEvent.h"
#include "TH2D.h"
#include <TGo4AnalysisImp.h>

TFOOTAnlProc::TFOOTAnlProc()
{
   par=  dynamic_cast<TFOOTParameter*>
    (TGo4Analysis::Instance()->GetParameter("FOOTPar"));
  char dir[]="FOOT/";
  hXXmult[0]=new TH2I ("hXXmult12",
		       "X mult. in st. 1 vs. st. 2",
		       FOOT_CHN,0,FOOT_CHN,
		       FOOT_CHN,0,FOOT_CHN);
  hXXmult[1]=new TH2I ("hXXmult24",
		       "X mult. in st. 2 vs. st. 4",
		       FOOT_CHN,0,FOOT_CHN,
		       FOOT_CHN,0,FOOT_CHN);
  hYYmult[0]=new TH2I ("hYYmult12",
		       "X mult. in st. 1 vs. st. 2",
		       FOOT_CHN,0,FOOT_CHN,
		       FOOT_CHN,0,FOOT_CHN);
  hYYmult[1]=new TH2I ("hYYmult24",
		       "X mult. in st. 2 vs. st. 4",
		       FOOT_CHN,0,FOOT_CHN,
		       FOOT_CHN,0,FOOT_CHN);
  for(int i=0; i<2;i++)
    {
      hXXmult[i]->SetMarkerColor(1);
      hXXmult[i]->SetXTitle("Mult first");
      hXXmult[i]->SetYTitle("Mult second");
      TGo4Analysis::Instance()->AddHistogram(hXXmult[i], dir);
      hYYmult[i]->SetMarkerColor(1);
      hYYmult[i]->SetXTitle("Mult first");
      hYYmult[i]->SetYTitle("Mult second");
      TGo4Analysis::Instance()->AddHistogram(hYYmult[i], dir);
      //
    }
    
  // FOOT correlations with TPCs
  for(int i=0;i<8;i++)
  {
   char fname[100];
   char fn[100];
   sprintf(fname,"TPC22_X_FOOT_%d", i+1);
   sprintf(fn,"Position distribution in X for TPC22 and FOOT_%d", i+1);
   hFOOT_tpcX[i] = new TH2D(fname,fn,300,-120,120,640,0,640);
   hFOOT_tpcX[i]->SetXTitle("TPC22_X [mm]");
   hFOOT_tpcX[i]->SetYTitle("Position (a. u.)");
   TGo4Analysis::Instance()->AddHistogram(hFOOT_tpcX[i], dir);
   
   sprintf(fname,"TPC22_Y_FOOT_%d", i+1);
   sprintf(fn,"Position distribution in Y for TPC22 and FOOT_%d", i+1);
   hFOOT_tpcY[i] = new TH2D(fname,fn,300,-120,120,640,0,640);
   hFOOT_tpcY[i]->SetXTitle("TPC22_Y [mm]");
   hFOOT_tpcY[i]->SetYTitle("Position (a. u.)");
   TGo4Analysis::Instance()->AddHistogram(hFOOT_tpcY[i], dir);
   
   sprintf(fname,"SCI21_E_FOOT_%d", i+1);
   sprintf(fn,"Energy correlation SCI21 and FOOT_%d", i+1);
   hFOOT_SCI21[i] = new TH2D(fname, fn, 600, 0, 3000, FOOT_ADC_BINS, 0, FOOT_ADC_MAX);
   hFOOT_SCI21[i]->SetXTitle("SCI21_E (a.u.)");
   hFOOT_SCI21[i]->SetYTitle("Energy deposit (a. u.)");
   TGo4Analysis::Instance()->AddHistogram(hFOOT_SCI21[i], dir);
  }

}
TFOOTAnlProc::~TFOOTAnlProc()
{;}

void  TFOOTAnlProc::FillHist1(TFOOTCalibrEvent* ev, TFRSSortEvent* ifrsSort, TFRSCalibrEvent* ifrsCal)
{

  fSci21_E = 0.5*(ifrsSort->de_21l+ifrsSort->de_21r);
  fTpc22_x = ifrsCal->tpc_x[1];
  fTpc22_y = ifrsCal->tpc_y[1];

  const int nx[3]={1,3,7};
  const int ny[3]={0,2,6};
  for (int i =0;i<2;i++)
    {
      hXXmult[i]->Fill(ev->data.at(nx[i]).mult,
		       ev->data.at(nx[i+1]).mult);
      hYYmult[i]->Fill(ev->data.at(ny[i]).mult,
		       ev->data.at(ny[i+1]).mult);		       
    }
    
  if (fTpc22_y>-150 && fTpc22_x>-150) 
  {     for(int i =0;i<8;i=i+2)
          hFOOT_tpcY[i]->Fill(fTpc22_y, ev->data.at(i).mult);
        for(int i =1;i<8;i=i+2)
          hFOOT_tpcX[i]->Fill(fTpc22_x, ev->data.at(i).mult);         
  }
  
  double foot_maxener[8] = {0.};
  for (int i =0;i<8;i++)
  for (UInt_t j = 0; j < ev->data.at(i).clmult; j++)
  if (ev->data.at(i).clE[j]>foot_maxener[i]){
       foot_maxener[i] = ev->data.at(i).clE[j];
  }
  
  
  if (fSci21_E>0) 
    for (int i =0;i<8;i++)  
       if(foot_maxener[i]>0)          
           hFOOT_SCI21[i]->Fill(fSci21_E, foot_maxener[i]);
  
}


ClassImp(TFOOTAnlProc)
