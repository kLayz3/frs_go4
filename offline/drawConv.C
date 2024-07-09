#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TH1I.h"

#include <iostream>
#include <vector>

TTree *tree;

// Function to create a TChain from a list of TFiles
TChain *createTChain(const std::vector<TString> &fileNames, const std::string &treeName)
{
    TChain *chain = new TChain(treeName.c_str());

    for (const auto &fileName : fileNames)
    {

        TFile *file = TFile::Open(fileName);
        if (!file || file->IsZombie())
        {
            std::cerr << "Error: Cannot open the file " << fileName << std::endl;
            continue;
        }

        if (file->GetListOfKeys()->Contains(treeName.c_str()))
        {
            chain->Add(fileName);
        }
        else
        {
            std::cerr << "Error: Tree " << treeName << " not found in file " << fileName << std::endl;
        }

        file->Close();
    }

    if (chain->GetNtrees() == 0)
    {
        std::cerr << "Error: No valid trees were added to the chain." << std::endl;
        delete chain;
        return nullptr;
    }

    return chain;
}

void OpenTree(const std::string fileName)
{
    // Open the ROOT file
    TFile *file = TFile::Open(fileName.c_str(), "READ");
    if (!file || file->IsZombie())
    {
        std::cerr << "Error: Cannot open the file!" << std::endl;
        return;
    }

    // Get the TTree
    tree = (TTree *)file->Get("trFOOT");
    if (!tree)
    {
        std::cerr << "Error: Cannot find the TTree!" << std::endl;
        return;
    }
}

void drawConv()
{

    // user parameters:
    TString filesLocation = "/home/vratislav/data/S111/conv/";
    TString convFile;
    convFile = filesLocation + "conv_0057.root";

    std::vector<TString> fileNames = {
        filesLocation + "conv_0058.root",
        filesLocation + "conv_0057.root",
        filesLocation + "conv_0072.root",
        filesLocation + "conv_0088.root",
        filesLocation + "conv_0106.root",
        filesLocation + "conv_0120.root",
        filesLocation + "conv_0129.root",
        filesLocation + "conv_0164.root",
        filesLocation + "conv_0167.root",
        filesLocation + "conv_0171.root",
        filesLocation + "conv_0199.root",
        filesLocation + "conv_0200.root",
        filesLocation + "conv_0201.root",
        filesLocation + "conv_0202.root",
        filesLocation + "conv_0203.root",
        filesLocation + "conv_0204.root",
        filesLocation + "conv_0205.root",
        filesLocation + "conv_0206.root",
        filesLocation + "conv_0207.root",
        filesLocation + "conv_0208.root"};

    TChain *chain = createTChain(fileNames, "trFOOT");

    if (chain)
    {
        std::cout << "TChain successfully created with " << chain->GetNtrees() << " files." << std::endl;
    }

    TCanvas *c1 = new TCanvas();

    TH1I *hA = new TH1I("histA", "title", 700, -500, 200);
    TH1I *hB = new TH1I("histB", "title", 700, -500, 200);
    hA->SetLineWidth(2);
    hB->SetLineWidth(2);

    tree->Draw("A1>>histA", "A1<200", "goff");
    tree->Draw("B1>>histB", "B1<200", "goff");
    hB->SetLineColor(kRed);

    hB->Draw();
    hA->Draw("same");
    // std::cout << hB->GetMean() << std::endl;

    TCanvas *c2 = new TCanvas();
    chain->Draw("tpat", "tpat<100");
}