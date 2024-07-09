#include <TCanvas.h>
#include <TGraph.h>
#include <TFile.h>
#include <TTree.h>
#include <TApplication.h>

#include <iostream>
#include <TSystem.h>

// #include <conio.h>

// Global variables
TTree *tree;
Long64_t currentEvent = 0;
Long64_t nEntries;
TCanvas *canvas;
TGraph *graphA[6];
TGraph *graphB[6];
Int_t tpat;
Short_t a[6][640];
Short_t b[6][640];

//trigger used for our analysis
Int_t trigger;

// Function to draw a single event
void drawEvent(Long64_t entry)
{

    const Int_t noStrips = 640;

    tree->GetEntry(entry);

    for (size_t i = 0; i < 6; i++)
    {
        /* code */

        if (!graphA[i])
        {
            graphA[i] = new TGraph(noStrips);
            graphB[i] = new TGraph(noStrips);
        }

        for (int j = 0; j < noStrips; ++j)
        {
            // std::cout << a[0][j] << std::endl;
            graphA[i]->SetPoint(j, j, a[i][j]);
            graphB[i]->SetPoint(j, j, b[i][j]);
        }
    }

    if (!canvas)
    {
        canvas = new TCanvas("canvas", "Event Display", 900, 1000);
        canvas->Divide(2, 3);
    }

    for (size_t i = 0; i < 6; i++)
    {
        canvas->cd(i + 1);
        graphA[i]->SetTitle(Form("Event %lld", entry));
        graphA[i]->SetMarkerStyle(20);
        graphA[i]->SetMarkerSize(.2);
        graphA[i]->Draw("AP");

        graphB[i]->SetTitle(Form("Event %lld", entry));
        graphB[i]->SetLineColor(kRed);
        graphB[i]->SetMarkerColor(kRed);
        graphB[i]->SetMarkerStyle(20);
        graphB[i]->SetMarkerSize(.2);
        // graphB[i]->Draw("AP same");
        graphB[i]->Draw("P");
    }

    canvas->Update();
}

// Function to handle key presses
void handleKeyPress()
{
    char key;
    while (true)
    {
        key = getchar(); // Capture key press
        // std::cout << key << std::endl;

        if (key == 'l')
        { // Right arrow key (next event)
            while (currentEvent < nEntries - 1)
            {
                currentEvent++;
                tree->GetEntry(currentEvent);
                if (tpat == trigger)
                {
                    drawEvent(currentEvent);
                    break;
                }
            }
        }
        else if (key == 'j')
        { // Left arrow key (previous event)
            while (currentEvent > 0)
            {
                currentEvent--;
                tree->GetEntry(currentEvent);
                if (tpat == trigger)
                {
                    drawEvent(currentEvent);
                    break;
                }
            }
        }
        else if (key == 'q' || key == 'Q')
        { // Quit
            break;
        }
    }
}

// Main function
void draw_events()
{

    //user parameters: 
    TString convFile;
    convFile = "/home/vratislav/data/S111/conv/conv_0057.root";

    trigger = 0;

    // Open the ROOT file
    TFile *file = TFile::Open(convFile);
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

    // Get the number of entries
    nEntries = tree->GetEntries();

    tree->SetBranchAddress("tpat", &tpat);
    
    tree->SetBranchAddress("A1", a[0]);
    tree->SetBranchAddress("A2", a[1]);
    tree->SetBranchAddress("A3", a[2]);
    tree->SetBranchAddress("A4", a[3]);
    tree->SetBranchAddress("A7", a[4]);
    tree->SetBranchAddress("A8", a[5]);

    tree->SetBranchAddress("B1", b[0]);
    tree->SetBranchAddress("B2", b[1]);
    tree->SetBranchAddress("B3", b[2]);
    tree->SetBranchAddress("B4", b[3]);
    tree->SetBranchAddress("B7", b[4]);
    tree->SetBranchAddress("B8", b[5]);

    // Draw the first event
    drawEvent(currentEvent);

    //  std::cin >> key;   // = gSystem->Getchar();
    handleKeyPress();


    // Close the file
    file->Close();
    delete file;
}