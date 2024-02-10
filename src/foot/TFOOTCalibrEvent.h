// -*- mode:C++ -*-

#ifndef TFOOTCALIBREVENT_H
#define TFOOTCALIBREVENT_H


#include "Riostream.h"
#include "TObjString.h"
#include "TFOOTContainer.h"
#include "TFOOTParameter.h"
#include <array>

class TFOOTCalibrEvent 
{
public:
  TFOOTCalibrEvent();
  virtual ~TFOOTCalibrEvent();

  std::array < TFOOTContainer , 8 > data;
  TFOOTParameter *par; //!
  void ReadCalib();
  
  ClassDef(TFOOTCalibrEvent,1)
};

#endif //TFOOTCALIBREVENT_H
