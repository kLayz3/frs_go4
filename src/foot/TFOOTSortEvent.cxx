#include "TFOOTSortEvent.h"
#include <algorithm>

TFOOTSortEvent::TFOOTSortEvent()
{
  ;
}

TFOOTSortEvent::~TFOOTSortEvent()
{
  ;
}

void TFOOTSortEvent::SetDetectorSorted(Int_t detectorPosition, UInt_t *p)
{
  std::copy_n(p, 640, FOOTRawCh + 640 * detectorPosition);
}

void TFOOTSortEvent::SetFlippedDetectorSorted(Int_t detectorPosition, UInt_t *p)
{
  std::copy_n(p, 320, FOOTRawCh + 320 + 640 * detectorPosition);
  std::copy_n(p + 320, 320, FOOTRawCh + 640 * detectorPosition);
}

UInt_t *TFOOTSortEvent::GetDetectorSorted(Int_t detectorPosition)
{
  return static_cast<UInt_t *>(FOOTRawCh + 640 * detectorPosition);
}

void TFOOTSortEvent::ClearSortEvent() {
  memset(FOOTRawCh, 0, sizeof(FOOTRawCh));
}

ClassImp(TFOOTSortEvent)
