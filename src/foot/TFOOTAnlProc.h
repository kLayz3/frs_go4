// -*- mode:C++ -*-

#ifndef TFOOTANLPROC_H
#define TFOOTANLPROC_H


#include "Riostream.h"
#include "TObjString.h"
#include "TFOOTParameter.h"

class TFOOTCalibrEvent;
class TH1;

class TFOOTAnlProc
{
public:
  TFOOTAnlProc();
  virtual ~TFOOTAnlProc();

  TH1 * hXXmult[2];  //!
  TH1 * hYYmult[2];  //!
  TFOOTParameter *par;
  //void  FillEvent(TFOOTCalibrEvent * oev,  TFOOTSortEvent* iev);
  //private:
  void  FillHist1(TFOOTCalibrEvent* ev );
};

#endif  //TFOOTANLPROC_H
