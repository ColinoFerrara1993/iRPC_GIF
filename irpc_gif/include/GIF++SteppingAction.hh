#ifndef GIFppSteppingAction_h
#define GIFppSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "GIF++EventAction.hh"
#include "GIF++RunAction.hh"
#include "HistoManager.hh"



/// Stepping action class
/// 


class GIFppSteppingAction : public G4UserSteppingAction
{
  public:
    GIFppSteppingAction();
    virtual ~GIFppSteppingAction();

   // static access method (per accedere a stepping da altri file)
    static GIFppSteppingAction* Instance();

 
    // method from the base class
    virtual void UserSteppingAction(const G4Step*);

   // metodo per resettare incident_signal_status , da chiamare a fine ogni event
    void Reset_incident_signal_status()
    {incident_signal_status=0;}

    // metodo da usare per resettare alcune variabili (chiamato da Event pere azzerare variabili ad ogni evento)
    void Reset();

// funzione per resettare variabili step, da chiamare ad ogi step
void StepReset()
{
z=0 ;				//posizione ad ogni step
x=0 ;
y=0 ;

edep_kodel=0 ;			//energia depositata ad ogni step in kodel
eIon_kodel=0 ;			// solo energia ionizzazione ad ogni step in kodel

compton_kodel=0 ;		// contatore di interazioni compton in kodel
photo_kodel=0 ;			// contatore di interazioni fotoelettrico in kodel
msc_kodel=0;
ion_kodel=0 ;			// contatore di interazioni ionizzzione in kodel
eBrem_kodel=0;

}		


    // get methods METODI PER PRENDERE VALORI DA ALTRI FILE (da event action)

// funzioni per contatori sensitività

    G4int GetIncident() const { return p_incidente; }	//funzione da chiamare in event per prendere stato della varibile particelle incidenti top+bot
    G4int GetIncident_top() const { return p_incidente_top; }	//funzione da chiamare in event per prendere stato della varibile particelle incidenti solo top
    G4int GetIncident_bot() const { return p_incidente_bot; }	//funzione da chiamare in event per prendere stato della varibile particelle incidenti solo bot

    G4int GetP_charged() const { return p_carica; }	//funzione da chiamare in event per prendere stato della varibile particelle cariche in gap1 o gap2
    G4int GetP_charged_gap1() const { return p_carica_gap1; }	//funzione da chiamare in event per prendere stato della varibile particelle cariche solo in gap1 
    G4int GetP_charged_gap2() const { return p_carica_gap2; }	//funzione da chiamare in event per prendere stato della varibile particelle cariche solo in gap2 

// funzione pre prendere energia cinetica delle particelle incidenti che HANNO PRODOTTO SEGNALE
   G4double GetEk_incident_signal() { return Ek_incident_signal; }		// da chiamare a fine evento in eventaction (il quale a sua volta la passa a histo)

/*
//funzioni per parametri analisi

    G4int GetID_TOT() const { return ID_TOT; }		// funzione da chiamare in event per prendere valore ID di particelle incidenti su top e bot
    G4int GetPID_TOT() const { return PID_TOT; }		// funzione da chiamare in event per prendere valore PID di particelle incidenti su top e bot
    G4int GetPDG_TOT() const { return PDG_TOT; }		// funzione da chiamare in event per prendere valore PDG di particelle incidenti su top e bot
    G4int GetEk_TOT() const { return Ek_TOT; }		// funzione da chiamare in event per prendere valore Ek di particelle incidenti su top e bot

    G4int GetID_TOP() const { return ID_TOP; }		// funzione da chiamare in event per prendere valore ID di particelle incidenti su top 
    G4int GetPID_TOP() const { return PID_TOP; }		// funzione da chiamare in event per prendere valore PID di particelle incidenti su top 
    G4int GetPDG_TOP() const { return PDG_TOP; }		// funzione da chiamare in event per prendere valore PDG di particelle incidenti su top 
    G4int GetEk_TOP() const { return Ek_TOP; }		// funzione da chiamare in event per prendere valore Ek di particelle incidenti su top 

    G4int GetID_BOT() const { return ID_BOT; }		// funzione da chiamare in event per prendere valore ID di particelle incidenti su bot
    G4int GetPID_BOT() const { return PID_BOT; }		// funzione da chiamare in event per prendere valore PID di particelle incidenti su  bot
    G4int GetPDG_BOT() const { return PDG_BOT; }		// funzione da chiamare in event per prendere valore PDG di particelle incidenti su  bot
    G4int GetEk_BOT() const { return Ek_BOT; }		// funzione da chiamare in event per prendere valore Ek di particelle incidenti su bot
*/


