#include "TFOOTAnlProc.h"
#include <algorithm>
#include "TFOOTSortEvent.h"
#include "TFOOTCalibrEvent.h"
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

}
TFOOTAnlProc::~TFOOTAnlProc()
{;}

void  TFOOTAnlProc::FillHist1(TFOOTCalibrEvent* ev )
{
  const int nx[3]={1,3,7};
  const int ny[3]={0,2,6};
  for (int i =0;i<2;i++)
    {
      hXXmult[i]->Fill(ev->data.at(nx[i]).mult,
		       ev->data.at(nx[i+1]).mult);
      hYYmult[i]->Fill(ev->data.at(ny[i]).mult,
		       ev->data.at(ny[i+1]).mult);
    }
}


ClassImp(TFOOTAnlProc)
