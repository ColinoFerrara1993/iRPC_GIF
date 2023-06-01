#ifndef GIFppEventAction_h
#define GIFppEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "GIF++SteppingAction.hh"
#include "GIF++RunAction.hh"
#include "HistoManager.hh"

class GIFppSteppingAction;


/// Event action class
///

class GIFppEventAction : public G4UserEventAction
{
  public:
    GIFppEventAction();
    virtual ~GIFppEventAction();

    // static access method  (per accere da altri file)
    static GIFppEventAction* Instance();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);


// funzioni per prendere valori da event 

    G4int GetIncident_final() const { return n_incidenti; }	
    G4int GetIncident_top_final() const { return n_incidenti_top; }	
    G4int GetIncident_bot_final() const { return n_incidenti_bot; }	

    G4int GetP_charged_final() const { return n_cariche; }	
    G4int GetP_charged_gap1_final() const { return n_cariche_gap1; }
    G4int GetP_charged_gap2_final() const { return n_cariche_gap2; }

     
  private:

    static GIFppEventAction* fgInstance;  // per accere da altri file

    G4int     fPrintModulo;

    G4int     n_incidenti;			// contatore particelle incidenti bot + top
    G4int     n_incidenti_top;			// contatore particelle incidenti top
    G4int     n_incidenti_bot;			// contatore particelle incidenti bot

    G4int     n_cariche;			// contatore particelle cariche in gap 1 + gap2
    G4int  n_cariche_gap1;			// contatore particelle cariche in gap1
    G4int   n_cariche_gap2;			// contatore particelle cariche in gap2

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
