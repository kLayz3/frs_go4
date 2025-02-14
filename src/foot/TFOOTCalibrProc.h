// -*- mode:C++ -*-

#ifndef TFOOTCALIBRPROC_H
#define TFOOTCALIBRPROC_H

#include "Riostream.h"
#include "TObjString.h"
#include "TFOOTParameter.h"
#include "foot_common.hh"

class TFOOTSortEvent;
class TFOOTCalibrEvent;
class TH1;
class TH2D;

class TFOOTCalibrProc
{
public:
	TFOOTCalibrProc();
	virtual ~TFOOTCalibrProc();

	TH1 *hcalamp[FOOT_DETECTORS_USED]; //!
	TH1 *hmult;		 //!
	TH1 *hclmult;	 //!
	TH1 *hpos;		 //!
	TH1 *hposE[FOOT_DETECTORS_USED];	 //!
	TH1 *hclsize[FOOT_DETECTORS_USED]; //!

	TFOOTParameter *par;
	void FillEvent(TFOOTSortEvent *srcEvent, TFOOTCalibrEvent *tgtEvent);

	void ReadCalibParsFromROOTfile(const char *file, Int_t i);	//TODO: change Int_t i to something reasonable
	void PrintCalibPars(Int_t detNumber);

private:

	void SetAmp(Int_t detPosition);
	void BaseLineCorrection(TFOOTCalibrEvent *outEvent, TFOOTSortEvent *inEvent, Int_t detPosition);
	double GetASICShift(Int_t detPosition, Int_t asicsNumber);
	
	void CreateHistograms();
	void FillHist();
	

	TFOOTSortEvent *sortEvent;
	TFOOTCalibrEvent *calibEvent;

	// Double_t *pedestal[8];			//!
	// Double_t *pedestalSigma[8];		//!
	Bool_t badStrip[FOOT_DETECTORS_USED][FOOT_CHN];  //!
	Double_t C0[FOOT_DETECTORS_USED][FOOT_CHN]; //!
	Double_t threshold[FOOT_DETECTORS_USED][FOOT_CHN]; //!
	
	// TODO: change 10 to constant corresponding to number of ASICS
	Double_t ASICShift[FOOT_DETECTORS_USED][10];		  //!

};

#endif // TFOOTCALIBRPROC_H
