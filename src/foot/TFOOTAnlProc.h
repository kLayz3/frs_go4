// -*- mode:C++ -*-

#ifndef TFOOTANLPROC_H
#define TFOOTANLPROC_H


#include "Riostream.h"
#include "TObjString.h"
#include "TFOOTParameter.h"

class TFOOTCalibrEvent;
class TFRSCalibrEvent;
class TFRSAnlEvent;
class TH1;
class TH2D;

class TFOOTAnlProc
{
public:
  TFOOTAnlProc();
  virtual ~TFOOTAnlProc();

  TH1 * hXXmult[2];  //!
  TH1 * hYYmult[2];  //!
  
  
  TH2D *hFOOT_tpcX[8];
  TH2D *hFOOT_tpcY[8];
  
  TH2D *hFOOT_SCI21[8];
  
  double fSci21_E=0.;
  double fTpc22_x=-150.;
  double fTpc22_y=-150.;
  
  
  TFOOTParameter *par;
  //void  FillEvent(TFOOTCalibrEvent * oev,  TFOOTSortEvent* iev);
  //private:
  void  FillHist1(TFOOTCalibrEvent* ev, TFRSAnlEvent* ifrsanl, TFRSCalibrEvent* ifrsCal);
};

#endif  //TFOOTANLPROC_H
