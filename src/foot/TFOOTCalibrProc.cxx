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

	// for (int j = 0; j < 640; j++)
	// {
	// 	B1[j] = short(srcEvent->FOOTRawCh[j + 640 * 0]) - offset[0][j];
	// 	B2[j] = short(srcEvent->FOOTRawCh[j + 640 * 1]) - offset[1][j];
	// 	// B3[j] = short(ev->FOOTRawCh[j + 640 * 2]) - offset[2][j];
	// 	// B4[j] = short(ev->FOOTRawCh[j + 640 * 3]) - offset[3][j];
	// 	// B5[j] = short(ev->FOOTRawCh[j + 640 * 4]) - offset[4][j];
	// 	// B6[j] = short(ev->FOOTRawCh[j + 640 * 5]) - offset[5][j];
	// 	// B7[j] = short(ev->FOOTRawCh[j + 640 * 6]) - offset[6][j];
	// 	// B8[j] = short(ev->FOOTRawCh[j + 640 * 7]) - offset[7][j];
	// }

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

		if (!badStrip[detPosition][i])
		{
			calibEvent->data.at(detPosition).AmpUncorrected[i] = rawAmp - C0[detPosition][i];
		}
		else
		{
			calibEvent->data.at(detPosition).AmpUncorrected[i] = 0.;
		}

	}

	BaseLineCorrection(detPosition);

	// TODO: change 10 to constant (number of ASICS)
	for (int i = 0; i < 10; i++)
	{
		// ASICShift[detPosition][i] = GetASICShift(detPosition, i);
	}

	// for (int i = 0; i < FOOT_CHN; i++)
	// {
	// 	if (!bad[i])
	// 	{
	// 		AmpUncorrected[i] -= ASICShift[i / FOOT_ASIC_LEN];
	// 	}
	// }

	// // std::cout << outEvent->data.at(detPosition).AmpUncorrected[320] << "\t" << inEvent->GetDetectorSorted(detPosition)[320] << "\t" << C0[detPosition][320] << std::endl
	// 		  << std::endl;
}

// TODO: rename start variable
void TFOOTCalibrProc::BaseLineCorrectionASIC(Int_t detPosition, Double_t *start)
{
	//baseline correction for one ASIC

	std::array<short, 64> chip_vals;
	std::copy(start, start + 64, chip_vals.begin());

	std::nth_element(chip_vals.begin(),
					 chip_vals.begin() + 31,
					 chip_vals.end());

	Short_t medval = (chip_vals[31]);

	// TODO: change 64 to parameter
	for (int j = 0; j < 64; j++)
		start[j] -= medval; // TODO: rename start variable
}

void TFOOTCalibrProc::BaseLineCorrection(Int_t detPosition)
{

	std::copy(calibEvent->data.at(detPosition).AmpUncorrected, calibEvent->data.at(detPosition).AmpUncorrected + 640, calibEvent->data.at(detPosition).Amp);
	for (int cn = 0; cn < 10; cn++)
	{
		BaseLineCorrectionASIC(detPosition, calibEvent->data.at(detPosition).Amp + 64 * cn);
	}

}

double TFOOTCalibrProc::GetASICShift(Int_t detPosition, Int_t asicsNumber)
{
	// TODO: write comments to this function and variables

	// TODO: baseline correction
	//	1) this function
	//	2) alternative approach - Pavel's function
	double res = 0;
	int n = 0;
	for (int j = asicsNumber; j < asicsNumber + FOOT_ASIC_LEN; j++)
	{
		// if ((!bad[detPosition][j]) && (AmpUncorrected[detPosition][asicsNumber] < threshold[asicsNumber]))
		if ((!badStrip[detPosition][j]) && (calibEvent->data.at(detPosition).AmpUncorrected[j] < threshold[detPosition][j]))

		{
			res += calibEvent->data.at(detPosition).AmpUncorrected[j];
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
							  Form("FOOT  AmpUncorrected. vs. ch. layer #%1d", i + 1),
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
		hcalampCorr[i] = new TH2D(Form("amp_Corr_ch_%1d", i + 1),
								  Form("FOOT  AmpCorr. vs. ch. layer #%1d", i + 1),
								  FOOT_CHN, 0, FOOT_CHN,
								  FOOT_ADC_BINS + 400, -400., FOOT_ADC_MAX);
		hcalampCorr[i]->SetMarkerColor(1);
		hcalampCorr[i]->SetXTitle("channel");
		hcalampCorr[i]->SetYTitle("ADC val.");
		TGo4Analysis::Instance()->AddHistogram(hcalampCorr[i], dir);
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
	// TODO: change 8 to constant

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < FOOT_CHN; j++)
		{
			// hcalamp[i]->Fill(j, outEvent->data.at(i).AmpUncorrected[j]);
			hcalamp[i]->Fill(j, calibEvent->data.at(i).AmpUncorrected[j]);
		}

		for (int j = 0; j < FOOT_CHN; j++)
		{
			// hcalamp[i]->Fill(j, outEvent->data.at(i).AmpUncorrected[j]);
			hcalampCorr[i]->Fill(j, calibEvent->data.at(i).Amp[j]);
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
