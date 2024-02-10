{

  TFOOTCalibrEvent * ev = dynamic_cast< TFOOTCalibrEvent * >
    (TGo4Analysis::Instance()->GetOutputEvent(2));
  ev->ReadCalib();
}
