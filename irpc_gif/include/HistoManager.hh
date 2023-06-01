#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"
#include "GIF++EventAction.hh"
#include "GIF++RunAction.hh"
#include "GIF++SteppingAction.hh"
#include "G4DataVector.hh"
#include <vector>
#include <map>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//root class

class TFile;
class TH1I;
class TH1D;
class TH2D;
class TH3D;
class TTree;
class TNtuple;
class TGraph;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HistoManager
{
  public:
  
    HistoManager();
   ~HistoManager();

  
static HistoManager* Instance();   // static access method (per accedere a stepping da altri file)

   
void book();			//metodo per creare root file e istogrammi a inizio run

// metodi per riempire gli istogrammi in stepping action
void Fill_incident_TOT(G4double p_incident_TOT, G4double PDG_TOT,G4double ID_TOT, G4double PID_TOT, G4double Ek_TOT, G4double x_TOT,G4double y_TOT,G4double z_TOT);
void Fill_incident_TOP(G4double p_incident_TOP, G4double PDG_TOP,G4double ID_TOP, G4double PID_TOP, G4double Ek_TOP, G4double x_TOP,G4double y_TOP,G4double z_TOP);
void Fill_incident_BOT(G4double p_incident_BOT, G4double PDG_BOT,G4double ID_BOT, G4double PID_BOT, G4double Ek_BOT, G4double x_BOT,G4double y_BOT,G4double z_BOT);

void Fill_charge_TOT(G4double p_charge_TOT,G4double ID_carica_TOT, G4double PID_carica_TOT,G4double PDG_carica_TOT,G4double Ek_carica_TOT, G4double x_charge_TOT,G4double y_charge_TOT,G4double z_charge_TOT,G4double eIoni_interaction_TOT,G4double msc_interaction_TOT,G4double eBrem_interaction_TOT, G4double x_gapTOT_creation,G4double y_gapTOT_creation,G4double z_gapTOT_creation,G4double compton_creator_TOT,G4double photo_creator_TOT);

void Fill_charge_GAP1(G4double p_charge_GAP1,G4double ID_carica_GAP1, G4double PID_carica_GAP1,G4double PDG_carica_GAP1,G4double Ek_carica_GAP1, G4double x_charge_GAP1,G4double y_charge_GAP1,G4double z_charge_GAP1,G4double eIoni_interaction_GAP1,G4double msc_interaction_GAP1,G4double eBrem_interaction_GAP1, G4double x_gap1_creation,G4double y_gap1_creation,G4double z_gap1_creation,G4double compton_creator_GAP1,G4double photo_creator_GAP1);

void Fill_charge_GAP2(G4double p_charge_GAP2,G4double ID_carica_GAP2, G4double PID_carica_GAP2,G4double PDG_carica_GAP2, G4double Ek_carica_GAP2,G4double x_charge_GAP2,G4double y_charge_GAP2,G4double z_charge_GAP2, G4double eIoni_interaction_GAP2,G4double msc_interaction_GAP2,G4double eBrem_interaction_GAP2, G4double x_gap2_creation,G4double y_gap2_creation,G4double z_gap2_creation,G4double compton_creator_GAP2,G4double photo_creator_GAP2);

void Fill_ttree_incident_signal();	// metodo per riempire histogramma di Ek di particelle incidenti che producono segnale

//void Fill_raw();		// riempie ttree raw ad ogni fine evento
//void rawReset();	       // resetta variabili per ttree raw


void Fill_step( G4double x_step, G4double y_step, G4double z_step, G4double edep_step, G4double eIon_step, G4double photo_kodel, G4double compton_kodel, G4double msc_kodel ,G4double ion_kodel, G4double eBrem_kodel);	// riempie ad ogni step con energia e posizione SE il volume corrisponde  a korea

void Get_EventNumber_histo(G4int nbEvents);
G4double GetSensitivity() const { return sensitivity; }		//passa valore sensitivity a runaction per plottare a fine run il valore 	

void Fill_final_histograms();    	// metodo da chiamare a fine run per creare istogrammi finali

void save();		//metodo per salvare tutto su root file a fine run

        
  private:

