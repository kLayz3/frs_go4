////////TPC Y Calibration Parameters Updated///////
#if !defined(__CLING__)

#include "TFRSAnalysis.h"
#include "TFOOTParameter.h"
// #include "TSystem.h"
// #include "TStopwatch.h"

#include <iostream>

using std::cout, std::endl;

#endif

// #include "Riostream.h"

void FOOT_sXXX_YYY_2024_attemptToRediscoverPars()
{
	// look up analysis object and all parameters

	TFRSAnalysis *an = dynamic_cast<TFRSAnalysis *>(TGo4Analysis::Instance());
	if (an == 0)
	{
		cout << "!!!  Script should be run in FRS analysis" << endl;
		return;
	}

	TFOOTParameter *foot = dynamic_cast<TFOOTParameter *>(an->GetParameter("FOOTPar"));
	if (foot == 0)
	{
		cout << "!!!  Parameter FOOTPar not found" << endl;
		return;
	}

	//	TODO: consider name of variable and function "flip"
	//
	//	this parameter change the ordering how two cables are stacked in detector
	//	either (1,2) or (2,1)
	//
	//	if "flip==1" then the order of cable is inversed
	//
	//	flip of the detectors
	foot->flip[0] = 1;
	foot->flip[1] = 0;
	foot->flip[2] = 0;
	foot->flip[3] = 0;
	foot->flip[4] = 0;
	foot->flip[5] = 0;
	foot->flip[6] = 0;
	foot->flip[7] = 0;

	// order of the de10nano before rebuilding the tower
	//  order[0] = 10;
	//  order[1] = 17;
	//  order[2] = 19;
	//  order[3] = 20;
	//  order[4] = 21;
	//  order[5] = 22;
	//  order[6] = 23;
	//  order[7] = 25;

	// TODO: order of the de10nano after rebuilding the tower  on 12th February 2024
	foot->order[0] = 10;
	foot->order[1] = 17;
	foot->order[2] = 19;
	foot->order[3] = 20;
	foot->order[4] = 22;
	foot->order[5] = 25;
	foot->order[6] = 23;
	foot->order[7] = 21;
	// cout << "Focus distance S4: " << frs->dist_focS4 << endl;

	foot->cal_prefix.Form("FOOT");

	cout << "Setup of parameters for FOOT done. " << endl;
}
