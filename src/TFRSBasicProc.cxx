#include "TFRSBasicProc.h"

#include "TH1.h"
#include "TH2.h"
#include "TCutG.h"

#include "Go4StatusBase/TGo4Picture.h"
#include "Go4ConditionsBase/TGo4WinCond.h"
#include "Go4ConditionsBase/TGo4PolyCond.h"
#include "Go4ConditionsBase/TGo4CondArray.h"

const  char* TFRSBasicProc::mw_name_ext[13]={"(1:1)", "(2:1)", "(2:2)", "(3:1)",  // "(4:1)", "(4:2)",
					     "(5:1)",  //"(6:1)",
					     "(7:1)", "(8:1)", "(8:2)", "(9:1)", "(10:1)",
                                             "(Z:1)", "(Z:2)", "(Z:3)"};
//const  char* TFRSBasicProc::mw_folder_ext1[13]={"1", "2", "2", "3","4", "4", "5", "6","7", "8","8", "B", "B"};
const  char* TFRSBasicProc::mw_folder_ext1[13]={"1", "2", "2", "3", "5", "7", "8","8", "B", "B", "Z",  "Z", "Z"};
//const  char* TFRSBasicProc::mw_folder_ext2[13]={"11", "21", "22", "31","41", "42", "51", "61","71", "81", "82", "B21", "B22"};
const  char* TFRSBasicProc::mw_folder_ext2[13]={"11", "21", "22", "31", "51", "71", "81", "82", "B21", "B22", "Z1", "Z2", "Z3"};

const  char* TFRSBasicProc::tpc_name_ext1[7]={"TPC21_","TPC22_","TPC23_","TPC24_","TPC41_","TPC42_", "TPC31_"};
const  char* TFRSBasicProc::tpc_folder_ext1[7]={"TPC21","TPC22","TPC23","TPC24","TPC41","TPC42","TPC31"};



TFRSBasicProc::TFRSBasicProc() : TGo4EventProcessor("FRSBasicProc"),
				 fbTestAutoSaveFile(kTRUE), fbUseAutoSaveFile(kFALSE)
{ }

TFRSBasicProc::TFRSBasicProc(const char* name) : TGo4EventProcessor(name),
						 fbTestAutoSaveFile(kTRUE), fbUseAutoSaveFile(kFALSE)
{ }

TFRSBasicProc::~TFRSBasicProc() {
}

TNamed* TFRSBasicProc::TestObject(getfunc func, const char* fname, const char* name) {
   fbObjWasCreated = kTRUE;
    
   if ((func==0) || (fname==0) || (name==0)) return 0;
   
   if (fbTestAutoSaveFile || fbUseAutoSaveFile) {
      char fullname[200];
      if (fname==0) strcpy(fullname, name);
               else sprintf(fullname,"%s/%s",fname, name);
      TNamed* res = (this->*func)(fullname);

      if (fbTestAutoSaveFile) {
        fbUseAutoSaveFile = (res!=0);
        fbTestAutoSaveFile = kFALSE;
      }
      if (res) {
        fbObjWasCreated = kFALSE;
        return res;
      }
   }
   
   return 0;

}

TH1I* TFRSBasicProc::MakeH1I(const char* fname, 
                            const char* hname,
                            Int_t nbinsx, 
                            Float_t xmin, Float_t xmax, 
                            const char* xtitle,
                            Color_t linecolor,
                            Color_t fillcolor,
                            const char* ytitle) {
   TNamed* res = TestObject((getfunc)&TGo4EventProcessor::GetHistogram, fname, hname);
   if (res!=0) return dynamic_cast<TH1I*>(res);
   
   TH1I* histo = new TH1I(hname, hname, nbinsx, xmin, xmax);
   histo->SetXTitle(xtitle);
   if (ytitle) histo->SetYTitle(ytitle);
   histo->SetLineColor(linecolor); 
   histo->SetFillColor(fillcolor);
   AddHistogram(histo, fname);
   return histo;
}

TH2I* TFRSBasicProc::MakeH2I(const char* fname, 
                             const char* hname,
                             Int_t nbinsx, Float_t xmin, Float_t xmax, 
                             Int_t nbinsy, Float_t ymin, Float_t ymax, 
                             const char* xtitle, const char* ytitle,
                             Color_t markercolor) {
   TNamed* res = TestObject((getfunc)&TGo4EventProcessor::GetHistogram, fname, hname);
   if (res!=0) return dynamic_cast<TH2I*>(res);
   
   TH2I* histo = new TH2I(hname, hname, nbinsx, xmin, xmax, nbinsy, ymin, ymax);
   histo->SetMarkerColor(markercolor);
   histo->SetXTitle(xtitle);
   histo->SetYTitle(ytitle);
   AddHistogram(histo, fname);
   return histo;
}

