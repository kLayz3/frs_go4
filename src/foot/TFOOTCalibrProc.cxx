#include "TFOOTCalibrProc.h"
#include <algorithm>
#include "TFOOTSortEvent.h"
#include "TFOOTCalibrEvent.h"
#include "TH2D.h"
#include <TGo4AnalysisImp.h>

TFOOTCalibrProc::TFOOTCalibrProc()
{
  par=  dynamic_cast<TFOOTParameter*>
    (TGo4Analysis::Instance()->GetParameter("FOOTPar"));
  char dir[]="FOOT/";
  for(int i=0; i<8;i++)
    {
      hcalamp[i]=new TH2I (Form("amp-ch-%1d",i),
		     Form("FOOT  Amp. vs. ch. layer #%1d",i),
		     FOOT_CHN,0,FOOT_CHN,
		     FOOT_ADC_BINS, 0,FOOT_ADC_MAX);
      hcalamp[i]->SetMarkerColor(1);
      hcalamp[i]->SetXTitle("channel");
      hcalamp[i]->SetYTitle("ADC val.");
      TGo4Analysis::Instance()->AddHistogram(hcalamp[i], dir);
    }
}

TFOOTCalibrProc::~TFOOTCalibrProc()
{;}

void  TFOOTCalibrProc::FillEvent(TFOOTCalibrEvent * oev,  TFOOTSortEvent* iev)
{
  for(int i=0;i<8;i++)
    {
      oev->data.at(i).Set( iev->Get1( i));
    }
  FillHist( oev );
}

void  TFOOTCalibrProc::FillHist(TFOOTCalibrEvent* oev )
{
  for (int i =0;i<8;i++)
    {
      for (int j=0;j<FOOT_CHN;j++)
	{
	  hcalamp[i]->Fill(j,oev->data.at(i).Amp[j]);
	}
    }
 ;
}

