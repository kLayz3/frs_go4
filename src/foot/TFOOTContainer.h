// -*- mode:C++ -*-

#ifndef TFOOTCONTAINER_H
#define TFOOTCONTAINER_H
#include "foot_common.hh"
#include "TObject.h"
#include <vector>
#include "TFOOTParameter.h"

class TFOOTContainer : public TObject
{
private:
	double GetASICShift(int i);

public:
	TFOOTContainer();
	virtual ~TFOOTContainer();

	// TODO: all functions for data processing should be moved to Proc class
	void ReadCalib(const char *file);
	// void ReadCalibFromROOTfile(const char *file, Int_t i);
	void Set(UInt_t *data);

	UInt_t maxstrip();
	UInt_t maxcluster();

	void EvalMult();
	void FindCluster();

	//TODO: write comments for all variables
	Double_t Amp[FOOT_CHN];		//TODO: rename this variable
	
	//FIXME: variables moved to Proc class, they shold be deleted from here;
	Bool_t bad[FOOT_CHN];  //!
	Double_t C0[FOOT_CHN]; //!
	// Double_t C1[FOOT_CHN]; //!
	Double_t threshold[FOOT_CHN]; //!

	//TODO: to be moved to Proc class
	Double_t ASICShift[10];		  //!

	UInt_t mult;				  // number of strips with deposit
	Double_t Ampnth[FOOT_CHN];	  //[mult]
	Int_t strip[FOOT_CHN];		  //[mult]
	UInt_t clmult;
	Int_t clfirst[FOOT_CHN];  //[clmult] first strip in the cluster
	Int_t cllast[FOOT_CHN];	  //[clmult] last strip in the cluster
	Double_t clE[FOOT_CHN];	  //[clmult]
	Double_t clpos[FOOT_CHN]; //[clmult]

	TFOOTParameter *par; //!

	ClassDef(TFOOTContainer, 1)
};

#endif // TFOOTCONTAINER_H
