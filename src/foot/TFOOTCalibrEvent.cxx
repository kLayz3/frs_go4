#include "TFOOTCalibrEvent.h"
#include <algorithm>
#include <TGo4AnalysisImp.h>


TFOOTCalibrEvent::TFOOTCalibrEvent()
{
  ReadCalib();
}

TFOOTCalibrEvent::~TFOOTCalibrEvent()
{;}

void TFOOTCalibrEvent::ReadCalib()
{
  par=  dynamic_cast<TFOOTParameter*>
    (TGo4Analysis::Instance()->GetParameter("FOOTPar"));
  for(int i=0;i<8;i++)
    {
      data.at(i).ReadCalib(Form("%s%1d.clb",par->cal_prefix.Data(),i));
    }
}


ClassImp(TFOOTCalibrEvent)
