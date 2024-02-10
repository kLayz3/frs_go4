// -*- mode:C++ -*-

#ifndef TFOOTCONTAINER_H
#define TFOOTCONTAINER_H
#include "foot_common.hh"
#include "TObject.h"
#include <vector>

class TFOOTContainer :public TObject
{
public:
  TFOOTContainer();
  virtual ~TFOOTContainer();

  void ReadCalib(const char * file);
  void Set(UInt_t * data);

  void Calib();
  Double_t Amp[FOOT_CHN];
  Bool_t bad[FOOT_CHN]; //!
  Double_t C0[FOOT_CHN]; //!
  // Double_t C1[FOOT_CHN]; //! 
  Double_t threshold[FOOT_CHN]; //!
  UInt_t mult;
  Double_t  Ampnth[FOOT_CHN]; //[mult]
  Int_t  strip[FOOT_CHN]; //[mult]
  ClassDef(TFOOTContainer,1)
};


#endif //TFOOTCONTAINER_H

