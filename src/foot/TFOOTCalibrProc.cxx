#include "TFOOTCalibrProc.h"
#include <algorithm>
#include "TFOOTSortEvent.h"
#include "TFOOTCalibrEvent.h"
#include "TFRSCalibrEvent.h"
#include "TFRSSortEvent.h"
#include "TH2D.h"
#include <TGo4AnalysisImp.h>

TFOOTCalibrProc::TFOOTCalibrProc()
{
  par = dynamic_cast<TFOOTParameter *>(TGo4Analysis::Instance()->GetParameter("FOOTPar"));
  char dir[] = "FOOT/";
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
  
  // FOOT correlations with TPCs
  for(int i=0;i<8;i++)
  {
   char fname[100];
   char fn[100];
   sprintf(fname,"TPC22_X_FOOT_%d", i+1);
   sprintf(fn,"Position distribution in X for TPC22 and FOOT_%d", i+1);
   hFOOT_tpcX[i] = new TH2D(fname,fn,300,-120,120,640,0,640);
   hFOOT_tpcX[i]->SetXTitle("TPC22_X [mm]");
   hFOOT_tpcX[i]->SetYTitle("Position (a. u.)");
   TGo4Analysis::Instance()->AddHistogram(hFOOT_tpcX[i], dir);
   
   sprintf(fname,"TPC22_Y_FOOT_%d", i+1);
   sprintf(fn,"Position distribution in Y for TPC22 and FOOT_%d", i+1);
   hFOOT_tpcY[i] = new TH2D(fname,fn,300,-120,120,640,0,640);
   hFOOT_tpcY[i]->SetXTitle("TPC22_Y [mm]");
   hFOOT_tpcY[i]->SetYTitle("Position (a. u.)");
   TGo4Analysis::Instance()->AddHistogram(hFOOT_tpcY[i], dir);
   
   sprintf(fname,"SCI21_E_FOOT_%d", i+1);
   sprintf(fn,"Energy correlation SCI21 and FOOT_%d", i+1);
   hFOOT_SCI21[i] = new TH2D(fname, fn, 600, 0, 3000, FOOT_ADC_BINS, 0, FOOT_ADC_MAX);
   hFOOT_SCI21[i]->SetXTitle("SCI21_E (a.u.)");
   hFOOT_SCI21[i]->SetYTitle("Energy deposit (a. u.)");
   TGo4Analysis::Instance()->AddHistogram(hFOOT_SCI21[i], dir);
  }
}

TFOOTCalibrProc::~TFOOTCalibrProc()
{
  ;
}

void TFOOTCalibrProc::FillEvent(TFOOTCalibrEvent *oev, TFOOTSortEvent *iev)
{
  for (int i = 0; i < 8; i++)
  {
    oev->data.at(i).Set(iev->Get1(i));
  }
  FillHist(oev);
}

void TFOOTCalibrProc::FillFootTpcEvent(TFOOTCalibrEvent *oev, TFRSCalibrEvent* ifrsCal, TFRSSortEvent* ifrsSort)
{
  for(int i=0;i<8;i++)
  {
   for (int j = 0; j < FOOT_CHN; j++)
   {
     hFOOT_tpcX[i]->Fill(ifrsCal->tpc_x[1],oev->data.at(i).clpos[j]);
     hFOOT_tpcY[i]->Fill(ifrsCal->tpc_y[1],oev->data.at(i).clpos[j]);
   }
   
   if (oev->data.at(i).clmult > 0)
    {
      for (UInt_t j = 0; j < oev->data.at(i).clmult; j++)
      {
        hposE[i]->Fill(0.5*(ifrsSort->de_21l+ifrsSort->de_21r),
                       oev->data.at(i).clE[j]);
      }
    }

  }
}

void TFOOTCalibrProc::FillHist(TFOOTCalibrEvent *oev)
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < FOOT_CHN; j++)
    {
      hcalamp[i]->Fill(j, oev->data.at(i).Amp[j]);
    }

    hmult->Fill(oev->data.at(i).mult, i);
    hclmult->Fill(oev->data.at(i).clmult, i);
    if (oev->data.at(i).clmult > 0)
    {
      for (UInt_t j = 0; j < oev->data.at(i).clmult; j++)
      {
        hpos->Fill(oev->data.at(i).clpos[j], i);
        hposE[i]->Fill(oev->data.at(i).clpos[j],
                       oev->data.at(i).clE[j]);
        hclsize[i]->Fill(oev->data.at(i).cllast[j] -
                             oev->data.at(i).clfirst[j] + 1,
                         oev->data.at(i).clE[j]);
      }
    }
  };
}