TH1F* TFRSBasicProc::MakeH1F(const char* fname, 
                            const char* hname,
                            Int_t nbinsx, 
                            Float_t xmin, Float_t xmax, 
                            const char* xtitle,
                            Color_t linecolor,
                            Color_t fillcolor,
                            const char* ytitle) {
   TNamed* res = TestObject((getfunc)&TGo4EventProcessor::GetHistogram, fname, hname);
   if (res!=0) return dynamic_cast<TH1F*>(res);
   
   TH1F* histo = new TH1F(hname, hname, nbinsx, xmin, xmax);
   histo->SetXTitle(xtitle);
   if (ytitle) histo->SetYTitle(ytitle);
   histo->SetLineColor(linecolor); 
   histo->SetFillColor(fillcolor);
   AddHistogram(histo, fname);
   return histo;
}


TH1I* TFRSBasicProc::MakeH1I_MW(const char* foldername, const char* name, int nameindex,
                                 Int_t nbinsx, Float_t xmin, Float_t xmax, 
                                 const char* xtitle, Color_t linecolor, Color_t fillcolor)
{
  char fullname[100];
  if(nameindex>=0)
    sprintf(fullname,"%s%s",name, mw_name_ext[nameindex]);
  else
    strcpy(fullname, name);  
   
  return MakeH1I(foldername, fullname, nbinsx, xmin, xmax, xtitle, linecolor, fillcolor);
}

//Make Histograms for TPCs
TH1I* TFRSBasicProc::MakeH1I_TPC(const char* foldername, const char* name, int nameindex,
				  Int_t nbinsx, Float_t xmin, Float_t xmax, 
				  const char* xtitle, Color_t linecolor, Color_t fillcolor)
{
  char fullname[100];
  if(nameindex>=0)
    sprintf(fullname,"%s%s",tpc_name_ext1[nameindex],name);
  else
    strcpy(fullname, name);  
  return MakeH1I(foldername, fullname, nbinsx, xmin, xmax, xtitle, linecolor, fillcolor);
}

TH1I* TFRSBasicProc::MakeH1ISeries(const char* foldername, Int_t seriesnumber, Int_t crate, Int_t number, Bool_t remove)
{
  char fullfoldername[100];                                    
  sprintf(fullfoldername,"%s/%02d", foldername, seriesnumber);
  char histoname[50];
  sprintf(histoname,"newVME%02d_%02d_%02d", crate, seriesnumber, number); 
  if (remove)
    {
      char fullname[200]; 
      sprintf(fullname,"%s/%s",fullfoldername,histoname);
      RemoveHistogram(fullname); 
      return 0;   
    }
  return MakeH1I(fullfoldername, histoname, 4096, 0, 4096);
}

TH1I* TFRSBasicProc::MakeH1ISeriesLabel(const char* foldername, Int_t seriesnumber, const char* labelname, Int_t crate, Int_t number, Bool_t remove)
{
  char fullfoldername[100];                                    
  sprintf(fullfoldername,"%s/%02d_%s", foldername, seriesnumber, labelname);
  char histoname[50];
  sprintf(histoname,"newVME%02d_%02d_%02d", crate, seriesnumber, number); 
  if (remove)
    {
      char fullname[200]; 
      sprintf(fullname,"%s/%s",fullfoldername,histoname);
      RemoveHistogram(fullname); 
      return 0;   
    }
  return MakeH1I(fullfoldername, histoname, 4096, 0, 4096);
}


TH1I* TFRSBasicProc::MakeH1ISeriesV1290raw(const char* foldername, Int_t seriesnumber, Int_t crate, Int_t number, Bool_t remove)
{
  char fullfoldername[100];                                    
  sprintf(fullfoldername,"%s/%02d/first_hit", foldername, seriesnumber);
  char histoname[50];
  sprintf(histoname,"newVME%02d_%02d_%02d", crate, seriesnumber, number); 
  if (remove)
    {
      char fullname[200]; 
      sprintf(fullname,"%s/%s/first_hit",fullfoldername,histoname);
      RemoveHistogram(fullname); 
      return 0;   
    }
  return MakeH1I(fullfoldername, histoname, 4000, 0, 200000);
}

