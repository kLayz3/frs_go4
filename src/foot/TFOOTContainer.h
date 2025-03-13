// -*- mode:C++ -*-

#ifndef TFOOTCONTAINER_H
#define TFOOTCONTAINER_H
#include "foot_common.hh"
#include "TObject.h"
#include <vector>
#include "RtypesCore.h"	//TODO: is it necessary?
#include "TFOOTParameter.h"

class TFOOTContainer : public TObject
{
private:
	double GetASICShift(int i);

public:
	TFOOTContainer();
	virtual ~TFOOTContainer();

	void ClearFOOTContainer();

	// TODO: all functions for data processing should be moved to Proc class
	void ReadCalib(const char *file);
	// void ReadCalibFromROOTfile(const char *file, Int_t i);
	void Set(UInt_t *data);

	UInt_t maxstrip();
	UInt_t maxcluster();

	// void EvalMult();
	void FindCluster();

	// TODO: write comments for all variables
	Double_t AmpUncorrected[FOOT_CHN]; // TODO: rename this variable
	Double_t Amp[FOOT_CHN];			   // TODO: rename this variable

	std::vector<Int_t> strip_id_;
	std::vector<Int_t> strip_energy_;
	Int_t multStrip;

	std::vector<Int_t> cluster_number_strips_;			   // Number of strips in a cluster
	std::vector<std::vector<Int_t>> cluster_strip_id_;	   // Strip ID in a cluster
	std::vector<std::vector<Int_t>> cluster_strip_energy_; // Strip energy in a cluster

	std::vector<Double_t> cluster_sigma_;	 // Standard deviation of a cluster position (weighted by the energy)
	std::vector<Double_t> cluster_position_; // Position of a cluster from Weighted Average
	std::vector<Double_t> Eta_;			 // Eta = CoG - int(CoG), where CoG - center-of-gravity of a cluster

	std::vector<Int_t> cluster_energy_summed_; // Sum of energies in a cluster

	Int_t cluster_multiplicity_; // Multiplicity

	UInt_t mult; // number of strips with deposit
	// TODO: what are following variables for?
	//  they should be very probably removed
	Double_t Ampnth[FOOT_CHN]; //[mult]
	Int_t strip[FOOT_CHN];	   //[mult]

	// FIXME: variables moved to Proc class, they shold be deleted from here;
	Bool_t bad[FOOT_CHN];  //!
	Double_t C0[FOOT_CHN]; //!
	// Double_t C1[FOOT_CHN]; //!
	Double_t threshold[FOOT_CHN]; //!

	// TODO: to be moved to Proc class
	// Double_t ASICShift[10]; //!

	UInt_t clmult;
	Int_t clfirst[FOOT_CHN];  //[clmult] first strip in the cluster
	Int_t cllast[FOOT_CHN];	  //[clmult] last strip in the cluster
	Double_t clE[FOOT_CHN];	  //[clmult]
	Double_t clpos[FOOT_CHN]; //[clmult]

	TFOOTParameter *par; //!

	ClassDef(TFOOTContainer, 1)
};

#endif // TFOOTCONTAINER_H
