#include "GIF++EventAction.hh"
#include "GIF++RunAction.hh"
#include "GIF++SteppingAction.hh"
#include "HistoManager.hh"
  
#include "G4RunManager.hh"
#include "G4Event.hh"


////////////////// metodi per accere da altri file ///////////////////////////////////////////////////////////////////////////////////////////////////////
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppEventAction* GIFppEventAction::fgInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppEventAction* GIFppEventAction::Instance()
{
// Static acces function via G4RunManager 

  return fgInstance;
}      
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppEventAction::GIFppEventAction(): 
  //G4UserEventAction(),
  fPrintModulo(100000),

  n_incidenti(0),
  n_incidenti_top(0),
  n_incidenti_bot(0),

  n_cariche(0),
  n_cariche_gap1(0),
  n_cariche_gap2(0)
{
  fgInstance = this;   // metodo per accedere da altri file
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppEventAction::~GIFppEventAction()
{ 
  fgInstance = 0;   // metodo per accedere da altri file
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

///    BEGIN OF EVENT

void GIFppEventAction::BeginOfEventAction(const G4Event* event)
{  

  G4int eventNb = event->GetEventID();
  if (eventNb%fPrintModulo == 0) { 
    G4cout << "\n---> Begin of event: " << eventNb << G4endl;
  }


  // Reset delle variabili
  GIFppSteppingAction::Instance()->Reset();	// resetta valori
//  HistoManager::Instance()-> rawReset();	//resetta valori per ttree raw



}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

///   END OF EVENT  

void GIFppEventAction::EndOfEventAction(const G4Event* /*event*/)
{

   ////    CONTATORE  PARTICELLE INCIDENTI     ////////////

    n_incidenti += GIFppSteppingAction::Instance()->GetIncident();	//CONTATORE particelle incidenti o fake TOP o fake BOT  (somma varibile di stepping presa tramite metodo GetIncident, la somma alla varibile del file event)

n_incidenti_top += GIFppSteppingAction::Instance()->GetIncident_top();	//CONTATORE particelle incidenti  TOP
n_incidenti_bot += GIFppSteppingAction::Instance()->GetIncident_bot();	//CONTATORE particelle incidenti  BOT


   //////////   CONTATORE PARTICELLE CARICHE IN GAS GAP    //////////

    n_cariche += GIFppSteppingAction::Instance()->GetP_charged();	//CONTATORE  p cariche sia in gap1 che gap 2 (somma varibile di stepping presa tramite metodo GetP_charged, la somma alla varibile del file event)

    n_cariche_gap1 += GIFppSteppingAction::Instance()->GetP_charged_gap1();	//CONTATORE p cariche solo in GAP1
    n_cariche_gap2 += GIFppSteppingAction::Instance()->GetP_charged_gap2();	//CONTATORE p cariche solo in GAP2

    // metodo per resettare incident_signal_status , da chiamare a fine ogni event
    GIFppSteppingAction::Instance()-> Reset_incident_signal_status();

//HistoManager::Instance()-> Fill_raw();			// riempier ttree raw anche quando valori sono zero


/*
	/// PRINT EVENT (gia messo a fine run)
   G4cout << "\n" <<"N INCIDENTI" << n_incidenti << "\n";
   G4cout <<"N INCIDENTI TOP" << n_incidenti_top << "\n";
   G4cout <<"N INCIDENTI BOT" << n_incidenti_bot << "\n";
  G4cout << "N CARICHE" << n_cariche << "\n";
   G4cout << "N CARICHE GAP1 " << n_cariche_gap1 << "\n";
   G4cout << "N CARICHE GAP2 " << n_cariche_gap2 << "\n";
*/

}  

//....oooOO0OOooo



