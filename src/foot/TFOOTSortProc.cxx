#include "TFOOTSortProc.h"
#include "foot_unpack_event.hh"
#include <algorithm>
#include "TFOOTSortEvent.h"
#include "TH2I.h"
#include "TGo4AnalysisImp.h"

TFOOTSortProc::TFOOTSortProc()
{
  par = dynamic_cast<TFOOTParameter *>(TGo4Analysis::Instance()->GetParameter("FOOTPar"));
  char dir[] = "FOOT/Sort";

  // create histograms with full information
  for (int i = 0; i < 8; i++)
  {
    hRawZeros[i] = new TH2I(Form("rawAmpFull_ch_%1d", i + 1),
                            Form("FOOT Raw AmpUncorrected. vs. stripN FOOT#%1d", i + 1),
                            FOOT_CHN, 0, FOOT_CHN,
                            FOOT_ADC_BINS + 200*FOOT_ADC_MAX/FOOT_ADC_BINS, -200., FOOT_ADC_MAX);

    hRawZeros[i]->SetMarkerColor(1);
    hRawZeros[i]->SetXTitle("strip");
    hRawZeros[i]->SetYTitle("ADC val.");
    TGo4Analysis::Instance()->AddHistogram(hRawZeros[i], dir);
  }

  // create histograms with suppressed zeros
  for (int i = 0; i < 8; i++)
  {
    hRawZerosSuppressed[i] = new TH2I(Form("rawAmp_ch_%1d", i + 1),
                                      Form("FOOT Raw AmpUncorrected. (sup. 0) vs. stripN FOOT#%1d", i + 1),
                                      FOOT_CHN, 0, FOOT_CHN,
                                      FOOT_ADC_BINS + 200*FOOT_ADC_MAX/FOOT_ADC_BINS, -200., FOOT_ADC_MAX);

    hRawZerosSuppressed[i]->SetMarkerColor(1);
    hRawZerosSuppressed[i]->SetXTitle("strip");
    hRawZerosSuppressed[i]->SetYTitle("ADC val.");
    TGo4Analysis::Instance()->AddHistogram(hRawZerosSuppressed[i], dir);
  }
}

TFOOTSortProc::~TFOOTSortProc()
{
  ;
}
void TFOOTSortProc::FillEvent(TFOOTSortEvent *outEvent,
                              TFootPtr *inEvent)
{

  //TODO: very probably it should be called from event itself as in the case of TFOOTCalibrEvent::Clear
  outEvent->ClearSortEvent();

  for (int i = 0; i < 8; i++)
  {
    const int detectorID = par->order[i];
    const bool flip = par->flip[i];
    if (flip)
      outEvent->SetFlippedDetectorSorted(i, inEvent[detectorID].foot_e);
    else
      outEvent->SetDetectorSorted(i, inEvent[detectorID].foot_e);
  }

  FillHist(outEvent);
}

void TFOOTSortProc::FillHist(TFOOTSortEvent *outEvent)
{
  UInt_t *val;
  for (int i = 0; i < 8; i++)
  {
    val = outEvent->GetDetectorSorted(i);
    for (int j = 0; j < FOOT_CHN; j++)
    {
      hRawZeros[i]->Fill(j, val[j]);

      if (val[j] > 0)
        hRawZerosSuppressed[i]->Fill(j, val[j]);
    }
  }
}
ClassImp(TFOOTSortProc)
