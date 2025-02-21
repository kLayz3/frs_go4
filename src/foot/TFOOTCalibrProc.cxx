#include "TFOOTCalibrProc.h"
#include <algorithm>
#include "TFOOTSortEvent.h"
#include "TFOOTCalibrEvent.h"
#include "TH2D.h"
#include <TGo4AnalysisImp.h>
#include "TFile.h"
#include "TGraph.h"

TFOOTCalibrProc::TFOOTCalibrProc()
{
	par = dynamic_cast<TFOOTParameter *>(TGo4Analysis::Instance()->GetParameter("FOOTPar"));

	// par = dynamic_cast<TFOOTParameter *>(TGo4Analysis::Instance()->GetParameter("FOOTPar"));

	for (int i = 0; i < 8; i++)
	{
		// data.at(i).ReadCalib(Form("%s%1d.clb",par->cal_prefix.Data(),i));
		// std::cout << "Call of ReadCalibFromROOTfile " << i << std::endl;
		// data.at(i).ReadCalibFromROOTfile("FOOT_HG.root", i + 1);
		// ReadCalibParsFromROOTfile("FOOT_HG.root", i + 1);
		ReadCalibParsFromROOTfile("FOOT_HG.root", i);
	}

	// for (Int_t i = 0; i < FOOT_CHN; i++) {
	// 	std::cout << badStrip[0][i] << std::endl;
	// }

	// PrintCalibPars(1);
	PrintCalibPars(7);

	CreateHistograms();
}

TFOOTCalibrProc::~TFOOTCalibrProc()
{
	;
}

void TFOOTCalibrProc::FillEvent(TFOOTSortEvent *srcEvent, TFOOTCalibrEvent *tgtEvent)
{

	// TODO: check if events!=0
	sortEvent = srcEvent;
	calibEvent = tgtEvent;

	//	loop over all detectors
	// TODO: change to constant

	// for (Int_t i = 0; i < FOOT_CHN; i++) {
	// 	std::cout << badStrip[0][i] << std::endl;
	// }

	for (int i = 0; i < 8; i++)
	{
		SetAmp(i);
	}
	
	FillHist();
}

void TFOOTCalibrProc::ReadCalibParsFromROOTfile(const char *file, Int_t i)
{

	par = dynamic_cast<TFOOTParameter *>(TGo4Analysis::Instance()->GetParameter("FOOTPar"));

	const Int_t detPosition = i + 1;

	// open files with saved graphs with pedestals
	TFile *parFile = new TFile(file, "READ");
	if (!parFile || parFile->IsZombie())
	{
		std::cerr << "Error: Unable to open input file " << parFile << std::endl;
		return;
	}

	// take a graph
	TGraph *pedestalsGraph = (TGraph *)parFile->Get(Form("pedestalsFOOT%d", detPosition));
	if (!pedestalsGraph)
	{
		std::cerr << "Error: Unable to get graph \"pedestalsFOOT" << detPosition
				  << "\" from input file " << parFile << std::endl;
		return;
	}

	TGraph *pedestalsGraphSigma = (TGraph *)parFile->Get(Form("pedestalsSigmaFOOT%d", detPosition));
	if (!pedestalsGraphSigma)
	{
		std::cerr << "Error: Unable to get graph \"pedestalsFOOT" << detPosition << "\" from input file " << parFile << std::endl;
		return;
	}

	Double_t *pedestal = pedestalsGraph->GetY();
	Double_t *pedestalSigma = pedestalsGraphSigma->GetY();
	// pedestal[i] = pedestalsGraph->GetY();
	// pedestalSigma[i] = pedestalsGraphSigma->GetY();

	// filling of parameters
	for (size_t j = 0; j < FOOT_CHN; j++)
	{
		// std::cout << badStrip[i][j] << std::endl;
		C0[i][j] = pedestal[j];
		if (pedestal[j] > 0)
			badStrip[i][j] = 0;
		else
			badStrip[i][j] = 1;

		threshold[i][j] = 40. * pedestalSigma[j];

		// std::cout << badStrip[i][j] << std::endl;

		// threshold[j] = par->thresholdsInSigmas[i] * pedestalSigma[j];
	}
	parFile->Close();
}

