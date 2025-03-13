// -*- mode:C++ -*-

#ifndef TFOOTCALIBRPROC_H
#define TFOOTCALIBRPROC_H

#include "Riostream.h"
#include "TObjString.h"
#include "TFOOTParameter.h"
#include "foot_common.hh"
#include <vector>
#include <RtypesCore.h>	// TODO: is it neccessary?

class TFOOTSortEvent;
class TFOOTCalibrEvent;
class TH1;
class TH2D;
class TH2I;

class TFOOTCalibrProc
{
public:
	TFOOTCalibrProc();
	virtual ~TFOOTCalibrProc();

	TH2D *hcalamp[FOOT_DETECTORS_USED];		//!
	TH2D *hcalampCorr[FOOT_DETECTORS_USED]; //!
	TH1 *hmult;								//!
	TH1 *hmultStrip;						//!
	TH1 *hmultCluster;						//!
	TH1 *hClusterEnergy;					//!
	TH2I *hSingleClusterPositionX;			//!
	TH2I *hSingleClusterPositionY;			//!

	// TODO: change 4 to constant
	TH2I *hClusterProjection[4]; //!
	TH2I *hSingleClusterCorrXX[3]; //!
	TH2I *hSingleClusterCorrYY[3]; //!
	TH2I *hSingleClusterProjectionYY[4]; //!

	TH1 *hclmult;					   //!
	TH1 *hpos;						   //!
	// TH1 *hposE[FOOT_DETECTORS_USED];   //!
	TH2I *hClusterLengthDeposit[FOOT_DETECTORS_USED]; //!

	TFOOTParameter *par;
	void FillEvent(TFOOTSortEvent *srcEvent, TFOOTCalibrEvent *tgtEvent);

	void ReadCalibParsFromROOTfile(const char *file, Int_t i); // TODO: change Int_t i to something reasonable
	void PrintCalibPars(Int_t detNumber);

private:

	void SetAmp(Int_t detPosition);
	// void BaseLineCorrection(Int_t detPosition, Short_t *start);
	void BaseLineCorrection(Int_t detPosition);
	void BaseLineCorrectionASIC(Int_t detPosition, Double_t *start);
	// double GetASICShift(Int_t detPosition, Int_t asicsNumber);

	void SetStripMultiplicity(Int_t detPosition);

	// work with clusters
	void ResizeEventData(Int_t detector_index);
	void FindStripsAboveThreshold(Int_t detector_index);
	void GroupStrips(Int_t detector_index);
	void ResizeAfterClusterFind(Int_t detector_index);
	void ComputeClusterProperties(Int_t detector_index);
	// void SortClustersByEnergy(Int_t detector_index);

	void CreateHistograms();
	void FillHist();

	TFOOTSortEvent *sortEvent;
	TFOOTCalibrEvent *calibEvent;

	// Double_t *pedestal[8];			//!
	// Double_t *pedestalSigma[8];		//!
	Bool_t badStrip[FOOT_DETECTORS_USED][FOOT_CHN]; //!

	// pedestals
	Double_t C0[FOOT_DETECTORS_USED][FOOT_CHN]; //!

};

#endif // TFOOTCALIBRPROC_H
