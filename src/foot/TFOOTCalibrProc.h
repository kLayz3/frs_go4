// -*- mode:C++ -*-

#ifndef TFOOTCALIBRPROC_H
#define TFOOTCALIBRPROC_H


#include "Riostream.h"
#include "TObjString.h"
#include "TFOOTParameter.h"

class TFOOTSortEvent;
class TFOOTCalibrEvent;
class TH1;

class TFOOTCalibrProc
{
public:
  TFOOTCalibrProc();
  virtual ~TFOOTCalibrProc();

  TH1 * hcalamp[8];  //!
  TFOOTParameter *par;
  void  FillEvent(TFOOTCalibrEvent * oev,  TFOOTSortEvent* iev);
private:
  void  FillHist(TFOOTCalibrEvent* oev );
};

#endif  //TFOOTCALIBRPROC_H
