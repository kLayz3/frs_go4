// -*- mode:C++ -*-

#include "TFOOTContainer.h"
#include <fstream>
#include <iostream>
#include "TFile.h"
#include "TGraph.h"
#include "TGo4AnalysisImp.h"
#include <algorithm>
#include <cmath>

TFOOTContainer::TFOOTContainer()
{
  ;
}

TFOOTContainer::~TFOOTContainer()
{
  ;
}

void TFOOTContainer::ReadCalib(const char *file)
{
  std::ifstream *input = new std::ifstream(file);
  int itmp;
  std::cout << file << std::endl;
  double ftmp1, ftmp2;
  for (int i = 0; i < FOOT_CHN; i++)
  {
    (*input) >> itmp >> C0[i] >> ftmp1 >> ftmp2 >> bad[i];
    // std::cout << itmp << '\t' << C0[i] << '\t' << bad[i] << std::endl;
    threshold[i] = 2.5 * (ftmp1 + ftmp2);
  }
  input->close();
  delete input;
}

void TFOOTContainer::ReadCalibFromROOTfile(const char *file, Int_t i)
{

  par = dynamic_cast<TFOOTParameter *>(TGo4Analysis::Instance()->GetParameter("FOOTPar"));

  // open files with saved graphs with pedestals
  TFile *parFile = new TFile(file, "READ");
  if (!parFile || parFile->IsZombie())
  {
    std::cerr << "Error: Unable to open input file " << parFile << std::endl;
    return;
  }

  // take a graph
  TGraph *pedestalsGraph = (TGraph *)parFile->Get(Form("pedestalsFOOT%d", i));
  if (!pedestalsGraph)
  {
    std::cerr << "Error: Unable to get graph \"pedestalsFOOT" << i << "\" from input file " << parFile << std::endl;
    return;
  }

  TGraph *pedestalsGraphSigma = (TGraph *)parFile->Get(Form("pedestalsSigmaFOOT%d", i));
  if (!pedestalsGraphSigma)
  {
    std::cerr << "Error: Unable to get graph \"pedestalsFOOT" << i << "\" from input file " << parFile << std::endl;
    return;
  }

  Double_t *pedestal = pedestalsGraph->GetY();
  Double_t *pedestalSigma = pedestalsGraphSigma->GetY();
  for (size_t j = 0; j < 10; j++)
  {
    std::cout << "detector " << i << " pedestal:\t" << pedestal[j] << std::endl;
  }

  // filling of parameters
  for (size_t j = 0; j < FOOT_CHN; j++)
  {
    C0[j] = pedestal[j];
    if (pedestal[j] > 0)
      bad[j] = 0;
    else
      bad[j] = 1;

    threshold[j] = 20. * pedestalSigma[j];

    // threshold[j] = par->thresholdsInSigmas[i] * pedestalSigma[j];
  }

  parFile->Close();
}

void TFOOTContainer::Set(UInt_t *data)
{
  for (int i = 0; i < FOOT_CHN; i++)
  {
    if (!bad[i])
    {
      Amp[i] = data[i] * 1.0 - C0[i];
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
  mult = 0;
  for (int i = 0; i < FOOT_CHN; i++)
  {
    if ((!bad[i]) && (Amp[i] > threshold[i]))
    {
      Ampnth[mult] = Amp[i];
      strip[mult] = i;
      mult++;
    }
  }
}

void TFOOTContainer::FindCluster()
{
  clmult = 0;
  if (mult == 0)
    return; // nothing to search

  clfirst[0] = strip[0];
  cllast[0] = strip[0];
  clmult = 1;

  if (mult >= 1)
  {
    for (UInt_t i = 1; i < mult; i++)
    {
      const int diff = strip[i] - cllast[clmult - 1];
      if (diff < 1)
      {
        std::cerr << "TFOOTContainer:Error in cluster search\n";
        clmult = 0;
        return;
      }
      if ((diff == 1) || ((diff == 2) && bad[strip[i] - 1]))
      {
        /*
    If strip is next to the last cluster or
    located over a bad strip we add one to the last cluster
        */
        cllast[clmult - 1] = strip[i];
      }
      else
      {
        // make new cluster
        clfirst[clmult] = strip[i];
        cllast[clmult] = strip[i];
        clmult++;
      }
    }
  }
 
  // Now we calculate  "charge" in cluster and it position
  if(clmult>0)
    {
      for (UInt_t i = 0; i < clmult; i++)
	{
	  clE[i] = 0;
	  clpos[i] = 0;
	  for (int s = clfirst[i]; s <= cllast[i]; s++)
	    {
	      clE[i] += Amp[s];
	      clpos[i] += (s*Amp[s]);
	    }
	  clpos[i] = clpos[i] /clE[i];
	}
    }
}
UInt_t TFOOTContainer::maxstrip()
{
  //return the number of strip with maximal deposit in Ampnth and strip arrays
  return  std::distance(Ampnth,std::max_element(Ampnth, Ampnth+mult));
}

UInt_t  TFOOTContainer::maxcluster()
{
  return  std::distance(clE,std::max_element(clE, clE+clmult));
}

ClassImp(TFOOTContainer)
