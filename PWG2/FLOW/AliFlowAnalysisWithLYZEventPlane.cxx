/*************************************************************************
* Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  * 
**************************************************************************/

/*
$Log$
*/ 

//#define AliFlowAnalysisWithLYZEventPlane_cxx
 
#include "Riostream.h"  //needed as include

#include "TFile.h"
#include "TList.h"
#include "TComplex.h"   //needed as include
#include "TCanvas.h"   //needed as include
#include "TLegend.h"   //needed as include
#include "TProfile.h"  //needed as include
#include "TVector2.h"

class TH1F;

#include "AliFlowLYZConstants.h"    //needed as include
#include "AliFlowCommonConstants.h" //needed as include
#include "AliFlowEventSimple.h"
#include "AliFlowTrackSimple.h"
#include "AliFlowCommonHist.h"
#include "AliFlowCommonHistResults.h"
#include "AliFlowLYZEventPlane.h"
#include "AliFlowAnalysisWithLYZEventPlane.h"

class AliFlowVector;

// AliFlowAnalysisWithLYZEventPlane:
//
// Class to do flow analysis with the event plane from the LYZ method
//
// author: N. van der Kolk (kolk@nikhef.nl)


ClassImp(AliFlowAnalysisWithLYZEventPlane)

  //-----------------------------------------------------------------------
 
 AliFlowAnalysisWithLYZEventPlane::AliFlowAnalysisWithLYZEventPlane():
   fHistList(NULL),
   fSecondRunList(NULL),
   fSecondReDtheta(NULL),
   fSecondImDtheta(NULL),
   fFirstr0theta(NULL),
   fHistProFlow(NULL),
   fHistProFlow2(NULL),
   fHistProWr(NULL),
   fHistProWrCorr(NULL),
   fHistQsumforChi(NULL),
   fHistDeltaPhi(NULL),
   fHistDeltaPhi2(NULL),
   fHistDeltaPhihere(NULL),
   fHistPhiEP(NULL),
   fHistPhiEPhere(NULL),
   fHistPhiLYZ(NULL),
   fHistPhiLYZ2(NULL),
   fCommonHists(NULL),
   fCommonHistsRes(NULL),
   fEventNumber(0),
   fQsum(NULL),
   fQ2sum(0)
{

  // Constructor.
  fQsum = new TVector2();        // flow vector sum

  fHistList = new TList();
  fSecondRunList = new TList();
}

 

 //-----------------------------------------------------------------------


 AliFlowAnalysisWithLYZEventPlane::~AliFlowAnalysisWithLYZEventPlane() 
 {
   //destructor
   delete fQsum;
   delete fHistList;
   delete fSecondRunList;
 }
 

//-----------------------------------------------------------------------

void AliFlowAnalysisWithLYZEventPlane::WriteHistograms(TString* outputFileName)
{
 //store the final results in output .root file

  TFile *output = new TFile(outputFileName->Data(),"RECREATE");
  output->WriteObject(fHistList, "cobjLYZEP","SingleKey");
  delete output;
}

//-----------------------------------------------------------------------
void AliFlowAnalysisWithLYZEventPlane::Init() {

  //Initialise all histograms
  cout<<"---Analysis with Lee Yang Zeros Event Plane Method---"<<endl;

  //input histograms
  if (fSecondRunList) {
    fSecondReDtheta = (TProfile*)fSecondRunList->FindObject("Second_FlowPro_ReDtheta_LYZ");
    fHistList->Add(fSecondReDtheta);

    fSecondImDtheta = (TProfile*)fSecondRunList->FindObject("Second_FlowPro_ImDtheta_LYZ");
    fHistList->Add(fSecondImDtheta);
    
    fFirstr0theta = (TProfile*)fSecondRunList->FindObject("First_FlowPro_r0theta_LYZ");
    fHistList->Add(fFirstr0theta);

    //warnings
    if (!fSecondReDtheta) {cout<<"fSecondReDtheta is NULL!"<<endl; }
    if (!fSecondImDtheta) {cout<<"fSecondImDtheta is NULL!"<<endl; }
    if (!fFirstr0theta)   {cout<<"fFirstr0theta is NULL!"<<endl; }

  }

  fCommonHists = new AliFlowCommonHist("AliFlowCommonHistLYZEP");
  fHistList->Add(fCommonHists);
  
  fCommonHistsRes = new AliFlowCommonHistResults("AliFlowCommonHistResultsLYZEP"); 
  fHistList->Add(fCommonHistsRes); 
    
  Int_t iNbinsPt = AliFlowCommonConstants::GetNbinsPt();
  Double_t  dPtMin = AliFlowCommonConstants::GetPtMin();	     
  Double_t  dPtMax = AliFlowCommonConstants::GetPtMax();

  fHistProFlow = new TProfile("FlowPro_VPt_LYZEP","FlowPro_VPt_LYZEP",iNbinsPt,dPtMin,dPtMax);
  fHistProFlow->SetXTitle("Pt");
  fHistProFlow->SetYTitle("v2 (%)");
  fHistList->Add(fHistProFlow);
  
  fHistProWr = new TProfile("FlowPro_Wr_LYZEP","FlowPro_Wr_LYZEP",100,0.,0.25);
  fHistProWr->SetXTitle("Q");
  fHistProWr->SetYTitle("Wr");
  fHistList->Add(fHistProWr);

  fHistQsumforChi = new TH1F("Flow_QsumforChi_LYZEP","Flow_QsumforChi_LYZEP",3,-1.,2.);
  fHistQsumforChi->SetXTitle("Qsum.X , Qsum.Y, Q2sum");
  fHistQsumforChi->SetYTitle("value");
  fHistList->Add(fHistQsumforChi);

  fHistDeltaPhi = new TH1F("Flow_DeltaPhi_LYZEP","Flow_DeltaPhi_LYZEP",100,0.,3.14);
  fHistDeltaPhi->SetXTitle("DeltaPhi");
  fHistDeltaPhi->SetYTitle("Counts");
  fHistList->Add(fHistDeltaPhi);

  fHistPhiLYZ = new TH1F("Flow_PhiLYZ_LYZEP","Flow_PhiLYZ_LYZEP",100,0.,3.14);
  fHistPhiLYZ->SetXTitle("Phi from LYZ");
  fHistPhiLYZ->SetYTitle("Counts");
  fHistList->Add(fHistPhiLYZ);

  fHistPhiEP = new TH1F("Flow_PhiEP_LYZEP","Flow_PhiEP_LYZEP",100,0.,3.14);
  fHistPhiEP->SetXTitle("Phi from EP");
  fHistPhiEP->SetYTitle("Counts");
  fHistList->Add(fHistPhiEP);

  fEventNumber = 0;  //set number of events to zero
      
} 
 
