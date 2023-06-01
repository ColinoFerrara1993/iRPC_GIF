#include "GIF++RunAction.hh"
#include "GIF++EventAction.hh"
#include "GIF++SteppingAction.hh"
#include "GIF++DetectorConstruction.hh"
#include "GIF++PrimaryGeneratorAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "HistoManager.hh"


//root staff
//#include <TROOT.h>
#include "g4root.hh"
//#include <TFile.h>
//#include <TH1D.h>
//#include "TH3D.h"
//#include "TH2D.h"
//#include "TGraph.h"
//#include "TBranch.h"
//#include "TTree.h"
//#include <TNtuple.h>



////////////////// metodi per accere da altri file ///////////////////////////////////////////////////////////////////////////////////////////////////////
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppRunAction* GIFppRunAction::fgInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppRunAction* GIFppRunAction::Instance()
{
// Static acces function via G4RunManager 

  return fgInstance;
}      
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




GIFppRunAction::GIFppRunAction():
// variabili da printare  afine run (prese da event)
n_incidenti_final(0),			
n_incidenti_top_final(0),		
n_incidenti_bot_final(0),		

n_cariche_final(0),		
n_cariche_gap1_final(0),	
n_cariche_gap2_final(0)		
 
{
  fgInstance = this;   // metodo per accedere da altri file
}

GIFppRunAction::~GIFppRunAction()
{
  fgInstance = 0;   // metodo per accedere da altri file
}


//INIZIO RUN
void GIFppRunAction::BeginOfRunAction(const G4Run* aRun)
{
   G4cout << "### Run " << aRun->GetRunID() << " START ### \n";			
   fTime = time(NULL);			//offset del timer


 HistoManager::Instance()->book();

}


//FINE RUN
void GIFppRunAction::EndOfRunAction(const G4Run* aRun)
{
  


 G4int nbEvents = aRun->GetNumberOfEvent();			//prende numero eventi
  if (nbEvents == 0) return;

  fTime = time(NULL) - fTime;				//conta tempo impiegato per eseguire run completo

  G4cout << "\n--------------------End of Run: "<< aRun->GetRunID() << "  tooks: " << fTime << " seconds     ------------------------------ \n";
  G4cout << "\n ======================== run summary ======================\n";  
  G4cout << "\n The run was " << nbEvents <<" events \n";
  G4cout << "-----------------------------------------------\n";


 // Run conditions: modello con sorgente fgun
  /*
  const G4ParticleGun* particleGun  = GE1PrimaryGeneratorAction::Instance()->GetParticleGun();
  G4String primary_particleName = particleGun->GetParticleDefinition()->GetParticleName();                       
  G4double primary_particleEnergy = particleGun->GetParticleEnergy();

  G4cout << "\n--------------------End of Run: "<< aRun->GetRunID() << "  tooks: " << fTime << " seconds     ------------------------------ \n";
  G4cout << "\n ======================== run summary ======================\n";  
  G4cout << "\n The run was " << nbEvents <<" events of " << primary_particleName << " having energy: " << G4BestUnit(primary_particleEnergy,"Energy")  <<"\n";
  G4cout << "\n-----------------------------------------------\n";

*/
HistoManager::Instance()->Get_EventNumber_histo(nbEvents);		//passa numero eventi richiesti per plot histo
HistoManager::Instance()->Fill_final_histograms();		// crea file finali composti
HistoManager::Instance()->save();				// salva tutto nel root file


// stampa il numero finale prendendo valori da event

n_incidenti_final= GIFppEventAction::Instance()->GetIncident_final();		
n_incidenti_top_final= GIFppEventAction::Instance()-> GetIncident_top_final();
n_incidenti_bot_final= GIFppEventAction::Instance()-> GetIncident_bot_final();

n_cariche_final= GIFppEventAction::Instance()-> GetP_charged_final();		
n_cariche_gap1_final= GIFppEventAction::Instance()-> GetP_charged_gap1_final();		
n_cariche_gap2_final= GIFppEventAction::Instance()-> GetP_charged_gap2_final();

G4double sensitivity_from_histo= HistoManager::Instance()-> GetSensitivity();


G4cout << "\n" <<"N INCIDENTI     " << n_incidenti_final << "\n";
G4cout <<"N INCIDENTI TOP " << n_incidenti_top_final << "\n";
G4cout <<"N INCIDENTI BOT " << n_incidenti_bot_final << "\n";
G4cout <<"N CARICHE       " << n_cariche_final << "\n";
G4cout <<"N CARICHE GAP1  " << n_cariche_gap1_final << "\n";
G4cout <<"N CARICHE GAP2  " << n_cariche_gap2_final << "\n";
G4cout <<"Sensitivity     " << sensitivity_from_histo << "\n";

}

