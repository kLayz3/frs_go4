#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <array>
#include "TFile.h"
#include "TTree.h"
#include "TFRSSortEvent.h"
#include "TGo4CommandReceiver.h"
#include "foot/foot_common.hh"

// program for fine calibration
// here we consider that in bad strip pattern 0x1bad is written
const size_t sample_size = (1 << 13) + 1;
const size_t median_pos = (1 << 12);

std::array<std::array<short, 640>, 8> offset;

void finecor(short *start)
{
	std::array<short, 64> chip_vals;
	std::copy(start, start + 64, chip_vals.begin());

	std::nth_element(chip_vals.begin(),
					 chip_vals.begin() + 31,
					 chip_vals.end());

	short medval = (chip_vals[31]);
	for (int j = 0; j < 64; j++)
		start[j] -= medval;
	;
}

int main(int argc, char *argv[])
{

	std::array<std::array<short, sample_size>, 64> sample;
	TFRSSortEvent *ev = nullptr;
	if (argc < 3)
	{
		std::cerr << "Input and output files are required\n";
		return EXIT_FAILURE;
	}
	std::cout << "Input file  " << argv[1] << std::endl;
	std::cout << "Output file " << argv[2] << std::endl;
	TFile f(argv[1]);
	f.ls();
	TTree *t = f.Get<TTree>("SortxTree");
	if (t == nullptr)
	{
		std::cerr << "There is no tree"
				  << "SortxTree"
				  << " in file" << argv[1] << std::endl;
		;
	}
	if (sample_size > size_t(t->GetEntries()))
	{
		std::cerr << "ROOT file to short\n";
		return EXIT_FAILURE;
		f.Close();
	}

	t->SetBranchAddress("FRSSortEvent.", &ev);
	// Now we make sample for offset calculations
	for (int board_num = 0; board_num < 8; board_num++)
	{
		for (int chip_num = 0; chip_num < 10; chip_num++)
		{
			for (size_t i = 0; i < sample_size; i++)
			{
				t->GetEntry(i);
				for (int j = 0; j < 64; j++)
					sample[j][i] = short(ev->FOOTRawCh[j + 64 * chip_num + 640 * board_num]);
			}

			for (int i = 0; i < 64; i++)
			{
				std::nth_element(sample[i].begin(),
								 sample[i].begin() + median_pos,
								 sample[i].end());
				offset[board_num][i + 64 * chip_num] = sample[i].at(median_pos);
				// std::cout<<sample[i].at(median_pos)<<std::endl;
			}
		}
	}

	TFile fo(argv[2], "RECREATE", "FOOT calibrated data");
	TTree trFOOT("trFOOT", "FOOT data");
	Int_t tpat;
	// data before baseline correction:
	Short_t A1[640], A2[640], A3[640], A4[640], A5[640], A6[640], A7[640], A8[640];
	// data after baseline correction:
	Short_t B1[640], B2[640], B3[640], B4[640], B5[640], B6[640], B7[640], B8[640];
	trFOOT.Branch("tpat", &tpat, "tpat/I");
	trFOOT.Branch("A1", A1, "A1[640]/S");
	trFOOT.Branch("A2", A2, "A2[640]/S");
	trFOOT.Branch("A3", A3, "A3[640]/S");
	trFOOT.Branch("A4", A4, "A4[640]/S");
	trFOOT.Branch("A5", A5, "A5[640]/S");
	trFOOT.Branch("A6", A6, "A6[640]/S");
	trFOOT.Branch("A7", A7, "A7[640]/S");
	trFOOT.Branch("A8", A8, "A8[640]/S");
	trFOOT.Branch("B1", B1, "B1[640]/S");
	trFOOT.Branch("B2", B2, "B2[640]/S");
	trFOOT.Branch("B3", B3, "B3[640]/S");
	trFOOT.Branch("B4", B4, "B4[640]/S");
	trFOOT.Branch("B5", B5, "B5[640]/S");
	trFOOT.Branch("B6", B6, "B6[640]/S");
	trFOOT.Branch("B7", B7, "B7[640]/S");
	trFOOT.Branch("B8", B8, "B8[640]/S");
	for (int i = 0; i < t->GetEntries(); i++)
	{
		t->GetEntry(i);
		tpat = ev->pattern;
		for (int j = 0; j < 640; j++)
		{
			B1[j] = short(ev->FOOTRawCh[j + 640 * 0]) - offset[0][j];
			B2[j] = short(ev->FOOTRawCh[j + 640 * 1]) - offset[1][j];
			B3[j] = short(ev->FOOTRawCh[j + 640 * 2]) - offset[2][j];
			B4[j] = short(ev->FOOTRawCh[j + 640 * 3]) - offset[3][j];
			B5[j] = short(ev->FOOTRawCh[j + 640 * 4]) - offset[4][j];
			B6[j] = short(ev->FOOTRawCh[j + 640 * 5]) - offset[5][j];
			B7[j] = short(ev->FOOTRawCh[j + 640 * 6]) - offset[6][j];
			B8[j] = short(ev->FOOTRawCh[j + 640 * 7]) - offset[7][j];
		}
		std::copy(B1, B1 + 640, A1);
		std::copy(B2, B2 + 640, A2);
		std::copy(B3, B3 + 640, A3);
		std::copy(B4, B4 + 640, A4);
		std::copy(B5, B5 + 640, A5);
		std::copy(B6, B6 + 640, A6);
		std::copy(B7, B7 + 640, A7);
		std::copy(B8, B8 + 640, A8);
		for (int cn = 0; cn < 10; cn++)
		{
			finecor(B1 + 64 * cn);
			finecor(B2 + 64 * cn);
			finecor(B3 + 64 * cn);
			finecor(B4 + 64 * cn);
			finecor(B5 + 64 * cn);
			finecor(B6 + 64 * cn);
			finecor(B7 + 64 * cn);
			finecor(B8 + 64 * cn);
		}
		trFOOT.Fill();
	}
	trFOOT.Write();
	fo.Close();
	f.cd();
	f.Close();
	return EXIT_SUCCESS; // optional return value
}
