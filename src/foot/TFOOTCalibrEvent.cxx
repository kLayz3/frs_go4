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
      // data.at(i).ReadCalib(Form("%s%1d.clb",par->cal_prefix.Data(),i));
      // std::cout << "Call of ReadCalibFromROOTfile " << i << std::endl;
      data.at(i).ReadCalibFromROOTfile("FOOT_HG.root", i+1);
    }
}


ClassImp(TFOOTCalibrEvent)