 private:

// static method (per accere da altri file)
 static GIFppSteppingAction* fgInstance;  


// VARIABILI 

//p incidenti
 G4int p_incidente;			// variabile 0/1  particelle incidenti sia bot che top
 G4int p_incidente_top;			// variabile 0/1 particelle incidenti solo top
 G4int p_incidente_bot;			// variabile 0/1  particelle incidenti solo bot


 G4int ID_TOT;			//ID particelle incidenti sia top che bot
 G4int PID_TOT;			//PID parent ID particelle incidenti sia top che bot
 G4int PDG_TOT;			//PDG particelle incidenti sia top che bot
 G4double Ek_TOT;		//Energia cinetica particelle incidenti si top che bot

 G4int ID_TOP;			//ID particelle incidenti  top 
 G4int PID_TOP;			//PID parent ID particelle incidenti  top
 G4int PDG_TOP;			//PDG particelle incidenti  top 
 G4double Ek_TOP;		//Energia cinetica particelle incidenti top 

 G4int ID_BOT;			//ID particelle incidenti bot
 G4int PID_BOT;			//PID parent ID particelle incidenti bot
 G4int PDG_BOT;			//PDG particelle incidenti bot
 G4double Ek_BOT;		//Energia cinetica particelle incidenti bot

 G4double z_TOT;					//posizione sulla superficie top sulla quale incidono particelle
 G4double x_TOT ;
 G4double y_TOT ;


 G4double z_TOP ;					//posizione sulla superficie top sulla quale incidono particelle
 G4double x_TOP ;
 G4double y_TOP ;

 G4double z_BOT ;					//posizione sulla superficie bot sulla quale incidono particelle
 G4double x_BOT;
 G4double y_BOT ;

//p carice in gap
 G4int p_carica;			// variabile 0/1 particelle cariche sia in gap1 che gap2
 G4int p_carica_gap1;			// variabile 0/1  particelle cariche solo in gap1
 G4int p_carica_gap2;			// variabile 0/1   particelle cariche solo in gap2


 G4int ID_carica_TOT;			//ID particelle incidenti sia GAP1 CHE GAP2
 G4int PID_carica_TOT;			//PID parent ID particelle incidenti sia GAP1 CHE GAP2
 G4int PDG_carica_TOT;			//PDG particelle incidenti sia GAP1 CHE GAP2
 G4double Ek_carica_TOT;		//Energia cinetica particelle incidenti sia GAP1 CHE GAP2

 G4int ID_carica_GAP1;			//ID particelle incidenti GAP1
 G4int PID_carica_GAP1;			//PID parent ID particelle incidenti  GAP1
 G4int PDG_carica_GAP1;			//PDG particelle incidenti GAP1
 G4double Ek_carica_GAP1;		//Energia cinetica particelle incidenti GAP1

 G4int ID_carica_GAP2;			//ID particelle incidenti GAP2
 G4int PID_carica_GAP2;			//PID parent ID particelle incidenti GAP2
 G4int PDG_carica_GAP2;			//PDG particelle incidenti GAP2
 G4double Ek_carica_GAP2;		//Energia cinetica particelle incidenti GAP2


 G4double z_gapTOT ;				//posizione sulla superficie gap1+GAP2 sulla quale particella carica interagisce (edep>0)
 G4double x_gapTOT ;
 G4double y_gapTOT ;