 static HistoManager* fgInstance;    // static method (per accere da altri file)
   char fStartTime[50];			//per nome root file
  
    TFile*   rootFile;        

   // ttree per eventi
   TTree*   ttree_incident_TOT; 	// ttree dove salvare dati particelle incidenti bot+top
   TTree*   ttree_incident_TOP; 	// ttree dove salvare dati particelle incidenti top
   TTree*   ttree_incident_BOT; 	// ttree dove salvare dati particelle incidenti BOT

   TTree*   ttree_charge_TOT; 	// ttree dove salvare dati particelle cariche in gap1+gap2
   TTree*   ttree_charge_GAP1; 	// ttree dove salvare dati particelle cariche in gap1
   TTree*   ttree_charge_GAP2; 	// ttree dove salvare dati particelle cariche in gap2

   TTree* ttree_Ek_incident_signal; 		        // ttree energia di particelle incidenti che producono segnale

   // ttree per ogni raw
   // TTree*   ttree_raw; 		// riempie ad ogni fine evento, compreso quando valori sono zero

    // ttree per ogni step
    TTree*   ttree_step; 	// 2 ttree dove salvare dati a fine ogni step 


   // hisotgrammi finali (creati a fine run)

    TH1I* nb_events_requested_histo; 		// histo per vedere numero eventi richiesti
    TH1D* sensitivity_histo; 		//istogramma sensitività (n p cariche tot / n tot incidenti)
    TH2D* xy_TOP; 			//istogramma 2D x-y (peso sempre = 1) per distribuzione su TOP
    TH2D* xy_BOT; 			//istogramma 2D x-y (peso sempre = 1) per distribuzione su BOT
    TH2D* xy_GAP1; 			//istogramma 2D x-y (peso sempre = 1) per distribuzione su GAP1
    TH2D* xy_GAP2; 			//istogramma 2D x-y (peso sempre = 1) per distribuzione su GAP2
    TH1D* edepz; 		        //energia media depositata vs z (normalizzata per n°eventi)
    

    // variabili per copiare valori da event o stepping, e poi salvarli in ttree1 event
   //p icnidenti 
   G4double fp_incident_TOT;
    G4double fp_incident_TOP;
    G4double fp_incident_BOT;

   G4double fPDG_TOT;
   G4double fPDG_TOP;
   G4double fPDG_BOT;

   G4double fID_TOT;
   G4double fID_TOP;
   G4double fID_BOT;

   G4double fPID_TOT;
   G4double fPID_TOP;
   G4double fPID_BOT;

   G4double fEk_TOT;
   G4double fEk_TOP;
   G4double fEk_BOT;


    G4double fx_TOT;			//posizioni particella incidente (sia bot che top)
    G4double fy_TOT;
    G4double fz_TOT;

    G4double fx_TOP;			//posizioni particella incidente top
    G4double fy_TOP;
    G4double fz_TOP;

    G4double fx_BOT;			//posizioni particella incidente bot
    G4double fy_BOT;
    G4double fz_BOT;

    // p cariche
    G4double fp_charge;				// numero particelle cariche in gap1+gap2
    G4double fp_charge_GAP1;			// numero particelle cariche in gap1
    G4double fp_charge_GAP2;			// numero particelle cariche in gap2

    G4double fPDG_charge_TOT;
    G4double fPDG_charge_GAP1; 
    G4double fPDG_charge_GAP2;	
    
    G4double fPID_charge_TOT;
    G4double fPID_charge_GAP1; 
    G4double fPID_charge_GAP2;

    G4double fID_charge_TOT;
    G4double fID_charge_GAP1; 
    G4double fID_charge_GAP2;

    G4double fEk_charge_TOT;
    G4double fEk_charge_GAP1; 
    G4double fEk_charge_GAP2;


