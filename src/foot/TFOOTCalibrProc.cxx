#include "TFOOTCalibrProc.h"
#include <algorithm>
#include "TFOOTSortEvent.h"
#include "TFOOTCalibrEvent.h"
#include "TH2D.h"
#include <TGo4AnalysisImp.h>
#include "TFile.h"
#include "TGraph.h"
#include "TError.h"
#include <set>

TFOOTCalibrProc::TFOOTCalibrProc()
{
	par = dynamic_cast<TFOOTParameter *>(TGo4Analysis::Instance()->GetParameter("FOOTPar"));

	Info("TFOOTCalibrProc::TFOOTCalibrProc", "Pedestals will be loaded from \"%s\"", par->pedestalROOTfile.Data());

	for (int i = 0; i < 8; i++)
	{
		// data.at(i).ReadCalib(Form("%s%1d.clb",par->cal_prefix.Data(),i));
		// std::cout << "Call of ReadCalibFromROOTfile " << i << std::endl;
		// data.at(i).ReadCalibFromROOTfile("FOOT_HG.root", i + 1);
		// ReadCalibParsFromROOTfile("FOOT_HG.root", i + 1);
		ReadCalibParsFromROOTfile(par->pedestalROOTfile, i);
	}

	// PrintCalibPars(0);
	// PrintCalibPars(7);

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

	for (int i = 0; i < 8; i++)
	{
		SetAmp(i);
	}

	FillHist();
}

void TFOOTCalibrProc::ReadCalibParsFromROOTfile(const char *file, Int_t i)
{

	par = dynamic_cast<TFOOTParameter *>(TGo4Analysis::Instance()->GetParameter("FOOTPar"));

	// const Int_t detPosition = i + 1;
	const Int_t detPosition = i;

	// open files with saved graphs with pedestals
	TFile *parFile = new TFile(file, "READ");
	if (!parFile || parFile->IsZombie())
	{
		std::cerr << "Error: Unable to open input file " << parFile->GetName() << std::endl;
		return;
	}

	// take a graph
	TString pedestalGraphName;
	pedestalGraphName.Form("pedestalGraph_%d", par->order[detPosition]);
	TGraph *pedestalsGraph = (TGraph *)parFile->Get(pedestalGraphName);
	if (!pedestalsGraph)
	{
		std::cerr << "Error: Unable to get graph \"" << pedestalGraphName
				  << "\" from input file " << parFile->GetName() << std::endl;
		return;
	}

	// FIXME: all treating of sigmas are unused and may be probably deleted
	TString pedestalSigmaGraphName;
	pedestalSigmaGraphName.Form("pedestalSigmaGraph_%d", par->order[detPosition]);
	TGraph *pedestalsGraphSigma = (TGraph *)parFile->Get(pedestalSigmaGraphName);
	if (!pedestalsGraphSigma)
	{
		std::cerr << "Error: Unable to get graph \"" << pedestalSigmaGraphName << "\" from input file " << parFile->GetName() << std::endl;
		return;
	}

	Info("TFOOTCalibrProc::ReadCalibParsFromROOTfile", "Graph \"%s\" loaded", pedestalsGraph->GetName());
	Info("TFOOTCalibrProc::ReadCalibParsFromROOTfile", "Graph with sigmas \"%s\" loaded\n", pedestalsGraphSigma->GetName());

	Double_t *pedestal = pedestalsGraph->GetY();
	Double_t *pedestalSigma = pedestalsGraphSigma->GetY();

	// filling of parameters
	if (!par->flip[i])
	{
		std::copy_n(pedestal, FOOT_CHN, C0[i]);
	}
	else
	{
		std::copy_n(pedestal + FOOT_RAW_DATA_WORDS, FOOT_RAW_DATA_WORDS, C0[i]);
		std::copy_n(pedestal, FOOT_RAW_DATA_WORDS, C0[i] + FOOT_RAW_DATA_WORDS);
	}

	for (size_t j = 0; j < FOOT_CHN; j++)
	{
		if (pedestal[j] > 0)
			badStrip[i][j] = 0;
		else
			badStrip[i][j] = 1;
	}

	parFile->Close();
}

