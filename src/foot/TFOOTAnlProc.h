// -*- mode:C++ -*-

#ifndef TFOOTANLPROC_H
#define TFOOTANLPROC_H

#include "Riostream.h"
#include "TObjString.h"
#include "TFOOTParameter.h"

class TFOOTCalibrEvent;
class TFRSCalibrEvent;
class TFRSAnlEvent;
class TH1;
class TH2D;

class TFOOTAnlProc
{
public:
	TFOOTAnlProc();
	virtual ~TFOOTAnlProc();
	
	void FillHist(TFOOTCalibrEvent *ev, TFRSAnlEvent *ifrsanl, TFRSCalibrEvent *ifrsCal);
private:

	TH2D *hFOOT_tpcX[3];
	TH2D *hFOOT_tpcY[3];

	TH2D *hFOOT_SCI21[8];

	double fSci21_E = 0.;
	double fTpc22_x = -150.;
	double fTpc22_y = -150.;

	TFOOTParameter *par;
	// void  FillEvent(TFOOTCalibrEvent * oev,  TFOOTSortEvent* iev);
	void CreateHistograms();
};

#endif  //TFOOTANLPROC_H
