#include "TFOOTAnlProc.h"
#include <algorithm>
#include "TFOOTSortEvent.h"
#include "TFOOTCalibrEvent.h"
#include "TFRSCalibrEvent.h"
#include "TFRSAnlEvent.h"
#include "TH2D.h"
#include <TGo4AnalysisImp.h>

TFOOTAnlProc::TFOOTAnlProc()
{

	par = dynamic_cast<TFOOTParameter *>(TGo4Analysis::Instance()->GetParameter("FOOTPar"));

	CreateHistograms();
}
TFOOTAnlProc::~TFOOTAnlProc()
{
	;
}

void TFOOTAnlProc::CreateHistograms()
{
	char dir[] = "FOOT/";

	// FOOT correlations with TPCs

	const int nx[3] = {1, 3, 7};
	const int ny[3] = {0, 2, 6};
	for (int i = 0; i < 3; i++)
	{
		char fname[100];
		char fn[100];
		sprintf(fname, "TPC22_FOOT%dX", nx[i] + 1);
		sprintf(fn, "Position distribution in X for TPC22 and FOOT_%d", i + 1);
		hFOOT_tpcX[i] = new TH2D(fname, fn, 300, -120, 120, 640, 0, 640);
		hFOOT_tpcX[i]->SetXTitle("TPC22_X [mm]");
		hFOOT_tpcX[i]->SetYTitle("Position (a. u.)");
		TGo4Analysis::Instance()->AddHistogram(hFOOT_tpcX[i], dir);
	}

	for (int i = 0; i < 3; i++)
	{
		char fname[100];
		char fn[100];
		sprintf(fname, "TPC22_FOOT%dY", ny[i] + 1);
		sprintf(fn, "Position distribution in Y for TPC22 and FOOT_%d", i + 1);
		hFOOT_tpcY[i] = new TH2D(fname, fn, 300, -120, 120, 640, 0, 640);
		hFOOT_tpcY[i]->SetXTitle("TPC22_Y [mm]");
		hFOOT_tpcY[i]->SetYTitle("Position (a. u.)");
		TGo4Analysis::Instance()->AddHistogram(hFOOT_tpcY[i], dir);
	}

	// SCI21

	for (int i = 0; i < 8; i++)
	{
		char fname[100];
		char fn[100];
		sprintf(fname, "SCI21E_FOOT%d", i + 1);
		sprintf(fn, "Energy correlation SCI21 and FOOT_%d", i + 1);
		hFOOT_SCI21[i] = new TH2D(fname, fn, 500, 0, 4100, FOOT_ADC_BINS, 0, FOOT_ADC_MAX);
		hFOOT_SCI21[i]->SetXTitle("SCI21_E (a.u.)");
		hFOOT_SCI21[i]->SetYTitle("Energy deposit (a. u.)");
		TGo4Analysis::Instance()->AddHistogram(hFOOT_SCI21[i], dir);
	}
}

void TFOOTAnlProc::FillHist(TFOOTCalibrEvent *ev, TFRSAnlEvent *ifrsanl, TFRSCalibrEvent *ifrsCal)
{

	// fSci21_E = ifrsanl->sci_e[2];
	fTpc22_x = ifrsCal->tpc_x[1];
	fTpc22_y = ifrsCal->tpc_y[1];

	// std::cout<<"here: "<< fSci21_E <<" "<<fTpc22_x<<" "<<fTpc22_y<<std::endl;

	const int nx[3] = {1, 3, 7};
	const int ny[3] = {0, 2, 6};
	// for (int i = 0; i < 2; i++)
	// {
	//   hXXmult[i]->Fill(ev->data.at(nx[i]).mult,
	//                    ev->data.at(nx[i + 1]).mult);
	//   hYYmult[i]->Fill(ev->data.at(ny[i]).mult,
	//                    ev->data.at(ny[i + 1]).mult);

	//   hXX[i]->Fill(ev->data.at(nx[i]).strip[ev->data.at(nx[i]).maxstrip()],
	//                ev->data.at(nx[i + 1]).strip[ev->data.at(nx[i+1]).maxstrip()]);
	//   hYY[i]->Fill(ev->data.at(ny[i]).strip[ev->data.at(ny[i]).maxstrip()],
	//                ev->data.at(ny[i + 1]).strip[ev->data.at(ny[i + 1]).maxstrip()]);
	// }

	// TODO: check the 9999	
	if (fTpc22_y < 9999 && fTpc22_x < 9999)
	{
		for (int i = 0; i < 3; i++)
		{
			// hFOOT_tpcY[i]->Fill(fTpc22_y, ev->data.at(i).mult);
			if (ev->data.at(ny[i]).cluster_multiplicity_ == 1)
			{
				hFOOT_tpcY[i]->Fill(fTpc22_y, ev->data.at(ny[i]).strip[0]);
				//     // hFOOT_tpcX[i]->Fill(fTpc22_x, ev->data.at(i).mult);
				std::cout << fTpc22_y << std::endl;
			}
			// if (ev->data.at(ny[i]).mult == 1)
				// hFOOT_tpcX[i]->Fill(fTpc22_x, ev->data.at(nx[i]).strip[0]);
		}
	}


	//TODO: check what it could be
	// double foot_maxener[8] = {0.};
	// for (int i = 0; i < 8; i++)
	// 	for (UInt_t j = 0; j < ev->data.at(i).clmult; j++)
	// 		if (ev->data.at(i).clE[j] > foot_maxener[i])
	// 		{
	// 			foot_maxener[i] = ev->data.at(i).clE[j];
	// 		}

	// std::cout<<"here:  "<<foot_maxener[0]<<"  "<<foot_maxener[1]<<std::endl;

	// if (fSci21_E > 0)
	// 	for (int i = 0; i < 8; i++)
	// 		if (foot_maxener[i] > 0)
	// 			hFOOT_SCI21[i]->Fill(fSci21_E, foot_maxener[i] / 100.);
}

ClassImp(TFOOTAnlProc)
