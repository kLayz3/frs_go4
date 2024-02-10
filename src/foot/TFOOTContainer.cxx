// -*- mode:C++ -*-

#include "TFOOTContainer.h"
#include <fstream>
#include <iostream>
TFOOTContainer::TFOOTContainer()
{;}


TFOOTContainer:: ~TFOOTContainer()
{;}

void TFOOTContainer::ReadCalib(const char * file)
{
  std::ifstream *input= new std::ifstream(file);
  int itmp;
  std::cout<<file<<std::endl;
  double ftmp1,ftmp2;
    for (int i=0;i<FOOT_CHN;i++)
      {
	(*input)>>itmp>>C0[i]>>ftmp1>>ftmp2>>bad[i];
	std::cout<<itmp<<'\t'<<C0[i]<<'\t'<<bad[i]<<std::endl;
	threshold[i]=2.5*(ftmp1+ftmp2);
      }
    input->close();
    delete input;
}

void TFOOTContainer::Set(UInt_t * data)
{
  for (int i=0;i<FOOT_CHN;i++)
    {
      if(!bad[i])
	{
	  Amp[i] = data[i]*1.0- C0[i];
	}
    }
  ;
}

void TFOOTContainer::Calib()
// TO BE REMOVED  
{;}

ClassImp(TFOOTContainer)
