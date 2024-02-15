// -*- mode:C++ -*-

#ifndef TFOOTCONTAINER_H
#define TFOOTCONTAINER_H
#include "foot_common.hh"
#include "TObject.h"
#include <vector>
#include "TFOOTParameter.h"


class TFOOTContainer : public TObject
{
public:
  TFOOTContainer();
  virtual ~TFOOTContainer();

  void ReadCalib(const char *file);
  void ReadCalibFromROOTfile(const char *file, Int_t i);
  void Set(UInt_t *data);

  void Calib();
  void EvalMult();
  void FindCluster();
  Double_t Amp[FOOT_CHN];
  Bool_t bad[FOOT_CHN];  //!
  Double_t C0[FOOT_CHN]; //!
  // Double_t C1[FOOT_CHN]; //!
  Double_t threshold[FOOT_CHN]; //!
  UInt_t mult;                  // number of strips with deposit
  Double_t Ampnth[FOOT_CHN];    //[mult]
  Int_t strip[FOOT_CHN];        //[mult]
  UInt_t clmult;
  Int_t clfirst[FOOT_CHN];  //[clmult] first strip in the cluster
  Int_t cllast[FOOT_CHN];   //[clmult] last strip in the cluster
  Double_t clE[FOOT_CHN];   //[clmult]
  Double_t clpos[FOOT_CHN]; //[clmult]

  TFOOTParameter *par; //!

  ClassDef(TFOOTContainer, 1)
};

#endif // TFOOTCONTAINER_H