//-----------------------------------------------------------------------
 
void AliFlowAnalysisWithLYZEventPlane::Make(AliFlowEventSimple* anEvent, AliFlowLYZEventPlane* aLYZEP) {
  
  //Get the event plane and weight for each event
  if (anEvent) {
         
    //fill control histograms     
    fCommonHists->FillControlHistograms(anEvent);

    //get the Q vector from the FlowEvent
    AliFlowVector vQ = anEvent->GetQ(); 
    if (vQ.X()== 0. && vQ.Y()== 0. ) { cout<<"Q vector is NULL!"<<endl; }
    //Weight with the multiplicity
    Double_t dQX = 0.;
    Double_t dQY = 0.;
    if (vQ.GetMult()!=0.) {
      dQX = vQ.X()/vQ.GetMult();
      dQY = vQ.Y()/vQ.GetMult();
    } else {cerr<<"vQ.GetMult() is zero!"<<endl; }
    vQ.Set(dQX,dQY);
    //cout<<"vQ("<<dQX<<","<<dQY<<")"<<endl;

    //for chi calculation:
    *fQsum += vQ;
    fHistQsumforChi->SetBinContent(1,fQsum->X());
    fHistQsumforChi->SetBinContent(2,fQsum->Y());
    fQ2sum += vQ.Mod2();
    fHistQsumforChi->SetBinContent(3,fQ2sum);
    //cout<<"fQ2sum = "<<fQ2sum<<endl;

    //call AliFlowLYZEventPlane::CalculateRPandW() here!
    aLYZEP->CalculateRPandW(vQ);

    Double_t dWR = aLYZEP->GetWR();     
    Double_t dRP = aLYZEP->GetPsi();

    //fHistProWr->Fill(vQ.Mod(),dWR); //this weight is always positive
    fHistPhiLYZ->Fill(dRP);   
    
    //plot difference between event plane from EP-method and LYZ-method
    Double_t dRPEP = vQ.Phi()/2;                              //gives distribution from (0 to pi)
    //Double_t dRPEP = 0.5*TMath::ATan2(vQ.Y(),vQ.X());       //gives distribution from (-pi/2 to pi/2)
    //cout<<"dRPEP = "<< dRPEP <<endl;
    fHistPhiEP->Fill(dRPEP);

    Double_t dDeltaPhi = dRPEP - dRP;
    if (dDeltaPhi < 0.) { dDeltaPhi += TMath::Pi(); }        //to shift distribution from (-pi/2 to pi/2) to (0 to pi)
    //cout<<"dDeltaPhi = "<<dDeltaPhi<<endl;
    fHistDeltaPhi->Fill(dDeltaPhi); 

    //Flip sign of WR
    Double_t dLow = TMath::Pi()/4.;
    Double_t dHigh = 3.*(TMath::Pi()/4.);
    if ((dDeltaPhi > dLow) && (dDeltaPhi < dHigh)){
      dRP -= (TMath::Pi()/2);
      dWR = -dWR;
      cerr<<"*** dRP modified ***"<<endl;
    }
    fHistProWr->Fill(vQ.Mod(),dWR); //corrected weight
       
    //calculate flow
    //loop over the tracks of the event
    Int_t iNumberOfTracks = anEvent->NumberOfTracks(); 
    for (Int_t i=0;i<iNumberOfTracks;i++) 
      {
	AliFlowTrackSimple* pTrack = anEvent->GetTrack(i) ; 
	if (pTrack){
	  if (pTrack->UseForDifferentialFlow()) {
	    Double_t dPhi = pTrack->Phi();
	    //if (dPhi<0.) fPhi+=2*TMath::Pi();
	    //calculate flow v2:
	    Double_t dv2 = dWR * TMath::Cos(2*(dPhi-dRP));
	    Double_t dPt = pTrack->Pt();
	    //fill histogram
	    fHistProFlow->Fill(dPt,100*dv2);  
	  }  
	}//track selected
      }//loop over tracks
	  
    fEventNumber++;
    cout<<"@@@@@ "<<fEventNumber<<" events processed"<<endl;
  }
}

  //--------------------------------------------------------------------    