void TFOOTCalibrProc::PrintCalibPars(Int_t detNumber)
{

	// TODO: error handling for detNumber>8

	Info("TFOOTCalibrProc::PrintCalibPars", "Parameters for detector %d", detNumber);

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
			if (rawAmp > 0.)
				calibEvent->data.at(detPosition).AmpUncorrected[i] = rawAmp - C0[detPosition][i];
		}
		else
		{
			calibEvent->data.at(detPosition).AmpUncorrected[i] = 0.;
		}
	}

	BaseLineCorrection(detPosition);

	SetStripMultiplicity(detPosition);

	ResizeEventData(detPosition);
	FindStripsAboveThreshold(detPosition);
	GroupStrips(detPosition);
	ResizeAfterClusterFind(detPosition);
	ComputeClusterProperties(detPosition);
}

// TODO: rename start variable
void TFOOTCalibrProc::BaseLineCorrectionASIC(Int_t detPosition, Double_t *start)
{
	// baseline correction for one ASIC

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

void TFOOTCalibrProc::SetStripMultiplicity(Int_t detPosition)
{

	for (int i = 0; i < FOOT_CHN; i++)
	{
		// FIXME: really AmpUncorrected[i] in condition?
		if ((!badStrip[detPosition][i]) && (calibEvent->data.at(detPosition).AmpUncorrected[i] > par->thresholds[detPosition]))
		{
			// Ampnth[mult] = AmpUncorrected[i];
			// strip[mult] = i;
			calibEvent->data.at(detPosition).mult++;
		}
	}
}

void TFOOTCalibrProc::ResizeEventData(Int_t detector_index)
{
	// Resizing to the maximum limit for number of clusters
	const Int_t temp_max_number_clusters = 200;

	calibEvent->data.at(detector_index).cluster_number_strips_.resize(temp_max_number_clusters);
	calibEvent->data.at(detector_index).cluster_strip_id_.resize(temp_max_number_clusters);
	calibEvent->data.at(detector_index).cluster_strip_energy_.resize(temp_max_number_clusters);

	calibEvent->data.at(detector_index).cluster_position_.resize(temp_max_number_clusters);
	// cluster_position_adj1_[iDetector].resize(temp_max_number_clusters);
	// cluster_position_adj2_[iDetector].resize(temp_max_number_clusters);

	calibEvent->data.at(detector_index).cluster_energy_summed_.resize(temp_max_number_clusters);
	// cluster_energy_summed_adj1_[iDetector].resize(temp_max_number_clusters);
	// cluster_energy_summed_adj2_[iDetector].resize(temp_max_number_clusters);

	calibEvent->data.at(detector_index).cluster_sigma_.resize(temp_max_number_clusters);
	// cluster_sigma_adj1_[iDetector].resize(temp_max_number_clusters);
	// cluster_sigma_adj2_[iDetector].resize(temp_max_number_clusters);

	calibEvent->data.at(detector_index).Eta_.resize(temp_max_number_clusters);
	// Eta_adj1_[iDetector].resize(temp_max_number_clusters);
	// Eta_adj2_[iDetector].resize(temp_max_number_clusters);
}

void TFOOTCalibrProc::FindStripsAboveThreshold(Int_t detector_index)
{
	std::set<int> unique_strips;

	TFOOTContainer &container = calibEvent->data.at(detector_index);

	for (Int_t strip = 0; strip < FOOT_CHN; ++strip)
	{
		// std::cout << strip << std::endl;
		// TODO: treat the trigger, it should be very probably taken from FRSEvent
		//  if (/* trigger_pattern_ == chosen_tpat_ && */ data_baseline_corr_[detector_index][strip] > par->thresholds[detector_index])
		// if (calibEvent->data.at(detector_index).Amp[strip] > par->thresholds[detector_index])
		// if (!badStrip[detector_index][strip] && (container.Amp[strip] > par->thresholds[detector_index]))
		if (!badStrip[detector_index][strip] && (calibEvent->data.at(detector_index).Amp[strip] > par->thresholds[detector_index]))
		{
			unique_strips.insert(strip);
		}
	}

	for (const auto &strip : unique_strips)
	{
		// strip_id_[detector_index].push_back(strip);
		// strip_energy_[detector_index].push_back(calibEvent->data.at(detector_index).Amp[strip][detector_index][strip]);
		// container.strip_id_.push_back(strip);
		// container.strip_energy_.push_back(container.Amp[strip]);
		calibEvent->data.at(detector_index).strip_id_.push_back(strip);
		calibEvent->data.at(detector_index).strip_energy_.push_back(calibEvent->data.at(detector_index).Amp[strip]);
	}

	calibEvent->data.at(detector_index).multStrip = (Int_t)calibEvent->data.at(detector_index).strip_id_.size();
}

void TFOOTCalibrProc::GroupStrips(Int_t detector_index)
{

	// Maybe rewrite it in a way that the strip_id and strip_energy entries are not erased, but rather there is a loop over all chosen strips
	Int_t cluster_count = 0;
	Int_t temp_strip_id = 0;

	// Int_t j = 0;
	size_t j = 0;

	while (j < calibEvent->data.at(detector_index).strip_id_.size())
	{
		if (j == 0)
		{
			temp_strip_id = calibEvent->data.at(detector_index).strip_id_[j];
		}

		// j++; // TODO: delete this line
		// std::cout << j << std::endl;
		// std::cout << calibEvent->data.at(detector_index).strip_id_[cluster_count] << std::endl;
		// calibEvent->data.at(detector_index).cluster_number_strips_.push_back(0);
		// std::cout << calibEvent->data.at(detector_index).cluster_number_strips_.size() << std::endl;
		calibEvent->data.at(detector_index).cluster_number_strips_[cluster_count]++;
		// std::cout << calibEvent->data.at(detector_index).cluster_number_strips_[cluster_count] << std::endl;
		// std::cout << calibEvent->data.at(detector_index).strip_id_[j] << std::endl;
		// calibEvent->data.at(detector_index).cluster_strip_id_.push_back(cluster_count);
		// calibEvent->data.at(detector_index).cluster_strip_id_.push_back(calibEvent->data.at(detector_index).strip_id_[j]);
		calibEvent->data.at(detector_index).cluster_strip_id_[cluster_count].push_back(calibEvent->data.at(detector_index).strip_id_[j]);
		calibEvent->data.at(detector_index).cluster_strip_energy_[cluster_count].push_back(calibEvent->data.at(detector_index).strip_energy_[j]);
		// Int_t k = j + 1;
		size_t k = j + 1;
		while (k < calibEvent->data.at(detector_index).strip_id_.size() && (calibEvent->data.at(detector_index).strip_id_[k] - temp_strip_id) == 1)
		{
			calibEvent->data.at(detector_index).cluster_number_strips_[cluster_count]++;
			calibEvent->data.at(detector_index).cluster_strip_id_[cluster_count].push_back(calibEvent->data.at(detector_index).strip_id_[k]);
			calibEvent->data.at(detector_index).cluster_strip_energy_[cluster_count].push_back(calibEvent->data.at(detector_index).strip_energy_[k]);
			temp_strip_id = calibEvent->data.at(detector_index).strip_id_[k];
			calibEvent->data.at(detector_index).strip_id_.erase(calibEvent->data.at(detector_index).strip_id_.begin() + k);
			calibEvent->data.at(detector_index).strip_energy_.erase(calibEvent->data.at(detector_index).strip_energy_.begin() + k);
		}

		calibEvent->data.at(detector_index).strip_id_.erase(calibEvent->data.at(detector_index).strip_id_.begin() + j);
		calibEvent->data.at(detector_index).strip_energy_.erase(calibEvent->data.at(detector_index).strip_energy_.begin() + j);

		cluster_count++;
	}
	calibEvent->data.at(detector_index).cluster_multiplicity_ = cluster_count;
}

void TFOOTCalibrProc::ResizeAfterClusterFind(Int_t detector_index)
{
	calibEvent->data.at(detector_index).cluster_position_.resize(calibEvent->data.at(detector_index).cluster_multiplicity_);
	// cluster_position_adj1_[detector_index].resize(cluster_multiplicity_[detector_index]);
	// cluster_position_adj2_[detector_index].resize(cluster_multiplicity_[detector_index]);

	calibEvent->data.at(detector_index).cluster_energy_summed_.resize(calibEvent->data.at(detector_index).cluster_multiplicity_);
	// cluster_energy_summed_adj1_[detector_index].resize(cluster_multiplicity_[detector_index]);
	// cluster_energy_summed_adj2_[detector_index].resize(cluster_multiplicity_[detector_index]);

	calibEvent->data.at(detector_index).cluster_sigma_.resize(calibEvent->data.at(detector_index).cluster_multiplicity_);
	// cluster_sigma_adj1_[detector_index].resize(cluster_multiplicity_[detector_index]);
	// cluster_sigma_adj2_[detector_index].resize(cluster_multiplicity_[detector_index]);

	// Eta_[detector_index].resize(cluster_multiplicity_[detector_index]);
	// Eta_adj1_[detector_index].resize(cluster_multiplicity_[detector_index]);
	// Eta_adj2_[detector_index].resize(cluster_multiplicity_[detector_index]);

	calibEvent->data.at(detector_index).cluster_number_strips_.resize(calibEvent->data.at(detector_index).cluster_multiplicity_);
	calibEvent->data.at(detector_index).cluster_strip_id_.resize(calibEvent->data.at(detector_index).cluster_multiplicity_);
	calibEvent->data.at(detector_index).cluster_strip_energy_.resize(calibEvent->data.at(detector_index).cluster_multiplicity_);
}

void TFOOTCalibrProc::ComputeClusterProperties(Int_t detector_index)
{

	for (Int_t iCluster = 0; iCluster < calibEvent->data.at(detector_index).cluster_multiplicity_; iCluster++)
	{
		for (Int_t iStrip = 0; iStrip < calibEvent->data.at(detector_index).cluster_number_strips_[iCluster]; iStrip++)
		{

			calibEvent->data.at(detector_index).cluster_energy_summed_[iCluster] += calibEvent->data.at(detector_index).cluster_strip_energy_[iCluster][iStrip];
			calibEvent->data.at(detector_index).cluster_position_[iCluster] += calibEvent->data.at(detector_index).cluster_strip_energy_[iCluster][iStrip] * calibEvent->data.at(detector_index).cluster_strip_id_[iCluster][iStrip];

			// Creating variables with addition of adjacent strips
			//  cluster_energy_summed_adj1_[detector_index][iCluster] += cluster_strip_energy_[detector_index][iCluster][iStrip];
			//  cluster_energy_summed_adj2_[detector_index][iCluster] += cluster_strip_energy_[detector_index][iCluster][iStrip];

			// cluster_position_adj1_[detector_index][iCluster] += cluster_strip_energy_[detector_index][iCluster][iStrip] * cluster_strip_id_[detector_index][iCluster][iStrip];
			// cluster_position_adj2_[detector_index][iCluster] += cluster_strip_energy_[detector_index][iCluster][iStrip] * cluster_strip_id_[detector_index][iCluster][iStrip];

			// //Adding the left strips characteristics
			// if (iStrip == 0) {
			//     if (cluster_strip_id_[detector_index][iCluster][iStrip] > 0 && cluster_strip_id_[detector_index][iCluster][iStrip] < foot_constants::kNumberStrips - 1) {

			//         cluster_position_adj1_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] - 1] * (cluster_strip_id_[detector_index][iCluster][iStrip] - 1);
			//         cluster_position_adj2_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] - 1] * (cluster_strip_id_[detector_index][iCluster][iStrip] - 1);

			//         cluster_energy_summed_adj1_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] - 1];
			//         cluster_energy_summed_adj2_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] - 1];

			//         if (cluster_strip_id_[detector_index][iCluster][iStrip] > 1 && cluster_strip_id_[detector_index][iCluster][iStrip] < foot_constants::kNumberStrips - 2) {
			//             cluster_position_adj2_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] - 2] * (cluster_strip_id_[detector_index][iCluster][iStrip] - 2);

			//             cluster_energy_summed_adj2_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] - 2];
			//         }

			//     }
			// }

			// //Adding the right strips characteristics
			// if (iStrip == (cluster_number_strips_[detector_index][iCluster] - 1)) {
			//     if (cluster_strip_id_[detector_index][iCluster][iStrip] > 0 && cluster_strip_id_[detector_index][iCluster][iStrip] < foot_constants::kNumberStrips - 1) {

			//         cluster_position_adj1_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] + 1] * (cluster_strip_id_[detector_index][iCluster][iStrip] + 1);
			//         cluster_position_adj2_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] + 1] * (cluster_strip_id_[detector_index][iCluster][iStrip] + 1);

			//         cluster_energy_summed_adj1_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] + 1];
			//         cluster_energy_summed_adj2_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] + 1];

			//         if (cluster_strip_id_[detector_index][iCluster][iStrip] > 1 && cluster_strip_id_[detector_index][iCluster][iStrip] < foot_constants::kNumberStrips - 2) {
			//             cluster_position_adj2_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] + 2] * (cluster_strip_id_[detector_index][iCluster][iStrip] + 2);

			//             cluster_energy_summed_adj2_[detector_index][iCluster] += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] + 2];
			//         }

			//     }

			// }
		}
		// Normalizing strip position and calculating Eta, which represents relative position of the hit with respect to the nears readout strip
		calibEvent->data.at(detector_index).cluster_position_[iCluster] = calibEvent->data.at(detector_index).cluster_position_[iCluster] / calibEvent->data.at(detector_index).cluster_energy_summed_[iCluster];
		// cluster_position_adj1_[detector_index][iCluster] = cluster_position_adj1_[detector_index][iCluster] / cluster_energy_summed_adj1_[detector_index][iCluster];
		// cluster_position_adj2_[detector_index][iCluster] = cluster_position_adj2_[detector_index][iCluster] / cluster_energy_summed_adj2_[detector_index][iCluster];

		calibEvent->data.at(detector_index).Eta_[iCluster] = calibEvent->data.at(detector_index).cluster_position_[iCluster] - (Int_t)calibEvent->data.at(detector_index).cluster_position_[iCluster];
		// Eta_adj1_[detector_index][iCluster] = cluster_position_adj1_[detector_index][iCluster] - (Int_t)cluster_position_adj1_[detector_index][iCluster];
		// Eta_adj2_[detector_index][iCluster] = cluster_position_adj2_[detector_index][iCluster] - (Int_t)cluster_position_adj2_[detector_index][iCluster];
		// put sigma computation and output here
		// Calculating sigma
		Double_t sum_squared_diff = 0.0;
		// Double_t sum_squared_diff_adj1 = 0.0;
		// Double_t sum_squared_diff_adj2 = 0.0;
		for (Int_t iStrip = 0; iStrip < calibEvent->data.at(detector_index).cluster_number_strips_[iCluster]; iStrip++)
		{
			Double_t diff = calibEvent->data.at(detector_index).cluster_strip_id_[iCluster][iStrip] - calibEvent->data.at(detector_index).cluster_position_[iCluster];
			sum_squared_diff += calibEvent->data.at(detector_index).cluster_strip_energy_[iCluster][iStrip] * diff * diff;
			// sum_squared_diff_adj1 += cluster_strip_energy_[detector_index][iCluster][iStrip] * diff * diff;
			// sum_squared_diff_adj2 += cluster_strip_energy_[detector_index][iCluster][iStrip] * diff * diff;
			// if (iStrip == 0)
			// {
			// 	if (cluster_strip_id_[detector_index][iCluster][iStrip] > 0 && cluster_strip_id_[detector_index][iCluster][iStrip] < foot_constants::kNumberStrips - 1)
			// 	{
			// 		Double_t diff_adj1 = cluster_strip_id_[detector_index][iCluster][iStrip] - 1 - cluster_position_adj1_[detector_index][iCluster];
			// 		Double_t diff_adj2 = cluster_strip_id_[detector_index][iCluster][iStrip] - 1 - cluster_position_adj2_[detector_index][iCluster];
			// 		sum_squared_diff_adj1 += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] - 1] * diff_adj1 * diff_adj1;
			// 		sum_squared_diff_adj2 += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] - 1] * diff_adj2 * diff_adj2;
			// 		if (cluster_strip_id_[detector_index][iCluster][iStrip] > 1 && cluster_strip_id_[detector_index][iCluster][iStrip] < foot_constants::kNumberStrips - 2)
			// 		{

			// 			diff_adj2 = cluster_strip_id_[detector_index][iCluster][iStrip] - 2 - cluster_position_adj2_[detector_index][iCluster];

			// 			sum_squared_diff_adj2 += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] - 2] * diff_adj2 * diff_adj2;
			// 		}
			// 	}
			// }
			// if (iStrip == (cluster_number_strips_[detector_index][iCluster] - 1))
			// {
			// 	if (cluster_strip_id_[detector_index][iCluster][iStrip] > 0 && cluster_strip_id_[detector_index][iCluster][iStrip] < foot_constants::kNumberStrips - 1)
			// 	{
			// 		Double_t diff_adj1 = cluster_strip_id_[detector_index][iCluster][iStrip] + 1 - cluster_position_adj1_[detector_index][iCluster];
			// 		Double_t diff_adj2 = cluster_strip_id_[detector_index][iCluster][iStrip] + 1 - cluster_position_adj2_[detector_index][iCluster];
			// 		sum_squared_diff_adj1 += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] + 1] * diff_adj1 * diff_adj1;
			// 		sum_squared_diff_adj2 += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] + 1] * diff_adj2 * diff_adj2;
			// 		if (cluster_strip_id_[detector_index][iCluster][iStrip] > 1 && cluster_strip_id_[detector_index][iCluster][iStrip] < foot_constants::kNumberStrips - 2)
			// 		{

			// 			diff_adj2 = cluster_strip_id_[detector_index][iCluster][iStrip] + 2 - cluster_position_adj2_[detector_index][iCluster];

			// 			sum_squared_diff_adj2 += data_baseline_corr_[detector_index][cluster_strip_id_[detector_index][iCluster][iStrip] + 2] * diff_adj2 * diff_adj2;
			// 		}
			// 	}
			// }
		}
		calibEvent->data.at(detector_index).cluster_sigma_[iCluster] = sqrt(sum_squared_diff / calibEvent->data.at(detector_index).cluster_energy_summed_[iCluster]);
		// cluster_sigma_adj1_[detector_index][iCluster] = sqrt(sum_squared_diff_adj1 / cluster_energy_summed_adj1_[detector_index][iCluster]);
		// cluster_sigma_adj2_[detector_index][iCluster] = sqrt(sum_squared_diff_adj2 / cluster_energy_summed_adj2_[detector_index][iCluster]);
	}
}

void TFOOTCalibrProc::CreateHistograms()
{
	// char dir[] = "FOOT/Calibrated";
	TString dir = "FOOT/Calibrated";

	for (int i = 0; i < 8; i++)
	{
		hcalamp[i] = new TH2D(Form("amp_ch_%1d", i + 1),
							  Form("FOOT  AmpUncorrected. vs. ch. layer #%1d", i + 1),
							  FOOT_CHN, 0, FOOT_CHN,
							  FOOT_ADC_BINS + 400 * FOOT_ADC_MAX / FOOT_ADC_BINS, -400., FOOT_ADC_MAX);
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
								  FOOT_ADC_BINS + 400 * FOOT_ADC_MAX / FOOT_ADC_BINS, -400., FOOT_ADC_MAX);
		hcalampCorr[i]->SetMarkerColor(1);
		hcalampCorr[i]->SetXTitle("channel");
		hcalampCorr[i]->SetYTitle("ADC val.");
		TGo4Analysis::Instance()->AddHistogram(hcalampCorr[i], dir);
		//
	}
	dir = "FOOT/multiplicities";
	hmult = new TH2I("hmult",
					 "FOOT mult. by layer",
					 640, 0, FOOT_CHN,
					 8, 0, 8);
	hmult->SetMarkerColor(1);
	hmult->SetXTitle("Multiplicity");
	hmult->SetYTitle("FOOT Layer");
	TGo4Analysis::Instance()->AddHistogram(hmult, dir);

	hmultStrip = new TH2I("MultStrip",
						  "FOOT strip mult. by layer",
						  640, 0, FOOT_CHN,
						  8, 0, 8);
	hmultStrip->SetMarkerColor(1);
	hmultStrip->SetXTitle("Strip multiplicity");
	hmultStrip->SetYTitle("FOOT Layer");
	TGo4Analysis::Instance()->AddHistogram(hmultStrip, dir);

	hmultCluster = new TH2I("MultCluster",
							"FOOT cluster mult. by layer",
							640, 0, FOOT_CHN,
							8, 0, 8);
	hmultCluster->SetMarkerColor(1);
	hmultCluster->SetXTitle("Cluster multiplicity");
	hmultCluster->SetYTitle("FOOT Layer");
	TGo4Analysis::Instance()->AddHistogram(hmultCluster, dir);

	/////////////////////////////////////
	// clusters
	/////////////////////////////////////

	dir = "FOOT/clusters";

	hClusterEnergy = new TH2I("Cluster_E",
							  "FOOT cluster energy",
							  FOOT_ADC_MAX, 0, 4 * FOOT_ADC_MAX,
							  8, 0, 8);
	hClusterEnergy->SetMarkerColor(1);
	hClusterEnergy->SetXTitle("Cluster energy");
	hClusterEnergy->SetYTitle("FOOT Layer");
	TGo4Analysis::Instance()->AddHistogram(hClusterEnergy, dir);

	hSingleClusterPositionX = new TH2I("SingleClusterPositionX",
									   "proper title",
									   640, 0, FOOT_CHN,
									   4, 0, 4);
	hSingleClusterPositionX->SetMarkerColor(1);
	hSingleClusterPositionX->SetXTitle("Cluster position [strip]");
	hSingleClusterPositionX->SetYTitle("FOOT X Layer");

    TGo4Analysis::Instance()->AddHistogram(hSingleClusterPositionX, dir);

	hSingleClusterPositionY = new TH2I("SingleClusterPositionY",
									   "proper title",
									   640, 0, FOOT_CHN,
									   4, 0, 4);
	hSingleClusterPositionY->SetMarkerColor(1);
	hSingleClusterPositionY->SetXTitle("Cluster position [strip]");
	hSingleClusterPositionY->SetYTitle("FOOT Y Layer");
	TGo4Analysis::Instance()->AddHistogram(hSingleClusterPositionY, dir);

	// number of stations
	for (Int_t i = 0; i < 4; i++)
	{
		hClusterProjection[i] = new TH2I(Form("Cluster_projection_%1d", i),
										 Form("Cluster projection in station #%1d", i),
										 640, 0, FOOT_CHN,
										 640, 0, FOOT_CHN);
		hClusterProjection[i]->SetMarkerColor(1);
		hClusterProjection[i]->SetXTitle("X");
		hClusterProjection[i]->SetYTitle("Y");
		TGo4Analysis::Instance()->AddHistogram(hClusterProjection[i], dir);
	}

	for (Int_t i = 0; i < 3; i++)
	{
		hSingleClusterCorrXX[i] = new TH2I(Form("Cluster_corrX0X%1d", i+1),
										   Form("Cluster correlation in XX station #%1d", i+1),
										   640, 0, FOOT_CHN,
										   640, 0, FOOT_CHN);
		hSingleClusterCorrXX[i]->SetMarkerColor(1);
		hSingleClusterCorrXX[i]->SetXTitle("X0");
		hSingleClusterCorrXX[i]->SetYTitle(Form("X%1d", i+1));
		TGo4Analysis::Instance()->AddHistogram(hSingleClusterCorrXX[i], dir);

		hSingleClusterCorrYY[i] = new TH2I(Form("Cluster_corrY0Y%1d", i+1),
										   Form("Cluster correlations in YY station #%1d", i+1),
										   640, 0, FOOT_CHN,
										   640, 0, FOOT_CHN);
		hSingleClusterCorrYY[i]->SetMarkerColor(1);
		hSingleClusterCorrYY[i]->SetXTitle("Y0");
		hSingleClusterCorrYY[i]->SetYTitle(Form("Y%1d", i+1));
		TGo4Analysis::Instance()->AddHistogram(hSingleClusterCorrYY[i], dir);
	}

	///////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////

	for (int i = 0; i < 8; i++)
	{
		hClusterLengthDeposit[i] = new TH2I(Form("ClusterLengthDepCorr_%1d", i),
											Form("Cluster length  vs. cluster dep. #%1d", i),
											FOOT_CHN, 0, FOOT_CHN,
											FOOT_ADC_MAX, 0, 4 * FOOT_ADC_MAX);
		hClusterLengthDeposit[i]->SetMarkerColor(1);
		hClusterLengthDeposit[i]->SetXTitle("Cluster length (a. u.)");
		hClusterLengthDeposit[i]->SetYTitle("Energy deposit (a. u.)");
		TGo4Analysis::Instance()->AddHistogram(hClusterLengthDeposit[i], dir);
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

		hmult->Fill(calibEvent->data.at(i).mult, i);

		hmultStrip->Fill(calibEvent->data.at(i).multStrip, i);

		hmultCluster->Fill(calibEvent->data.at(i).cluster_multiplicity_, i);

		// single cluster histograms
		if (calibEvent->data.at(i).cluster_multiplicity_ == 1)
		{
			hClusterEnergy->Fill(calibEvent->data.at(i).cluster_energy_summed_[0], i);

			hClusterLengthDeposit[i]->Fill(calibEvent->data.at(i).cluster_number_strips_[0], calibEvent->data.at(i).cluster_energy_summed_[0]);
		}
	};

	// loop over all stations
	for (Int_t i = 0; i < 4; i++)
	{
		if (calibEvent->data.at(2 * i).cluster_multiplicity_ == 1)
			hSingleClusterPositionY->Fill(calibEvent->data.at(2 * i).cluster_position_[0], i);
		if (calibEvent->data.at(2 * i + 1).cluster_multiplicity_ == 1)
			hSingleClusterPositionX->Fill(calibEvent->data.at(2 * i + 1).cluster_position_[0], i);

        if (calibEvent->data.at(2 * i).cluster_multiplicity_ == 1 && calibEvent->data.at(2 * i + 1).cluster_multiplicity_ == 1)
			hClusterProjection[i]->Fill(calibEvent->data.at(2 * i + 1).cluster_position_[0], calibEvent->data.at(2 * i).cluster_position_[0]);
	}

	// spatial correlations
	for (Int_t i = 0; i < 3; i++)
	{
		if (calibEvent->data.at(0).cluster_multiplicity_ == 1 && calibEvent->data.at(2 * i + 2).cluster_multiplicity_ == 1)
			hSingleClusterCorrYY[i]->Fill(calibEvent->data.at(0).cluster_position_[0], calibEvent->data.at(2 * i + 2).cluster_position_[0]);

		if (calibEvent->data.at(1).cluster_multiplicity_ == 1 && calibEvent->data.at(2 * i + 3).cluster_multiplicity_ == 1)
			hSingleClusterCorrXX[i]->Fill(calibEvent->data.at(1).cluster_position_[0], calibEvent->data.at(2 * i + 3).cluster_position_[0]);
	}
}