TH1I* TFRSBasicProc::MakeH1ISeriesV1290multip(const char* foldername, Int_t seriesnumber, Int_t crate, Int_t number, Bool_t remove)
{
  char fullfoldername[100];                                    
  sprintf(fullfoldername,"%s/%02d/multihit", foldername, seriesnumber);
  char histoname[50];
  sprintf(histoname,"Multip%02d_%02d_%02d", crate, seriesnumber, number); 
  if (remove)
    {
      char fullname[200]; 
      sprintf(fullname,"%s/%s/multihit",fullfoldername,histoname);
      RemoveHistogram(fullname); 
      return 0;   
    }
  return MakeH1I(fullfoldername, histoname, 4000, 0, 200000);
}

TH1I* TFRSBasicProc::MakeH1ISeriesV1190raw(const char* foldername, Int_t seriesnumber, Int_t crate, Int_t number, Bool_t remove)
{
  char fullfoldername[100];                                    
  sprintf(fullfoldername,"%s/%02d", foldername, seriesnumber);
  char histoname[50];
  sprintf(histoname,"newVME%02d_%02d_%02d", crate, seriesnumber, number); 
  if (remove)
    {
      char fullname[200]; 
      sprintf(fullname,"%s/%s",fullfoldername,histoname);
      RemoveHistogram(fullname); 
      return 0;   
    }
  return MakeH1I(fullfoldername, histoname, 4000, 0, 200000);
}

TH1I* TFRSBasicProc::MakeH1ISeries3(const char* foldername, Int_t seriesnumber, Int_t crate, Int_t number, Bool_t remove) 
{
  char fullfoldername[100];                                    
  sprintf(fullfoldername,"%s/%02d", foldername, seriesnumber);
  char histoname[100];
  sprintf(histoname,"VME%02d_%02d", crate, number); 
  if (remove)
    {
      char fullname[200]; 
      sprintf(fullname,"%s/%s",fullfoldername,histoname);
      RemoveHistogram(fullname); 
      return 0;   
    }
//  return MakeH1I(fullfoldername, histoname, 8192, 0, 500000);
  return MakeH1I(fullfoldername, histoname, 8192, 0, 2097152); // changed SP 30052016 10h17
}




TGo4Picture* TFRSBasicProc::MakeSeriesPicture(const char* foldername, Int_t seriesnumber, Bool_t remove) 
{
  char picname[100];
  sprintf(picname,"Pic_VME0_%02d", seriesnumber);
  
  if (remove)
    {
      char fullname[200]; 
      sprintf(fullname,"%s/%s",foldername,picname);
      RemovePicture(fullname); 
      return 0;   
    }
  
  TGo4Picture* pic = MakePic(foldername, picname, 4, 8);
   
  if (ObjWasCreated())
    for(int i=0;i<4;i++)
      for(int j=0;j<8;j++)
	{
	  char histoname[100];
	  sprintf(histoname,"newVME0_%02d_%02d", seriesnumber, i*8+j);    
	  pic->AddObjName(i, j, histoname);
	}
      
  return pic;
}



TGo4WinCond* TFRSBasicProc::MakeWindowCond(const char* fname,
                                           const char* cname,
                                           float left,
                                           float right,
                                           const char* HistoName) {
   TNamed* res = TestObject((getfunc)&TGo4EventProcessor::GetAnalysisCondition, fname, cname);
   if (res!=0) return dynamic_cast<TGo4WinCond*>(res);
   
   TGo4WinCond* cond = new TGo4WinCond((Text_t*)cname);
   cond->SetValues(left, right);
   cond->Enable();
   if (HistoName!=0)
     cond->SetHistogram(HistoName);
   AddAnalysisCondition(cond, fname);
   return cond;
}

TGo4PolyCond* TFRSBasicProc::MakePolyCond(const char* fname,
                                          const char* cname,
                                          Int_t size,
                                          Float_t (*points)[2],
                                          const char* HistoName) {
   TNamed* res = TestObject((getfunc)&TGo4EventProcessor::GetAnalysisCondition, fname, cname);
   if (res!=0) return dynamic_cast<TGo4PolyCond*>(res);
   
   Float_t fullx[size+1], fully[size+1];
   int numpoints = size;
   
   for (int i=0;i<numpoints;i++) {
     fullx[i] = points[i][0];
     fully[i] = points[i][1];
   }
   
   // connect first and last points
   if ((fullx[0]!=fullx[numpoints-1]) || (fully[0]!=fully[numpoints-1])) {
      fullx[numpoints] = fullx[0];
      fully[numpoints] = fully[0];
      numpoints++;
   }
 
   TCutG mycat("initialcut", numpoints, fullx, fully);
   TGo4PolyCond* cond = new TGo4PolyCond((Text_t*)cname);
   cond->SetValues(&mycat);
   cond->Enable();
   if (HistoName!=0)
     cond->SetHistogram(HistoName);
   AddAnalysisCondition(cond, fname);
   return cond;
}