    G4double fx_charge_TOT;			//posizioni particella incidente (sia gap1 che gap2)
    G4double fy_charge_TOT;
    G4double fz_charge_TOT;

    G4double fx_charge_GAP1;			//posizioni particella incidente gap1
    G4double fy_charge_GAP1;
    G4double fz_charge_GAP1;

    G4double fx_charge_GAP2;			//posizioni particella incidente gap2
    G4double fy_charge_GAP2;
    G4double fz_charge_GAP2;

 G4double f_msc_interaction_TOT ;		// contatore numero interazioni con effetto multiple scattering dell'elettrone in GAP1+GAP2 
 G4double f_eIoni_interaction_TOT ;	// contatore numero interazioni con effetto ionizzazione dell'elettrone in GAP1+GAP2 
 G4double f_eBrem_interaction_TOT ;	// contatore numero interazioni con effetto Bremstra dell'elettrone in GAP1+GAP2 

 G4double f_msc_interaction_GAP1 ;		// contatore numero interazioni con effetto multiple scattering dell'elettrone in GAP1 
 G4double f_eIoni_interaction_GAP1 ;	// contatore numero interazioni con effetto ionizzazione dell'elettrone in GAP1
 G4double f_eBrem_interaction_GAP1 ;	// contatore numero interazioni con effetto Bremstra dell'elettrone in GAP1 
 
 G4double f_msc_interaction_GAP2 ;		// contatore numero interazioni con effetto multiple scattering dell'elettrone in GAP2
 G4double f_eIoni_interaction_GAP2 ;	// contatore numero interazioni con effetto ionizzazione dell'elettrone in GAP2
 G4double f_eBrem_interaction_GAP2 ;	// contatore numero interazioni con effetto Bremstra dell'elettrone in GAP2

// info creazione particelle cariche

 G4double f_z_gapTOT_creation ;			//posizione sulla superficie gap1+GAP2 sulla quale si crea particella carica (getvertexposition)
 G4double f_x_gapTOT_creation ;
 G4double f_y_gapTOT_creation ;

 G4double f_z_gap1_creation ;				//posizione sulla superficie gap1 sulla quale si crea particella carica (getvertexposition)
 G4double f_x_gap1_creation ;
 G4double f_y_gap1_creation ;

 G4double f_z_gap2_creation ;				//posizione sulla superficie gap2 sulla quale si crea particella carica (getvertexposition)
 G4double f_x_gap2_creation ;
 G4double f_y_gap2_creation ;

 G4double f_compton_creator_TOT ;		// contatore numero di volte che il primo elettrone è formato per effetto compton in GAP1+GAP2
 G4double f_photo_creator_TOT ;			// contatore numero di volte che il primo elettrone è formato per effetto fotoelettrico in GAP1+GAP2

 G4double f_compton_creator_GAP1 ;		// contatore numero di volte che il primo elettrone è formato per effetto compton in GAP1
 G4double f_photo_creator_GAP1 ;			// contatore numero di volte che il primo elettrone è formato per effetto fotoelettrico in GAP1

 G4double f_compton_creator_GAP2 ;		// contatore numero di volte che il primo elettrone è formato per effetto compton in GAP1
 G4double f_photo_creator_GAP2 ;			// contatore numero di volte che il primo elettrone è formato per effetto fotoelettrico in GAP1

// variabile in cui copiare valore energia cinetica di SOLO particelle incidenti che hanno prodotto segnale (valore copiato da stepping a fine ogni event) 
 G4double f_Ek_incident_signal ;

   //variabili per copiare valori da event o stepping, e poi salvarli in ttree2 step
    G4double f_x;
    G4double f_y;
    G4double f_z;
    G4double f_edep;
    G4double f_eIon;
    G4double f_photo_kodel; 
    G4double f_compton_kodel;
    G4double f_msc_kodel;
    G4double f_ion_kodel ;
    G4double f_eBrem_kodel; 
   
   //sensitivity
    G4double sensitivity; 

   // variabile per numero eventi in histo
   G4int nevent;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

