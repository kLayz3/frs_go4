// -*- mode:C++ -*-

#include "TFOOTParameter.h"
#include "Riostream.h"
#include "TObjString.h"
#include <algorithm>

TFOOTParameter::TFOOTParameter() : TGo4Parameter("FOOTParameter")
{
  for(int i=0;i<8;i++)
    {
      order[i] = i+10;
      flip[i] = false;
    }
  cal_prefix.Form("FOOT");
}

TFOOTParameter::TFOOTParameter(const char* name) : TGo4Parameter(name)
{
  for(int i=0;i<8;i++)
    {
      flip[i] = false;
    }
  cal_prefix.Form("FOOT");
   order[0] = 10;
   order[1] = 17;
   order[2] = 19;
   order[3] = 20;
   order[4] = 21;
   order[5] = 22;
   order[6] = 23;
   order[7] = 25;
}

TFOOTParameter::~TFOOTParameter()
{
  ;
}

void TFOOTParameter::Print(Option_t*) const
{
  std::cout<<"order\t";
  for(int i=0;i<8;i++)
    std::cout<<' '<<order[i];
  std::cout<<std::endl;
  std::cout<<"flip\t";
  for(int i=0;i<8;i++)
    std::cout<<' '<<flip[i];
  std::cout<<std::endl;
  
}

Bool_t TFOOTParameter::UpdateFrom(TGo4Parameter *pp) { 
  TFOOTParameter *from = dynamic_cast<TFOOTParameter*> (pp);
  if (from==0) {
    std::cerr << "Wrong parameter object: "<< pp->ClassName()<< std::endl; 
    return kFALSE;   
  }
   for(int i=0;i<8;i++)
    {
      order[i] = from->order[i];
      flip[i] = from-> flip[i];
    }
  // std::copy_n(from->order,8,order);
  //std::copy_n(from->flip,8,flip);

  return true;
}

ClassImp(TFOOTParameter)