void TFOOTCalibrProc::PrintCalibPars(Int_t detNumber)
{

	// TODO: error handling for detNumber>8

	// for (size_t j = 0; j < 0; j++)
	for (size_t j = 0; j < FOOT_CHN / 10; j++)
	{
		std::cout << "detector " << detNumber
				  << " pedestal:\t" << C0[detNumber][j]
				  //   << " thr.:\t" << threshold[detNumber][j]
				  << " badStrip:\t" << badStrip[detNumber][j]
				  << std::endl;
	}
}

void TFOOTCalibrProc::SetAmp(Int_t detPosition)
{
	// TODO: document this function

	for (int i = 0; i < FOOT_CHN; i++)
	{

		Double_t rawAmp = static_cast<Double_t>(sortEvent->GetDetectorSorted(detPosition)[i]);

		// std::cout << detPosition
		// 		  << "\t" << i
		// 		//   << "\t" << outEvent->data.at(detPosition).Amp[i]
		// 		  << "\tempty Amp[i]"
		// 		  //   << "\t" << inEvent->GetDetectorSorted(detPosition)[i]
		// 		  << "\t" << rawAmp
		// 		  << "\t" << C0[detPosition][i]
		// 		  << "\t" << badStrip[detPosition][i]
		// 		  //   << std::endl
		// 		  << std::endl;

		if (!badStrip[detPosition][i])
		{
			calibEvent->data.at(detPosition).Amp[i] = rawAmp - C0[detPosition][i];
			// Amp[i] = data[i] * 1.0 - C0[i];
		}
		else
		{
			calibEvent->data.at(detPosition).Amp[i] = 0.;
		}

		// std::cout << detPosition
		// 		  << "\t" << i
		// 		  << "\t" << outEvent->data.at(detPosition).Amp[i]
		// 		  //   << "\t" << inEvent->GetDetectorSorted(detPosition)[i]
		// 		  << "\t" << rawAmp
		// 		  << "\t" << C0[detPosition][i]
		// 		  << "\t" << badStrip[detPosition][i]
		// 		  //   << std::endl
		// 		  << std::endl;
	}

	//TODO: change 10 to constant (number of ASICS)
	for (int i = 0; i < 10; i++)
	{
		// ASICShift[detPosition][i] = GetASICShift(detPosition, i);
	}
	// for (int i = 0; i < FOOT_CHN; i++)
	// {
	// 	if (!bad[i])
	// 	{
	// 		Amp[i] -= ASICShift[i / FOOT_ASIC_LEN];
	// 	}
	// }

	// // std::cout << outEvent->data.at(detPosition).Amp[320] << "\t" << inEvent->GetDetectorSorted(detPosition)[320] << "\t" << C0[detPosition][320] << std::endl
	// 		  << std::endl;
}

void TFOOTCalibrProc::BaseLineCorrection(Int_t detPosition)
{

	std::array<short,64> chip_vals;
    // std::copy(start,start+64,chip_vals.begin());
    
    // std::nth_element(chip_vals.begin(),
	// 	     chip_vals.begin()+31,
	// 	     chip_vals.end());
    
    // short medval= (chip_vals[31]);
    // for (int j=0;j<64;j++)
    //   start[j]-=medval;

}

double TFOOTCalibrProc::GetASICShift(Int_t detPosition, Int_t asicsNumber)
{
	//TODO: write comments to this function and variables

	//TODO: baseline correction
	//	1) this function
	//	2) alternative approach - Pavel's function
	double res = 0;
	int n = 0;
	for (int j = asicsNumber; j < asicsNumber + FOOT_ASIC_LEN; j++)
	{
		// if ((!bad[detPosition][j]) && (Amp[detPosition][asicsNumber] < threshold[asicsNumber]))
		if ((!badStrip[detPosition][j]) && (calibEvent->data.at(detPosition).Amp[j] < threshold[detPosition][j]))
		
		{
			res += calibEvent->data.at(detPosition).Amp[j];
			n++;
		}
	}
	if (n == 0)
	{
		return (0.0);
	}
	else
	{
		return (res / n);
	}
}