TGo4CondArray* TFRSBasicProc::MakeCondArray(const char* fname,
                                            const char* cname,
                                            Int_t size,
                                            const char* condclassname) {
   TNamed* res = TestObject((getfunc)&TGo4EventProcessor::GetAnalysisCondition, fname, cname);
   if (res!=0) return dynamic_cast<TGo4CondArray*>(res);
   
   TGo4CondArray* conarr = new TGo4CondArray((Text_t*) cname, size, (Text_t*) condclassname);
   conarr->Enable();
   AddAnalysisCondition(conarr, fname);
   return conarr;
}

void TFRSBasicProc::AssignCond(const char* condname, const char* HistoName) {
   TGo4Condition* cond = GetAnalysisCondition(condname);
   if (cond!=0) 
      cond->SetHistogram(HistoName);
}

TGo4Picture* TFRSBasicProc::MakePic(const char* fname,
                                    const char* pname,
                                    Int_t ndivy, Int_t ndivx, 
                                    TObject* obj0,
                                    TObject* obj1,
                                    TObject* obj2,
                                    TObject* obj3,
                                    TObject* obj4,
                                    TObject* obj5,
                                    TObject* obj6,
                                    TObject* obj7,
                                    TObject* obj8,
                                    TObject* obj9) {
   TNamed* res = TestObject((getfunc)&TGo4EventProcessor::GetPicture, fname, pname);
   if (res!=0) return dynamic_cast<TGo4Picture*>(res);

   TGo4Picture* pic = new TGo4Picture(pname, pname);
   pic->SetDivision(ndivy, ndivx);
   
   for(int posx=0;posx<ndivx;posx++) 
     for(int posy=0;posy<ndivy;posy++) {
        TObject* obj = 0;
        switch(posy*ndivx+posx) {
           case 0: obj = obj0; break;   
           case 1: obj = obj1; break;   
           case 2: obj = obj2; break;   
           case 3: obj = obj3; break;   
           case 4: obj = obj4; break;   
           case 5: obj = obj5; break;   
           case 6: obj = obj6; break;   
           case 7: obj = obj7; break;   
           case 8: obj = obj8; break;   
           case 9: obj = obj9; break;   
           default: obj = 0; 
        } // switch
        pic->AddObject(posy, posx, obj);
     }
     
   pic->SetDrawHeader();  
     
   AddPicture(pic, fname);
   
   return pic;
}

TGo4Picture* TFRSBasicProc::MakePicCond(const char* fname,
                                        const char* pname,
                               Int_t ndivy, Int_t ndivx, 
                               TObject* obj0, TGo4Condition* cond0,
                               TObject* obj1, TGo4Condition* cond1,
                               TObject* obj2, TGo4Condition* cond2,
                               TObject* obj3, TGo4Condition* cond3,
                               TObject* obj4, TGo4Condition* cond4,
                               TObject* obj5, TGo4Condition* cond5,
                               TObject* obj6, TGo4Condition* cond6,
                               TObject* obj7, TGo4Condition* cond7,
                               TObject* obj8, TGo4Condition* cond8) {
   TNamed* res = TestObject((getfunc)&TGo4EventProcessor::GetPicture, fname, pname);
   if (res!=0) return dynamic_cast<TGo4Picture*>(res);
   
   TGo4Picture* pic = new TGo4Picture(pname, pname);
   pic->SetDivision(ndivy, ndivx);
   
   for(int posx=0;posx<ndivx;posx++) 
     for(int posy=0;posy<ndivy;posy++) {
        TObject* obj = 0;
        TGo4Condition* cond = 0;
        switch(posy*ndivx+posx) {
           case 0: obj = obj0; cond = cond0; break;   
           case 1: obj = obj1; cond = cond1; break;   
           case 2: obj = obj2; cond = cond2; break;   
           case 3: obj = obj3; cond = cond3; break;   
           case 4: obj = obj4; cond = cond4; break;   
           case 5: obj = obj5; cond = cond5; break;   
           case 6: obj = obj6; cond = cond6; break;   
           case 7: obj = obj7; cond = cond7; break;   
           case 8: obj = obj8; cond = cond8; break;   
           default: obj = 0; 
        } // switch
        pic->AddObject(posy, posx, obj);
        pic->AddCondition(posy, posx, cond);   
     }

   pic->SetDrawHeader();  
     
//   pic->Print();

   AddPicture(pic, fname);
     
   return pic;
}

ClassImp(TFRSBasicProc)