 G4double z_gap1 ;				//posizione sulla superficie gap1 sulla quale particella carica interagisce (edep>0)
 G4double x_gap1 ;
 G4double y_gap1 ;

 G4double z_gap2 ;				//posizione sulla superficie gap2 sulla quale particella carica interagisce (edep>0)
 G4double x_gap2 ;
 G4double y_gap2 ;

 G4double msc_interaction_TOT ;		// contatore numero interazioni con effetto multiple scattering dell'elettrone in GAP1+GAP2 
 G4double eIoni_interaction_TOT ;	// contatore numero interazioni con effetto ionizzazione dell'elettrone in GAP1+GAP2 
 G4double eBrem_interaction_TOT ;	// contatore numero interazioni con effetto Bremstra dell'elettrone in GAP1+GAP2 

 G4double msc_interaction_GAP1 ;		// contatore numero interazioni con effetto multiple scattering dell'elettrone in GAP1 
 G4double eIoni_interaction_GAP1 ;	// contatore numero interazioni con effetto ionizzazione dell'elettrone in GAP1
 G4double eBrem_interaction_GAP1 ;	// contatore numero interazioni con effetto Bremstra dell'elettrone in GAP1 
 
 G4double msc_interaction_GAP2 ;		// contatore numero interazioni con effetto multiple scattering dell'elettrone in GAP2
 G4double eIoni_interaction_GAP2 ;	// contatore numero interazioni con effetto ionizzazione dell'elettrone in GAP2
 G4double eBrem_interaction_GAP2 ;	// contatore numero interazioni con effetto Bremstra dell'elettrone in GAP2
 

// prende informazioni (posizione e processo di creazione) delle particelle che hanno interagito
 G4double z_gapTOT_creation ;			//posizione sulla superficie gap1+GAP2 sulla quale si crea particella carica (getvertexposition)
 G4double x_gapTOT_creation ;
 G4double y_gapTOT_creation ;

 G4double z_gap1_creation ;				//posizione sulla superficie gap1 sulla quale si crea particella carica (getvertexposition)
 G4double x_gap1_creation ;
 G4double y_gap1_creation ;

 G4double z_gap2_creation ;				//posizione sulla superficie gap2 sulla quale si crea particella carica (getvertexposition)
 G4double x_gap2_creation ;
 G4double y_gap2_creation ;

 G4double compton_creator_TOT ;		// contatore numero di volte che il primo elettrone è formato per effetto compton in GAP1+GAP2
 G4double photo_creator_TOT ;			// contatore numero di volte che il primo elettrone è formato per effetto fotoelettrico in GAP1+GAP2

 G4double compton_creator_GAP1 ;		// contatore numero di volte che il primo elettrone è formato per effetto compton in GAP1
 G4double photo_creator_GAP1 ;			// contatore numero di volte che il primo elettrone è formato per effetto fotoelettrico in GAP1

 G4double compton_creator_GAP2 ;		// contatore numero di volte che il primo elettrone è formato per effetto compton in GAP1
 G4double photo_creator_GAP2 ;			// contatore numero di volte che il primo elettrone è formato per effetto fotoelettrico in GAP1



// info ad ogni step

 G4double z ;				//posizione ad ogni step
 G4double x ;
 G4double y ;

 G4double edep_kodel ;			//energia depositata ad ogni step in kodel
 G4double eIon_kodel ;			// solo energia ionizzazione ad ogni step in kodel

 G4double compton_kodel ;		// contatore di interazioni compton in kodel
 G4double photo_kodel ;			// contatore di interazioni fotoelettrico in kodel
 G4double msc_kodel;
 G4double ion_kodel ;			// contatore di interazioni ionizzzione in kodel
 G4double eBrem_kodel;

//energia SOLO dei fotoni incidenti che poi producono segnale
G4double Ek_incident_signal ;	
G4int incident_signal_status;


// info comune a tutti per check su p cariche
G4double edep ;			//energia depositata ad ogni step


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