void AliFlowAnalysisWithLYZEventPlane::Finish() {
   
  //plot histograms etc. 
  cout<<"AliFlowAnalysisWithLYZEventPlane::Finish()"<<endl;
  
  //constants:
  Double_t  dJ01 = 2.405; 
  Int_t iNtheta = AliFlowLYZConstants::kTheta;
  Int_t iNbinsPt = AliFlowCommonConstants::GetNbinsPt();
  //set the event number
  SetEventNumber((int)fCommonHists->GetHistMultOrig()->GetEntries());
  //cout<<"number of events processed is "<<fEventNumber<<endl;
  
  //set the sum of Q vectors
  fQsum->Set(fHistQsumforChi->GetBinContent(1),fHistQsumforChi->GetBinContent(2));
  SetQ2sum(fHistQsumforChi->GetBinContent(3));  

  //calculate dV the mean of dVtheta
  Double_t  dVtheta = 0; 
  Double_t  dV = 0; 
  for (Int_t theta=0;theta<iNtheta;theta++)	{
    Double_t dR0 = fFirstr0theta->GetBinContent(theta+1); 
    if (dR0!=0.) { dVtheta = dJ01/dR0 ;}
    dV += dVtheta;
  }
  dV /= iNtheta;

  //calculate dChi 
  Double_t  dSigma2 = 0;
  Double_t  dChi= 0;
  if (fEventNumber!=0) {
    *fQsum /= fEventNumber;
    //cerr<<"fQsum->X() = "<<fQsum->X()<<endl;
    //cerr<<"fQsum->Y() = "<<fQsum->Y()<<endl;
    fQ2sum /= fEventNumber;
    //cerr<<"fEventNumber = "<<fEventNumber<<endl;
    //cerr<<"fQ2sum = "<<fQ2sum<<endl;
    dSigma2 = fQ2sum - TMath::Power(fQsum->X(),2.) - TMath::Power(fQsum->Y(),2.) - TMath::Power(dV,2.);  //BP eq. 62
    //cerr<<"dSigma2"<<dSigma2<<endl;
    if (dSigma2>0) dChi = dV/TMath::Sqrt(dSigma2);
    else dChi = -1.;
    fCommonHistsRes->FillChi(dChi);
    cerr<<"dV = "<<dV<<" and chi = "<<dChi<<endl;
  }
  
  for(Int_t b=0;b<iNbinsPt;b++){
    Double_t dv2pro = 0.;
    Double_t dErr2difcomb = 0.;   
    Double_t dErrdifcomb = 0.;
    if(fHistProFlow) {
      dv2pro = fHistProFlow->GetBinContent(b);
      //calculate error
      for (Int_t theta=0;theta<iNtheta;theta++) {
	Double_t dTheta = ((double)theta/iNtheta)*TMath::Pi(); 
	Int_t iNprime = TMath::Nint(fHistProFlow->GetBinEntries(b));
	//cerr<<"iNprime = "<<iNprime<<endl;
	if (iNprime!=0) { 
	  Double_t dApluscomb = TMath::Exp((dJ01*dJ01)/(2*dChi*dChi)*
					   TMath::Cos(dTheta));
	  Double_t dAmincomb = TMath::Exp(-(dJ01*dJ01)/(2*dChi*dChi)*
					  TMath::Cos(dTheta));
	  dErr2difcomb += (TMath::Cos(dTheta)/(4*iNprime*TMath::BesselJ1(dJ01)*
						 TMath::BesselJ1(dJ01)))*
	    ((dApluscomb*TMath::BesselJ0(2*dJ01*TMath::Sin(dTheta/2))) - 
	     (dAmincomb*TMath::BesselJ0(2*dJ01*TMath::Cos(dTheta/2))));
	} //if !=0
	//else { cout<<"iNprime = 0"<<endl; }
      } //loop over theta
      
      if (dErr2difcomb!=0.) {
	dErr2difcomb /= iNtheta;
	dErrdifcomb = TMath::Sqrt(dErr2difcomb)*100;
	//cerr<<"dErrdifcomb = "<<dErrdifcomb<<endl;
      }
      else {dErrdifcomb = 0.; }

      //fill TH1D
      fCommonHistsRes->FillDifferentialFlow(b, dv2pro, dErrdifcomb); 
    
    } //if fHistProFLow
    else  {
      cout << "Profile Hist missing" << endl;
      break;
    }
    
  } //loop over b

  cout<<".....finished"<<endl;
 }

