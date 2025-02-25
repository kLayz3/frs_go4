// -*- mode:C++ -*-

#include "TFOOTParameter.h"
#include "Riostream.h"
#include "TObjString.h"
#include <algorithm>

TFOOTParameter::TFOOTParameter() : TGo4Parameter("FOOTParameter")
{
  for (int i = 0; i < 8; i++)
  {
    order[i] = i + 10;
    flip[i] = false;
    thresholdsInSigmas[i] = 0.;
  }
  cal_prefix.Form("FOOT");
  pedestalROOTfile.Form("FOOT_HG.root");
}

TFOOTParameter::TFOOTParameter(const char *name) : TGo4Parameter(name)
{
  for (int i = 0; i < 8; i++)
  {
    order[i] = i + 10;
    flip[i] = false;
    thresholdsInSigmas[i] = 0.;
  }
  cal_prefix.Form("FOOT");
  pedestalROOTfile.Form("FOOT_HG.root");
}

TFOOTParameter::~TFOOTParameter()
{
  ;
}

void TFOOTParameter::Print(Option_t *) const
{
  std::cout << "order\t";
  for (int i = 0; i < 8; i++)
    std::cout << ' ' << order[i];
  std::cout << std::endl;
  std::cout << "flip\t";
  for (int i = 0; i < 8; i++)
    std::cout << ' ' << flip[i];
  std::cout << std::endl;

  std::cout << std::endl;
  std::cout << "thresholds\t";
  for (int i = 0; i < 8; i++)
    std::cout << ' ' << thresholdsInSigmas[i];
  std::cout << std::endl;

  std::cout << pedestalROOTfile << std::endl;
}

Bool_t TFOOTParameter::UpdateFrom(TGo4Parameter *pp)
{
  TFOOTParameter *from = dynamic_cast<TFOOTParameter *>(pp);
  if (from == 0)
  {
    std::cerr << "Wrong parameter object: " << pp->ClassName() << std::endl;
    return kFALSE;
  }
  for (int i = 0; i < 8; i++)
  {
    order[i] = from->order[i];
    flip[i] = from->flip[i];
    thresholdsInSigmas[i] = from->thresholdsInSigmas[i];
  }
  // std::copy_n(from->order,8,order);
  // std::copy_n(from->flip,8,flip);

  return true;
}

ClassImp(TFOOTParameter)
