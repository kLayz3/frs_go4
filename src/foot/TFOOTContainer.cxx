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
      else
	{
	  Amp[i] = 0.0;
	}
    }
  EvalMult();
  FindCluster();
 
}

void TFOOTContainer::EvalMult()
{
  mult=0;
  for (int i=0;i<FOOT_CHN;i++)
    {
      if((!bad[i])&&(Amp[i]>threshold[i]))
	{
	  Ampnth[mult]=Amp[i];strip[mult]=i;
	  mult++;
	}
    }
}

void TFOOTContainer::FindCluster()
{
  clmult=0;
  if (mult==0) return; // nothing to search

  clfirst[0]=strip[0]; 
  cllast[0]=strip[0];
  clmult=1;
  if (mult>=1)
    {
      for(UInt_t i=1;i<mult;i++)
	{
	  const int diff=strip[i]-cllast[clmult-1];
	  if(diff < 1)
	    {
	      std::cerr<<"TFOOTContainer:Error in cluster search\n";
	      clmult=0;
	      return;
	    }
	  if((diff==1)||((diff==2)&&bad[strip[i]-1]))
	    {
	      /*
		If strip is next to the last cluster or
		located over a bad strip we add one to the last cluster
	      */
	      cllast[clmult-1]=strip[i];
	    }
	  else
	    {
	      // make new cluster
	      clfirst[clmult]=strip[i]; 
	      cllast[clmult]=strip[i];
	      clmult++;
	    }
	}
    }
  //Now we calculate  "charge" in cluster and it position
  for (UInt_t i =0;i<clmult;i++)
    {
      clE[i]=0;
      clpos[i]=0;
      for (int s=clfirst[i];s<=cllast[i];i++)
	{
	  clE[i]+=Amp[s];
	  clpos[i]+=Amp[s]*s;
	}
      clpos[i]=clpos[i]/clE[i];
    }
}
void TFOOTContainer::Calib()
// TO BE REMOVED  
{;}

ClassImp(TFOOTContainer)