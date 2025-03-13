// -*- mode:C++ -*-

#ifndef TFOOTSORTEVENT_H
#define TFOOTSORTEVENT_H


#include "Riostream.h"
#include "TObjString.h"
#include "foot_common.hh"

class TFOOTSortEvent 
{
public:
   TFOOTSortEvent();
  virtual ~TFOOTSortEvent();

  UInt_t FOOTRawCh[8*FOOT_CHN];
  
  void SetDetectorSorted(Int_t detectorPosition, UInt_t * p);
  void SetFlippedDetectorSorted(Int_t detectorPosition, UInt_t * p);
  UInt_t * GetDetectorSorted(Int_t detectorPosition);

  //TODO: rename function, check where is it called.
  void ClearSortEvent();

  ClassDef(TFOOTSortEvent,1)
};

#endif //TFOOTSORTEVENT_H
