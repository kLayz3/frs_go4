// -*- mode:C++ -*-

#ifndef TFOOTCALIBRPROC_H
#define TFOOTCALIBRPROC_H


#include "Riostream.h"
#include "TObjString.h"
#include "TFOOTParameter.h"

class TFOOTSortEvent;
class TFOOTCalibrEvent;
class TFRSCalibrEvent;
class TFRSSortEvent;
class TH1;
class TH2D;

class TFOOTCalibrProc
{
public:
  TFOOTCalibrProc();
  virtual ~TFOOTCalibrProc();

  TH1 * hcalamp[8];  //!
  TH1 * hmult;  //!
  TH1 * hclmult;  //!
  TH1 * hpos;  //!
  TH1 * hposE[8];  //!
  TH1 * hclsize[8];  //!
    
  TH2D *hFOOT_tpcX[8];
  TH2D *hFOOT_tpcY[8];
  
  TH2D *hFOOT_SCI21[8];
  
  TFOOTParameter *par;
  void  FillEvent(TFOOTCalibrEvent * oev,  TFOOTSortEvent* iev);
  void  FillFootTpcEvent(TFOOTCalibrEvent * oev, TFRSCalibrEvent* ifrsCal, TFRSSortEvent* ifrsSort);
private:
  void  FillHist(TFOOTCalibrEvent* oev);
};

#endif  //TFOOTCALIBRPROC_H