void TFOOTCalibrProc::CreateHistograms()
{
	char dir[] = "FOOT/Calibrated";
	for (int i = 0; i < 8; i++)
	{
		hcalamp[i] = new TH2D(Form("amp_ch_%1d", i + 1),
							  Form("FOOT  Amp. vs. ch. layer #%1d", i + 1),
							  FOOT_CHN, 0, FOOT_CHN,
							  FOOT_ADC_BINS + 400, -400., FOOT_ADC_MAX);
		hcalamp[i]->SetMarkerColor(1);
		hcalamp[i]->SetXTitle("channel");
		hcalamp[i]->SetYTitle("ADC val.");
		TGo4Analysis::Instance()->AddHistogram(hcalamp[i], dir);
		//
	}
	for (int i = 0; i < 8; i++)
	{
		hposE[i] = new TH2D(Form("cl_pos_E_%1d", i),
							Form("Cluster pos.  vs. cluster dep. #%1d", i),
							FOOT_CHN, 0, FOOT_CHN,
							FOOT_ADC_BINS, 0, FOOT_ADC_MAX);
		hposE[i]->SetMarkerColor(1);
		hposE[i]->SetXTitle("position (a. u.)");
		hposE[i]->SetYTitle("Energy deposit (a. u.)");
		TGo4Analysis::Instance()->AddHistogram(hposE[i], dir);
	}
	hmult = new TH2I("hmult",
					 "FOOT mult. by layer",
					 640, 0, FOOT_CHN,
					 8, 0, 8);
	hmult->SetMarkerColor(1);
	hmult->SetXTitle("Multiplicity");
	hmult->SetYTitle("FOOT Layer");
	TGo4Analysis::Instance()->AddHistogram(hmult, dir);

	hclmult = new TH2I("hclmult",
					   "FOOT mult. by layer",
					   640, 0, FOOT_CHN,
					   8, 0, 8);
	hclmult->SetXTitle("Cluster Multiplicity");
	hclmult->SetYTitle("FOOT Layer");
	TGo4Analysis::Instance()->AddHistogram(hclmult, dir);

	hpos = new TH2D("hclpos",
					"Cluster position by layer",
					FOOT_CHN, 0, FOOT_CHN,
					8, 0, 8);
	hpos->SetXTitle("position (a. u.)");
	hpos->SetYTitle("FOOT Layer");
	TGo4Analysis::Instance()->AddHistogram(hpos, dir);
	for (int i = 0; i < 8; i++)
	{
		hclsize[i] = new TH2D(Form("cl_len_E_%1d", i),
							  Form("Cluster length  vs. cluster dep. #%1d", i),
							  FOOT_CHN, 0, FOOT_CHN,
							  FOOT_ADC_BINS, 0, FOOT_ADC_MAX);
		hclsize[i]->SetMarkerColor(1);
		hclsize[i]->SetXTitle("cluster length (a. u.)");
		hclsize[i]->SetYTitle("Energy deposit (a. u.)");
		TGo4Analysis::Instance()->AddHistogram(hclsize[i], dir);
	}
}

void TFOOTCalibrProc::FillHist()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < FOOT_CHN; j++)
		{
			// hcalamp[i]->Fill(j, outEvent->data.at(i).Amp[j]);
			hcalamp[i]->Fill(j, calibEvent->data.at(i).Amp[j]);
		}

		// hmult->Fill(outEvent->data.at(i).mult, i);
		// hclmult->Fill(outEvent->data.at(i).clmult, i);
		// if (outEvent->data.at(i).clmult > 0)
		// {
		// 	for (UInt_t j = 0; j < outEvent->data.at(i).clmult; j++)
		// 	{
		// 		hpos->Fill(outEvent->data.at(i).clpos[j], i);
		// 		hposE[i]->Fill(outEvent->data.at(i).clpos[j],
		// 					   outEvent->data.at(i).clE[j]);
		// 		hclsize[i]->Fill(outEvent->data.at(i).cllast[j] -
		// 							 outEvent->data.at(i).clfirst[j] + 1,
		// 						 outEvent->data.at(i).clE[j]);
		// 	}
		// }
	};
}
