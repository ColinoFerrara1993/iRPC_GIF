#include <CLHEP/Units/SystemOfUnits.h>
#include "HistoManager.hh"
#include "G4UnitsTable.hh"
#include "G4Step.hh"
#include "G4LossTableManager.hh"
#include "G4PhysicalConstants.hh"

#include <iomanip>

#ifdef G4ANALYSIS_USE
#include "TH1D.h"
#include "TH2D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1I.h"
#include "TFile.h"
#include "TGraph.h"
#include "TBranch.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TVirtualPad.h"
#include "TPad.h"
#include "TFrame.h"
#include "TLegend.h"
#include "TColor.h"
#include "TAttFill.h"
#include "TStyle.h"
#include "TNtuple.h"
#include "TPad.h"
#include "TMath.h"
#endif
#include "G4SystemOfUnits.hh"



// metodi per accere da ALTRI FILE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager* HistoManager::fgInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager* HistoManager::Instance()
{
// Static acces function via G4RunManager 

  return fgInstance;
}      

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager():
	// root file and ttree
	rootFile(0),ttree_incident_TOT(0),ttree_incident_TOP(0),ttree_incident_BOT(0),ttree_charge_TOT(0),ttree_charge_GAP1(0),ttree_charge_GAP2(0),ttree_step(0),ttree_Ek_incident_signal(0), //ttree_raw(0), 

	// Histograms final
	nb_events_requested_histo(0), sensitivity_histo(0), xy_BOT(0), xy_TOP(0), xy_GAP1(0),xy_GAP2(0), edepz(0),
	nevent(0),	//variabile per plot numero eventi richiesti
	// p incidenti
	fp_incident_TOT(0),fp_incident_TOP(0), fp_incident_BOT(0),      	 // variabili ttree event
	fPDG_TOT(0),fPDG_TOP(0), fPDG_BOT(0),	
	fPID_TOT(0),fPID_TOP(0), fPID_BOT(0),
	fID_TOT(0),fID_TOP(0), fID_BOT(0),
	fEk_TOT(0),fEk_TOP(0), fEk_BOT(0),
	fx_TOT(0),fx_TOP(0),fx_BOT(0),						// variabili posizioni
	fy_TOT(0),fy_TOP(0),fy_BOT(0),
	fz_TOT(0),fz_TOP(0),fz_BOT(0),

	// p cariche
	fp_charge(0), fp_charge_GAP1(0),fp_charge_GAP2(0),			// variabili particelle cariche	 	
	fPDG_charge_TOT(0),fPDG_charge_GAP1(0), fPDG_charge_GAP2(0),	
	fPID_charge_TOT(0),fPID_charge_GAP1(0), fPID_charge_GAP2(0),
	fID_charge_TOT(0),fID_charge_GAP1(0), fID_charge_GAP2(0),
	fEk_charge_TOT(0),fEk_charge_GAP1(0), fEk_charge_GAP2(0),
	fx_charge_TOT(0), fy_charge_TOT(0), fz_charge_TOT(0),			//posizione particelle cariche
	fx_charge_GAP1(0), fy_charge_GAP1(0), fz_charge_GAP1(0),	
	fx_charge_GAP2(0), fy_charge_GAP2(0), fz_charge_GAP2(0),
	f_msc_interaction_TOT (0), f_eIoni_interaction_TOT (0), f_eBrem_interaction_TOT (0),	
	f_msc_interaction_GAP1 (0), f_eIoni_interaction_GAP1 (0),f_eBrem_interaction_GAP1 (0),	
        f_msc_interaction_GAP2 (0),f_eIoni_interaction_GAP2 (0), f_eBrem_interaction_GAP2 (0),
	//info creazione particelle cariche
	f_z_gapTOT_creation (0), f_x_gapTOT_creation (0), f_y_gapTOT_creation (0),
	f_z_gap1_creation (0),f_x_gap1_creation (0), f_y_gap1_creation (0),
	f_z_gap2_creation (0),f_x_gap2_creation (0), f_y_gap2_creation (0),
	f_compton_creator_TOT (0),f_photo_creator_TOT (0),			
	f_compton_creator_GAP1 (0),f_photo_creator_GAP1 (0),			
	f_compton_creator_GAP2 (0),f_photo_creator_GAP2 (0),		
		
	//energia cinetica SOLO p incidenti che hanno prdotto segnale
	f_Ek_incident_signal(0),

	// step
	f_x(0),f_y(0),f_z(0),f_edep(0),f_eIon(0),				// variaibli 2 tree step
       f_photo_kodel(0), f_compton_kodel(0), f_msc_kodel(0),f_ion_kodel(0), f_eBrem_kodel(0),
	//sensitivity
	sensitivity(0)		

  
{ //metodo per accere da altri file
  fgInstance = this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::~HistoManager()
{
  if ( rootFile ) delete rootFile;

//metodo per accedere da altri file
 fgInstance = 0;  
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::book()
{ 
 // Creating a tree container to handle histograms and ntuples.
 // This tree is associated to an output file.
 //
  // create ROOT file (dove alla fine salvare dati e grafici)


	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(fStartTime, 50, "%d_%m_%Y_%H_%M_%S", timeinfo);

	G4String fileName = std::string(fStartTime) +"_GIFpp.root";     //nome file root di uscita


	rootFile = new TFile(fileName, "RECREATE");
	if (!rootFile)
		{
		G4cout << " problem creating the ROOT TFile "
		<< G4endl;
		return;
		}


// creazione ttree p_incident_TOT
 ttree_incident_TOT = new TTree("p_incident_TOT", "p_incident_TOT");

 ttree_incident_TOT->Branch("PARTICLE_INCIDENT_TOT", &fp_incident_TOT, "PARTICLE_INCIDENT_TOT/D");
 ttree_incident_TOT->Branch("PARTICLE_DATA_GROUP_TOT", &fPDG_TOT, "PARTICLE_DATA_GROUP_TOT/D");
 ttree_incident_TOT->Branch("ID_TOT", &fID_TOT, "ID_TOT/D");
 ttree_incident_TOT->Branch("PARENT_ID_TOT", &fPID_TOT, "PARENT_ID_TOT/D");
 ttree_incident_TOT->Branch("KINETIC_ENERGY_TOT", &fEk_TOT, "KINETIC_ENERGY_TOT/D");
 ttree_incident_TOT->Branch("x_TOT", &fx_TOT, "x_TOT/D");
 ttree_incident_TOT->Branch("y_TOT", &fy_TOT, "y_TOT/D");
 ttree_incident_TOT->Branch("z_TOT", &fz_TOT, "z_TOT/D");

// creazione ttree p_incident_TOP
 ttree_incident_TOP = new TTree("p_incident_TOP", "p_incident_TOP");

 ttree_incident_TOP->Branch("PARTICLE_INCIDENT_TOP", &fp_incident_TOP, "PARTICLE_INCIDENT_TOP/D");
 ttree_incident_TOP->Branch("PARTICLE_DATA_GROUP_TOP", &fPDG_TOP, "PARTICLE_DATA_GROUP_TOP/D");
 ttree_incident_TOP->Branch("ID_TOP", &fID_TOP, "ID_TOP/D");
 ttree_incident_TOP->Branch("PARENT_ID_TOP", &fPID_TOP, "PARENT_ID_TOP/D");
 ttree_incident_TOP->Branch("KINETIC_ENERGY_TOP", &fEk_TOP, "KINETIC_ENERGY_TOP/D");
 ttree_incident_TOP->Branch("x_TOP", &fx_TOP, "x_TOP/D");
 ttree_incident_TOP->Branch("y_TOP", &fy_TOP, "y_TOP/D");
 ttree_incident_TOP->Branch("z_TOP", &fz_TOP, "z_TOP/D");


// creazione ttree p_incident_BOT
 ttree_incident_BOT = new TTree("p_incident_BOT", "p_incident_BOT");

 ttree_incident_BOT->Branch("PARTICLE_INCIDENT_BOT", &fp_incident_BOT, "PARTICLE_INCIDENT_BOT/D");
 ttree_incident_BOT->Branch("PARTICLE_DATA_GROUP_BOT", &fPDG_BOT, "PARTICLE_DATA_GROUP_BOT/D");
 ttree_incident_BOT->Branch("ID_BOT", &fID_BOT, "ID_BOT/D");
 ttree_incident_BOT->Branch("PARENT_ID_BOT", &fPID_BOT, "PARENT_ID_BOT/D");
 ttree_incident_BOT->Branch("KINETIC_ENERGY_BOT", &fEk_BOT, "KINETIC_ENERGY_BOT/D");
 ttree_incident_BOT->Branch("x_BOT", &fx_BOT, "x_BOT/D");
 ttree_incident_BOT->Branch("y_BOT", &fy_BOT, "y_BOT/D");
 ttree_incident_BOT->Branch("z_BOT", &fz_BOT, "z_BOT/D");


// creazione ttree CHARGE TOT (GAP1+GAP2)
 ttree_charge_TOT = new TTree("p_charge_TOT", "p_charge_TOT");

 ttree_charge_TOT->Branch("p_charge_TOT", &fp_charge, "p_charge_TOT/D");
 ttree_charge_TOT->Branch("PARTICLE_DATA_GROUP_CHARGE_TOT", &fPDG_charge_TOT, "PARTICLE_DATA_GROUP_CHARGE_TOT/D");
 ttree_charge_TOT->Branch("ID_CHARGE_TOT", &fID_charge_TOT, "ID_TOT/D");
 ttree_charge_TOT->Branch("PARENT_ID_CHARGE_TOT", &fPID_charge_TOT, "PARENT_ID_CHARGE_TOT/D");
 ttree_charge_TOT->Branch("KINETIC_ENERGY_CHARGE_TOT", &fEk_charge_TOT, "KINETIC_ENERGY_CHARGE_TOT/D");
 ttree_charge_TOT->Branch("x_charge_TOT", &fx_charge_TOT, "x_charge_TOT/D");
 ttree_charge_TOT->Branch("y_charge_TOT", &fy_charge_TOT, "y-charge_TOT/D");
 ttree_charge_TOT->Branch("z_charge_TOT", &fz_charge_TOT, "z_charge_TOT/D");
 ttree_charge_TOT->Branch("msc_interaction_TOT", &f_msc_interaction_TOT, "msc_interaction_TOT/D");
 ttree_charge_TOT->Branch("ion_interaction_TOT", &f_eIoni_interaction_TOT, "ion_interaction_TOT/D");
 ttree_charge_TOT->Branch("eBrem_interaction_TOT", &f_eBrem_interaction_TOT, "eBrem_interaction_TOT/D");
 ttree_charge_TOT->Branch("x_creation_TOT", &f_x_gapTOT_creation, "x_creation_TOT/D");
 ttree_charge_TOT->Branch("y_creation_TOT", &f_y_gapTOT_creation, "y_creation_TOT/D");
 ttree_charge_TOT->Branch("z_creation_TOT", &f_z_gapTOT_creation, "z_creation_TOT/D");
 ttree_charge_TOT->Branch("Compton_creation_TOT", &f_compton_creator_TOT, "Compton_creation_TOT/D");
 ttree_charge_TOT->Branch("photo_creation_TOT", &f_photo_creator_TOT, "photo_creation_TOT/D");


// creazione ttree CHARGE GAP1
 ttree_charge_GAP1 = new TTree("p_charge_GAP1", "p_charge_GAP1");

 ttree_charge_GAP1->Branch("p_charge_GAP1", &fp_charge_GAP1, "p_charge_GAP1/D");
 ttree_charge_GAP1->Branch("PARTICLE_DATA_GROUP_CHARGE_GAP1", &fPDG_charge_GAP1, "PARTICLE_DATA_GROUP_CHARGE_GAP1/D");
 ttree_charge_GAP1->Branch("ID_CHARGE_GAP1", &fID_charge_GAP1, "ID_GAP1/D");
 ttree_charge_GAP1->Branch("PARENT_ID_CHARGE_GAP1", &fPID_charge_GAP1, "PARENT_ID_CHARGE_GAP1/D");
 ttree_charge_GAP1->Branch("KINETIC_ENERGY_CHARGE_GAP1", &fEk_charge_GAP1, "KINETIC_ENERGY_CHARGE_GAP1/D");
 ttree_charge_GAP1->Branch("x_charge_GAP1", &fx_charge_GAP1, "x_charge_GAP1/D");
 ttree_charge_GAP1->Branch("y_charge_GAP1", &fy_charge_GAP1, "y-charge_GAP1/D");
 ttree_charge_GAP1->Branch("z_charge_GAP1", &fz_charge_GAP1, "z_charge_GAP1/D");
 ttree_charge_GAP1->Branch("msc_interaction_GAP1", &f_msc_interaction_GAP1, "msc_interaction_GAP1/D");
 ttree_charge_GAP1->Branch("ion_interaction_GAP1", &f_eIoni_interaction_GAP1, "ion_interaction_GAP1/D");
 ttree_charge_GAP1->Branch("eBrem_interaction_GAP1", &f_eBrem_interaction_GAP1, "eBrem_interaction_GAP1/D");
 ttree_charge_GAP1->Branch("x_creation_GAP1", &f_x_gap1_creation, "x_creation_GAP1/D");
 ttree_charge_GAP1->Branch("y_creation_GAP1", &f_y_gap1_creation, "y_creation_GAP1/D");
 ttree_charge_GAP1->Branch("z_creation_GAP1", &f_z_gap1_creation, "z_creation_GAP1/D");
 ttree_charge_GAP1->Branch("Compton_creation_GAP1", &f_compton_creator_GAP1, "Compton_creation_GAP1/D");
 ttree_charge_GAP1->Branch("photo_creation_GAP1", &f_photo_creator_GAP1, "photo_creation_GAP1/D");




// creazione ttree CHARGE GAP2
 ttree_charge_GAP2 = new TTree("p_charge_GAP2", "p_charge_GAP2");

 ttree_charge_GAP2->Branch("p_charge_GAP2", &fp_charge_GAP2, "p_charge_GAP2/D");
 ttree_charge_GAP2->Branch("PARTICLE_DATA_GROUP_CHARGE_GAP2", &fPDG_charge_GAP2, "PARTICLE_DATA_GROUP_CHARGE_GAP2/D");
 ttree_charge_GAP2->Branch("ID_CHARGE_GAP2", &fID_charge_GAP2, "ID_GAP2/D");
 ttree_charge_GAP2->Branch("PARENT_ID_CHARGE_GAP2", &fPID_charge_GAP2, "PARENT_ID_CHARGE_GAP2/D");
 ttree_charge_GAP2->Branch("KINETIC_ENERGY_CHARGE_GAP2", &fEk_charge_GAP2, "KINETIC_ENERGY_CHARGE_GAP2/D");
 ttree_charge_GAP2->Branch("x_charge_GAP2", &fx_charge_GAP2, "x_charge_GAP2/D");
 ttree_charge_GAP2->Branch("y_charge_GAP2", &fy_charge_GAP2, "y-charge_GAP2/D");
 ttree_charge_GAP2->Branch("z_charge_GAP2", &fz_charge_GAP2, "z_charge_GAP2/D");
 ttree_charge_GAP2->Branch("msc_interaction_GAP2", &f_msc_interaction_GAP2, "msc_interaction_GAP2/D");
 ttree_charge_GAP2->Branch("ion_interaction_GAP2", &f_eIoni_interaction_GAP2, "ion_interaction_GAP2/D");
 ttree_charge_GAP2->Branch("eBrem_interaction_GAP2", &f_eBrem_interaction_GAP2, "eBrem_interaction_GAP2/D");
 ttree_charge_GAP2->Branch("x_creation_GAP2", &f_x_gap2_creation, "x_creation_GAP2/D");
 ttree_charge_GAP2->Branch("y_creation_GAP2", &f_y_gap2_creation, "y_creation_GAP2/D");
 ttree_charge_GAP2->Branch("z_creation_GAP2", &f_z_gap2_creation, "z_creation_GAP2/D");
 ttree_charge_GAP2->Branch("Compton_creation_GAP2", &f_compton_creator_GAP2, "Compton_creation_GAP2/D");
 ttree_charge_GAP2->Branch("photo_creation_GAP2", &f_photo_creator_GAP2, "photo_creation_GAP2/D");



// creazione ttree p incident signal (ttree con ek di particelle incidenti che creano segnale (elettrone in gap)
 ttree_Ek_incident_signal = new TTree("Ek_incident_signal", "Ek_incident_signal");

 ttree_Ek_incident_signal->Branch("Ek_incident_signal", &f_Ek_incident_signal, "Ek_incident_signal/D");



/*
// creazione ttree RAW
 ttree_raw = new TTree("raw", "raw");

 ttree_raw->Branch("PARTICLE_INCIDENT_TOT", &fp_incident_TOT, "PARTICLE_INCIDENT_TOT/D");
 ttree_raw->Branch("PARTICLE_DATA_GROUP_TOT", &fPDG_TOT, "PARTICLE_DATA_GROUP_TOT/D");
 ttree_raw->Branch("ID_TOT", &fID_TOT, "ID_TOT/D");
 ttree_raw->Branch("PARENT_ID_TOT", &fPID_TOT, "PARENT_ID_TOT/D");
 ttree_raw->Branch("KINETIC_ENERGY_TOT", &fEk_TOT, "KINETIC_ENERGY_TOT/D");
 ttree_raw->Branch("x_TOT", &fx_TOT, "x_TOT/D");
 ttree_raw->Branch("y_TOT", &fy_TOT, "y_TOT/D");
 ttree_raw->Branch("z_TOT", &fz_TOT, "z_TOT/D");
 ttree_raw->Branch("PARTICLE_INCIDENT_TOP", &fp_incident_TOP, "PARTICLE_INCIDENT_TOP/D");
 ttree_raw->Branch("PARTICLE_DATA_GROUP_TOP", &fPDG_TOP, "PARTICLE_DATA_GROUP_TOP/D");
 ttree_raw->Branch("ID_TOP", &fID_TOP, "ID_TOP/D");
 ttree_raw->Branch("PARENT_ID_TOP", &fPID_TOP, "PARENT_ID_TOP/D");
 ttree_raw->Branch("KINETIC_ENERGY_TOP", &fEk_TOP, "KINETIC_ENERGY_TOP/D");
 ttree_raw->Branch("x_TOP", &fx_TOP, "x_TOP/D");
 ttree_raw->Branch("y_TOP", &fy_TOP, "y_TOP/D");
 ttree_raw->Branch("z_TOP", &fz_TOP, "z_TOP/D");
 ttree_raw->Branch("PARTICLE_INCIDENT_BOT", &fp_incident_BOT, "PARTICLE_INCIDENT_BOT/D");
 ttree_raw->Branch("PARTICLE_DATA_GROUP_BOT", &fPDG_BOT, "PARTICLE_DATA_GROUP_BOT/D");
 ttree_raw->Branch("ID_BOT", &fID_BOT, "ID_BOT/D");
 ttree_raw->Branch("PARENT_ID_BOT", &fPID_BOT, "PARENT_ID_BOT/D");
 ttree_raw->Branch("KINETIC_ENERGY_BOT", &fEk_BOT, "KINETIC_ENERGY_BOT/D");
 ttree_raw->Branch("x_BOT", &fx_BOT, "x_BOT/D");
 ttree_raw->Branch("y_BOT", &fy_BOT, "y_BOT/D");
 ttree_raw->Branch("z_BOT", &fz_BOT, "z_BOT/D");
 ttree_raw->Branch("p_charge_TOT", &fp_charge, "p_charge_TOT/D");
 ttree_raw->Branch("PARTICLE_DATA_GROUP_CHARGE_TOT", &fPDG_charge_TOT, "PARTICLE_DATA_GROUP_CHARGE_TOT/D");
 ttree_raw->Branch("ID_CHARGE_TOT", &fID_charge_TOT, "ID_TOT/D");
 ttree_raw->Branch("PARENT_ID_CHARGE_TOT", &fPID_charge_TOT, "PARENT_ID_CHARGE_TOT/D");
 ttree_raw->Branch("KINETIC_ENERGY_CHARGE_TOT", &fEk_charge_TOT, "KINETIC_ENERGY_CHARGE_TOT/D");
 ttree_raw->Branch("x_charge_TOT", &fx_charge_TOT, "x_charge_TOT/D");
 ttree_raw->Branch("y_charge_TOT", &fy_charge_TOT, "y-charge_TOT/D");
 ttree_raw->Branch("z_charge_TOT", &fz_charge_TOT, "z_charge_TOT/D");
 ttree_raw->Branch("msc_interaction_TOT", &f_msc_interaction_TOT, "msc_interaction_TOT/D");
 ttree_raw->Branch("ion_interaction_TOT", &f_eIoni_interaction_TOT, "ion_interaction_TOT/D");
 ttree_raw->Branch("eBrem_interaction_TOT", &f_eBrem_interaction_TOT, "eBrem_interaction_TOT/D");
 ttree_raw->Branch("x_creation_TOT", &f_x_gapTOT_creation, "x_creation_TOT/D");
 ttree_raw->Branch("y_creation_TOT", &f_y_gapTOT_creation, "y_creation_TOT/D");
 ttree_raw->Branch("z_creation_TOT", &f_z_gapTOT_creation, "z_creation_TOT/D");
 ttree_raw->Branch("Compton_creation_TOT", &f_compton_creator_TOT, "Compton_creation_TOT/D");
 ttree_raw->Branch("photo_creation_TOT", &f_compton_creator_TOT, "photo_creation_TOT/D");
 ttree_raw->Branch("p_charge_GAP1", &fp_charge_GAP1, "p_charge_GAP1/D");
 ttree_raw->Branch("PARTICLE_DATA_GROUP_CHARGE_GAP1", &fPDG_charge_GAP1, "PARTICLE_DATA_GROUP_CHARGE_GAP1/D");
 ttree_raw->Branch("ID_CHARGE_GAP1", &fID_charge_GAP1, "ID_GAP1/D");
 ttree_raw->Branch("PARENT_ID_CHARGE_GAP1", &fPID_charge_GAP1, "PARENT_ID_CHARGE_GAP1/D");
 ttree_raw->Branch("KINETIC_ENERGY_CHARGE_GAP1", &fEk_charge_GAP1, "KINETIC_ENERGY_CHARGE_GAP1/D");
 ttree_raw->Branch("x_charge_GAP1", &fx_charge_GAP1, "x_charge_GAP1/D");
 ttree_raw->Branch("y_charge_GAP1", &fy_charge_GAP1, "y-charge_GAP1/D");
 ttree_raw->Branch("z_charge_GAP1", &fz_charge_GAP1, "z_charge_GAP1/D");
 ttree_raw->Branch("msc_interaction_GAP1", &f_msc_interaction_GAP1, "msc_interaction_GAP1/D");
 ttree_raw->Branch("ion_interaction_GAP1", &f_eIoni_interaction_GAP1, "ion_interaction_GAP1/D");
 ttree_raw->Branch("eBrem_interaction_GAP1", &f_eBrem_interaction_GAP1, "eBrem_interaction_GAP1/D");
 ttree_raw->Branch("x_creation_GAP1", &f_x_gap1_creation, "x_creation_GAP1/D");
 ttree_raw->Branch("y_creation_GAP1", &f_y_gap1_creation, "y_creation_GAP1/D");
 ttree_raw->Branch("z_creation_GAP1", &f_z_gap1_creation, "z_creation_GAP1/D");
 ttree_raw->Branch("Compton_creation_GAP1", &f_compton_creator_GAP1, "Compton_creation_GAP1/D");
 ttree_raw->Branch("photo_creation_GAP1", &f_compton_creator_GAP1, "photo_creation_GAP1/D");
 ttree_raw->Branch("p_charge_GAP2", &fp_charge_GAP2, "p_charge_GAP2/D");
 ttree_raw->Branch("PARTICLE_DATA_GROUP_CHARGE_GAP2", &fPDG_charge_GAP2, "PARTICLE_DATA_GROUP_CHARGE_GAP2/D");
 ttree_raw->Branch("ID_CHARGE_GAP2", &fID_charge_GAP2, "ID_GAP2/D");
 ttree_raw->Branch("PARENT_ID_CHARGE_GAP2", &fPID_charge_GAP2, "PARENT_ID_CHARGE_GAP2/D");
 ttree_raw->Branch("KINETIC_ENERGY_CHARGE_GAP2", &fEk_charge_GAP2, "KINETIC_ENERGY_CHARGE_GAP2/D");
 ttree_raw->Branch("x_charge_GAP2", &fx_charge_GAP2, "x_charge_GAP2/D");
 ttree_raw->Branch("y_charge_GAP2", &fy_charge_GAP2, "y_charge_GAP2/D");
 ttree_raw->Branch("z_charge_GAP2", &fz_charge_GAP2, "z_charge_GAP2/D");
 ttree_raw->Branch("msc_interaction_GAP2", &f_msc_interaction_GAP2, "msc_interaction_GAP2/D");
 ttree_raw->Branch("ion_interaction_GAP2", &f_eIoni_interaction_GAP2, "ion_interaction_GAP2/D");
 ttree_raw->Branch("eBrem_interaction_GAP2", &f_eBrem_interaction_GAP2, "eBrem_interaction_GAP2/D");
 ttree_raw->Branch("x_creation_GAP2", &f_x_gap2_creation, "x_creation_GAP2/D");
 ttree_raw->Branch("y_creation_GAP2", &f_y_gap2_creation, "y_creation_GAP2/D");
 ttree_raw->Branch("z_creation_GAP2", &f_z_gap2_creation, "z_creation_GAP2/D");
 ttree_raw->Branch("Compton_creation_GAP2", &f_compton_creator_GAP2, "Compton_creation_GAP2/D");
 ttree_raw->Branch("photo_creation_GAP2", &f_photo_creator_GAP2, "photo_creation_GAP2/D");
*/

// creazione ttree STEP
 ttree_step = new TTree("step", "step");

 ttree_step->Branch("x_step", &f_x, "x_step/D");
 ttree_step->Branch("y_step", &f_y, "y_step/D");
 ttree_step->Branch("z_step", &f_z, "z_step/D");
 ttree_step->Branch("deposited_energy_step", &f_edep, "deposited_energy_step/D");
 ttree_step->Branch("Ionization_energy_step", &f_eIon, "Ionization_energy_step/D");

 ttree_step->Branch("photoelectric_phenomenons_step", &f_photo_kodel, "photoelectric_phenomenons_step/D");
 ttree_step->Branch("compton_phenomenons_step", &f_compton_kodel, "compton_phenomenons_step/D");
 ttree_step->Branch("msc_phenomenons_step", &f_msc_kodel, "msc_phenomenons_step/D");
 ttree_step->Branch("ionization_phenomenons_step", &f_ion_kodel, "ionization_phenomenons_step/D");
 ttree_step->Branch("eBrem_phenomenons_step", &f_eBrem_kodel, "eBrem_phenomenons_step/D");

} // CHIUSURA METEDODO BOOK



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//    METODI PER RIEMPIRE GLI HISTOGRAMMI


//Metodo per riempire histogram p_incident TOT

void HistoManager::Fill_incident_TOT(G4double p_incident_TOT, G4double PDG_TOT,G4double ID_TOT, G4double PID_TOT, G4double Ek_TOT,G4double x_TOT,G4double y_TOT,G4double z_TOT)

{
 fp_incident_TOT = p_incident_TOT;
 fPDG_TOT = PDG_TOT;
 fID_TOT = ID_TOT;
 fPID_TOT = PID_TOT;
 fEk_TOT = Ek_TOT;
 fx_TOT=x_TOT;
 fy_TOT=y_TOT;
 fz_TOT=z_TOT;

  if (ttree_incident_TOT) ttree_incident_TOT->Fill();
}


//Metodo per riempire histogram p_incident TOP

void HistoManager::Fill_incident_TOP(G4double p_incident_TOP, G4double PDG_TOP,G4double ID_TOP, G4double PID_TOP, G4double Ek_TOP,G4double x_TOP,G4double y_TOP,G4double z_TOP)
{
 fp_incident_TOP = p_incident_TOP;
 fPDG_TOP = PDG_TOP;
 fID_TOP = ID_TOP;
 fPID_TOP = PID_TOP;
 fEk_TOP = Ek_TOP;
 fx_TOP=x_TOP;
 fy_TOP=y_TOP;
 fz_TOP=z_TOP;


  if (ttree_incident_TOP) ttree_incident_TOP->Fill();
}



//Metodo per riempire histogram p_incident BOT

void HistoManager::Fill_incident_BOT(G4double p_incident_BOT, G4double PDG_BOT,G4double ID_BOT, G4double PID_BOT, G4double Ek_BOT,G4double x_BOT,G4double y_BOT,G4double z_BOT)
{
 fp_incident_BOT = p_incident_BOT;
 fPDG_BOT = PDG_BOT;
 fID_BOT = ID_BOT;
 fPID_BOT = PID_BOT;
 fEk_BOT = Ek_BOT;
 fx_BOT=x_BOT;
 fy_BOT=y_BOT;
 fz_BOT=z_BOT;


  if (ttree_incident_BOT) ttree_incident_BOT->Fill();
}



//Metodo per riempire histogram p charge TOT (GAP1+GAP2)

void HistoManager::Fill_charge_TOT(G4double p_charge_TOT,G4double ID_carica_TOT, G4double PID_carica_TOT,G4double PDG_carica_TOT,G4double Ek_carica_TOT, G4double x_charge_TOT,G4double y_charge_TOT,G4double z_charge_TOT,G4double eIoni_interaction_TOT,G4double msc_interaction_TOT,G4double eBrem_interaction_TOT, G4double x_gapTOT_creation,G4double y_gapTOT_creation,G4double z_gapTOT_creation,G4double compton_creator_TOT,G4double photo_creator_TOT)
{
 fp_charge = p_charge_TOT;
 fPDG_charge_TOT = PDG_carica_TOT;
 fID_charge_TOT = ID_carica_TOT;
 fPID_charge_TOT = PID_carica_TOT;
 fEk_charge_TOT = Ek_carica_TOT;
 fx_charge_TOT= x_charge_TOT;
 fy_charge_TOT= y_charge_TOT;
 fz_charge_TOT= z_charge_TOT;
 f_msc_interaction_TOT=msc_interaction_TOT ;		
 f_eIoni_interaction_TOT=eIoni_interaction_TOT ;	
 f_eBrem_interaction_TOT=eBrem_interaction_TOT ;
 f_z_gapTOT_creation=z_gapTOT_creation ;			
 f_x_gapTOT_creation=x_gapTOT_creation ;
 f_y_gapTOT_creation=y_gapTOT_creation ;
 f_compton_creator_TOT=compton_creator_TOT ;		
 f_photo_creator_TOT=photo_creator_TOT ;	

  if (ttree_charge_TOT) ttree_charge_TOT->Fill();
}



//Metodo per riempire histogram p charge GAP1

void HistoManager::Fill_charge_GAP1(G4double p_charge_GAP1,G4double ID_carica_GAP1, G4double PID_carica_GAP1,G4double PDG_carica_GAP1,G4double Ek_carica_GAP1, G4double x_charge_GAP1,G4double y_charge_GAP1,G4double z_charge_GAP1,G4double eIoni_interaction_GAP1,G4double msc_interaction_GAP1,G4double eBrem_interaction_GAP1, G4double x_gap1_creation,G4double y_gap1_creation,G4double z_gap1_creation,G4double compton_creator_GAP1,G4double photo_creator_GAP1)
{
 fp_charge_GAP1 = p_charge_GAP1;
 fPDG_charge_GAP1 = PDG_carica_GAP1;
 fID_charge_GAP1 = ID_carica_GAP1;
 fPID_charge_GAP1 = PID_carica_GAP1;
 fEk_charge_GAP1 = Ek_carica_GAP1;
 fx_charge_GAP1= x_charge_GAP1;
 fy_charge_GAP1= y_charge_GAP1;
 fz_charge_GAP1= z_charge_GAP1;
 f_msc_interaction_GAP1=msc_interaction_GAP1 ;		
 f_eIoni_interaction_GAP1=eIoni_interaction_GAP1 ;	
 f_eBrem_interaction_GAP1=eBrem_interaction_GAP1 ;
 f_z_gap1_creation=z_gap1_creation ;			
 f_x_gap1_creation=x_gap1_creation ;
 f_y_gap1_creation=y_gap1_creation ;
 f_compton_creator_GAP1=compton_creator_GAP1 ;		
 f_photo_creator_GAP1=photo_creator_GAP1 ;

  if (ttree_charge_GAP1) ttree_charge_GAP1->Fill();
}



//Metodo per riempire histogram p charge GAP2

void HistoManager::Fill_charge_GAP2(G4double p_charge_GAP2,G4double ID_carica_GAP2, G4double PID_carica_GAP2,G4double PDG_carica_GAP2, G4double Ek_carica_GAP2,G4double x_charge_GAP2,G4double y_charge_GAP2,G4double z_charge_GAP2, G4double eIoni_interaction_GAP2,G4double msc_interaction_GAP2,G4double eBrem_interaction_GAP2, G4double x_gap2_creation,G4double y_gap2_creation,G4double z_gap2_creation,G4double compton_creator_GAP2,G4double photo_creator_GAP2)
{
 fp_charge_GAP2 = p_charge_GAP2;
 fPDG_charge_GAP2 = PDG_carica_GAP2;
 fID_charge_GAP2 = ID_carica_GAP2;
 fPID_charge_GAP2 = PID_carica_GAP2;
 fEk_charge_GAP2 = Ek_carica_GAP2;
 fx_charge_GAP2= x_charge_GAP2;
 fy_charge_GAP2= y_charge_GAP2;
 fz_charge_GAP2= z_charge_GAP2;
 f_msc_interaction_GAP2=msc_interaction_GAP2 ;		
 f_eIoni_interaction_GAP2=eIoni_interaction_GAP2 ;	
 f_eBrem_interaction_GAP2=eBrem_interaction_GAP2 ;
 f_z_gap2_creation=z_gap2_creation ;			
 f_x_gap2_creation=x_gap2_creation ;
 f_y_gap2_creation=y_gap2_creation ;
 f_compton_creator_GAP2=compton_creator_GAP2 ;		
 f_photo_creator_GAP2=photo_creator_GAP2 ;


  if (ttree_charge_GAP2) ttree_charge_GAP2->Fill();
}


//void HistoManager::Fill_raw()

//{
//  if (ttree_raw) ttree_raw->Fill();
//}



void HistoManager::Fill_step( G4double x_step, G4double y_step, G4double z_step, G4double edep_step, G4double eIon_step, G4double photo_kodel, G4double compton_kodel, G4double msc_kodel ,G4double ion_kodel, G4double eBrem_kodel)

{

f_x= x_step;
f_y= y_step;
f_z= z_step;
f_edep = edep_step;
f_eIon = eIon_step;
f_photo_kodel= photo_kodel; 
f_compton_kodel=compton_kodel;
f_msc_kodel= msc_kodel;
f_ion_kodel= ion_kodel;
f_eBrem_kodel=eBrem_kodel;

  if (ttree_step) ttree_step->Fill();
}


void HistoManager::Get_EventNumber_histo(G4int nbEvents)
{
nevent= nbEvents;
}


//metodo per riempire istogramma energia cinetica di SOLO particelle incidenti che producono segnale (chiamato a fine di ogni evento in eventaction)
void HistoManager::Fill_ttree_incident_signal()
{
f_Ek_incident_signal = GIFppSteppingAction::Instance()->GetEk_incident_signal();	//usa metodo di steppingaction per copiare valore energia cinetica di p incidenti che hanno prodotto segnale

 if (ttree_Ek_incident_signal) ttree_Ek_incident_signal->Fill();

}



// funzione per resettare valori ad ogni fine event (per raw ttree)
/*
void HistoManager::rawReset()
{

	fp_incident_TOT=0;fp_incident_TOP=0; fp_incident_BOT=0;      	 // variabili ttree event
	fPDG_TOT=0;fPDG_TOP=0; fPDG_BOT=0;	
	fPID_TOT=0;fPID_TOP=0; fPID_BOT=0;
	fID_TOT=0;fID_TOP=0; fID_BOT=0;
	fEk_TOT=0;fEk_TOP=0; fEk_BOT=0;
	fx_TOT=0;fx_TOP=0;fx_BOT=0;						// variabili posizioni
	fy_TOT=0;fy_TOP=0;fy_BOT=0;
	fz_TOT=0;fz_TOP=0;fz_BOT=0;

	fp_charge=0; fp_charge_GAP1=0;fp_charge_GAP2=0;			// variabili particelle cariche	 	
	fPDG_charge_TOT=0;fPDG_charge_GAP1=0; fPDG_charge_GAP2=0;	
	fPID_charge_TOT=0;fPID_charge_GAP1=0; fPID_charge_GAP2=0;
	fID_charge_TOT=0;fID_charge_GAP1=0; fID_charge_GAP2=0;
	fEk_charge_TOT=0;fEk_charge_GAP1=0; fEk_charge_GAP2=0;
	fx_charge_TOT=0; fy_charge_TOT=0; fz_charge_TOT=0;			//posizione particelle cariche
	fx_charge_GAP1=0; fy_charge_GAP1=0; fz_charge_GAP1=0;	
	fx_charge_GAP2=0; fy_charge_GAP2=0; fz_charge_GAP2=0;	
	f_msc_interaction_TOT =0; f_eIoni_interaction_TOT =0; f_eBrem_interaction_TOT =0;	
	f_msc_interaction_GAP1 =0; f_eIoni_interaction_GAP1 =0;f_eBrem_interaction_GAP1 =0;	
        f_msc_interaction_GAP2 =0;f_eIoni_interaction_GAP2 =0; f_eBrem_interaction_GAP2 =0;
	//info creazione particelle cariche
	f_z_gapTOT_creation =0; f_x_gapTOT_creation =0; f_y_gapTOT_creation =0;
	f_z_gap1_creation =0;f_x_gap1_creation =0; f_y_gap1_creation =0;
	f_z_gap2_creation =0;f_x_gap2_creation =0; f_y_gap2_creation =0;
	f_compton_creator_TOT =0;f_photo_creator_TOT =0;			
	f_compton_creator_GAP1 =0;f_photo_creator_GAP1 =0;			
	f_compton_creator_GAP2 =0;f_photo_creator_GAP2 =0;

	f_x=0;f_y=0;f_z=0;f_edep=0;f_eIon=0;				// variaibli tree step
	f_compton_kodel=0; f_photo_kodel=0;f_msc_kodel=0; f_ion_kodel=0; f_eBrem_kodel=0;
}
*/



// istogrammi fineli (metodo chiamato a fine run)
void HistoManager::Fill_final_histograms()
{

// n eventi richiesti
TH1I* nb_events_requested_histo= new TH1I("n_events_requested","n_events_requested",10,0,2);
nb_events_requested_histo->Fill(1, nevent);
nb_events_requested_histo->GetYaxis()->SetTitle("n events requested");


//SENSITIVITÀ
// estrazione dati p incidenti
ttree_incident_TOT->Draw("PARTICLE_INCIDENT_TOT","","goff");
TH1D* htemp = (TH1D*)ttree_incident_TOT->GetHistogram();
G4double p_incidenti_sensitivity= htemp->GetBinContent(htemp->FindFixBin(1));
delete htemp;		//elimina file htemp temporaneo usato per prendere valori da Tbranch e passarli a TH

if (ttree_charge_TOT->GetEntries() != 0) {
	//estrazione p-cariche interagenti
	ttree_charge_TOT->Draw("p_charge_TOT","","goff");
	TH1D* htemp2 = (TH1D*)ttree_charge_TOT->GetHistogram();
	G4double p_cariche_sensitivity= htemp2->GetBinContent(htemp2->FindFixBin(1));
	delete htemp2;		//elimina file htemp temporaneo usato per prendere valori da Tbranch e passarli a TH

	//calcolo sensitività
	sensitivity= (p_cariche_sensitivity/p_incidenti_sensitivity)*100;	// calcolo sensitività
	TH1D* sensitivity_histo= new TH1D("sensitivity","sensitivity",10,0,2);
	sensitivity_histo->Fill(1, sensitivity);
	sensitivity_histo->GetYaxis()->SetTitle("sensitivity %");
	sensitivity_histo->SetFillColor(4);
	}


//X-Y TOT particelle incidenti

//tolto per questo plot il controllo che il ttree non sia vuoto in modo da avere variabili min e max 
ttree_incident_TOT->Draw("x_TOT","","goff");			// disegna x
TH1D* h_x_TOT = (TH1D*)ttree_incident_TOT->GetHistogram()->Clone();	// assegna x ad histo
G4double x_min_TOT=h_x_TOT->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
G4double x_max_TOT=h_x_TOT->GetXaxis()->GetBinCenter(h_x_TOT->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
delete h_x_TOT;

ttree_incident_TOT->Draw("y_TOT","","goff");			// disegna y
TH1D* h_y_TOT = (TH1D*)ttree_incident_TOT->GetHistogram()->Clone();	// assegna x ad histo
G4double y_min_TOT=h_y_TOT->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
G4double y_max_TOT=h_y_TOT->GetXaxis()->GetBinCenter(h_y_TOT->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
delete h_y_TOT;

TH2D* xy_TOT = new TH2D("xy_incident_TOT", "xy_incident_TOT", 500, x_min_TOT, x_max_TOT,500, y_min_TOT, y_max_TOT);	//n bin x, min-x, max-x, nbin y, y-min, y-max) 
ttree_incident_TOT->Draw("y_TOT:x_TOT>>xy_incident_TOT");   // 1 è la y, 2 la x
xy_TOT->GetYaxis()->SetTitle("y");
xy_TOT->GetXaxis()->SetTitle("x");


//X-Y TOP particelle incidenti

if (ttree_incident_TOT->GetEntries() != 0) { 
	ttree_incident_TOP->Draw("x_TOP","","goff");			// disegna x
	TH1D* h_x_TOP = (TH1D*)ttree_incident_TOP->GetHistogram()->Clone();	// assegna x ad histo
	//G4double x_min_TOP=h_x_TOP->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	//G4double x_max_TOP=h_x_TOP->GetXaxis()->GetBinCenter(h_x_TOP->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_x_TOP;

	ttree_incident_TOP->Draw("y_TOP","","goff");			// disegna y
	TH1D* h_y_TOP = (TH1D*)ttree_incident_TOP->GetHistogram()->Clone();	// assegna x ad histo
	//G4double y_min_TOP=h_y_TOP->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	//G4double y_max_TOP=h_y_TOP->GetXaxis()->GetBinCenter(h_y_TOP->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_y_TOP;

	TH2D* xy_TOP = new TH2D("xy_incident_TOP", "xy_incident_TOP", 500, x_min_TOT, x_max_TOT,500, y_min_TOT, y_max_TOT);	//n bin x, min-x, max-x, nbin y, y-min, y-max) 
	ttree_incident_TOP->Draw("y_TOP:x_TOP>>xy_incident_TOP");   // 1 è la y, 2 la x
	xy_TOP->GetYaxis()->SetTitle("y");
	xy_TOP->GetXaxis()->SetTitle("x");
	}

//X-Y BOT particelle incidenti

if (ttree_incident_BOT->GetEntries() != 0) {
	ttree_incident_BOT->Draw("x_BOT","","goff");			// disegna x
	TH1D* h_x_BOT = (TH1D*)ttree_incident_BOT->GetHistogram()->Clone();	// assegna x ad histo
	//G4double x_min_BOT=h_x_BOT->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	//G4double x_max_BOT=h_x_BOT->GetXaxis()->GetBinCenter(h_x_BOT->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_x_BOT;

	ttree_incident_BOT->Draw("y_BOT","","goff");			// disegna y
	TH1D* h_y_BOT = (TH1D*)ttree_incident_BOT->GetHistogram()->Clone();	// assegna x ad histo
	//G4double y_min_BOT=h_y_BOT->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	//G4double y_max_BOT=h_y_BOT->GetXaxis()->GetBinCenter(h_y_BOT->GetNbinsX());   // prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_y_BOT;

	TH2D* xy_BOT = new TH2D("xy_incident_BOT", "xy_incident_BOT", 500, x_min_TOT, x_max_TOT,500, y_min_TOT, y_max_TOT);    //n bin x, min-x, max-x, nbin y, y-min, y-max)   USA PER TUTTI QUELLA DI TOP PER AVERE STESSE DIMENSIONI
	ttree_incident_BOT->Draw("y_BOT:x_BOT>>xy_incident_BOT");   // 1 è la y, 2 la x
	xy_BOT->GetYaxis()->SetTitle("y");
	xy_BOT->GetXaxis()->SetTitle("x");
	}


//X-Y TOT particelle interagenti

if (ttree_charge_TOT->GetEntries() != 0) {
	ttree_charge_TOT->Draw("x_charge_TOT","","goff");			// disegna x
	TH1D* h_x_TOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	// assegna x ad histo
	//G4double x_min_TOT=h_x_TOT->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	//G4double x_max_TOT=h_x_TOT->GetXaxis()->GetBinCenter(h_x_TOT->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_x_TOT;

	ttree_charge_TOT->Draw("y_charge_TOT","","goff");			// disegna y
	TH1D* h_y_TOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	// assegna x ad histo
	G4double y_min_TOT=h_y_TOT->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	G4double y_max_TOT=h_y_TOT->GetXaxis()->GetBinCenter(h_y_TOT->GetNbinsX());  //prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_y_TOT;

	TH2D* xy_charge_TOT = new TH2D("xy_charge_TOT", "xy_charge_TOT", 500, x_min_TOT, x_max_TOT, 500, y_min_TOT, y_max_TOT);    //n bin x, min-x, max-x, nbin y, y-min, y-max) 
	ttree_charge_TOT->Draw("y_charge_TOT:x_charge_TOT>>xy_charge_TOT");   // 1 è la y, 2 la x
	xy_charge_TOT->GetYaxis()->SetTitle("y");
	xy_charge_TOT->GetXaxis()->SetTitle("x");


//X-Y TOT coordinate posizione in cui sono state create particelle cariche

	ttree_charge_TOT->Draw("x_creation_TOT","","goff");			// disegna x
	TH1D* h_x_creationTOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	// assegna x ad histo
	G4double x_min_creationTOT=h_x_creationTOT->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	G4double x_max_creationTOT=h_x_creationTOT->GetXaxis()->GetBinCenter(h_x_creationTOT->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_x_creationTOT;

	ttree_charge_TOT->Draw("y_creation_TOT","","goff");			// disegna y
	TH1D* h_y_creationTOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	// assegna x ad histo
	G4double y_min_creationTOT=h_y_creationTOT->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	G4double y_max_creationTOT=h_y_creationTOT->GetXaxis()->GetBinCenter(h_y_creationTOT->GetNbinsX());  //prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_y_creationTOT;

	TH2D* xy_creationTOT = new TH2D("xy_creationTOT", "xy_creationTOT", 500, x_min_TOT, x_max_TOT,500, y_min_TOT, y_max_TOT);    //n bin x, min-x, max-x, nbin y, y-min, y-max) 
	ttree_charge_TOT->Draw("y_creation_TOT:x_creation_TOT>>xy_creationTOT");   // 1 è la y, 2 la x
	xy_creationTOT->GetYaxis()->SetTitle("y");
	xy_creationTOT->GetXaxis()->SetTitle("x");


//canvas con interaction e produced TOT insieme:

TCanvas * xy_creation_interaction_TOT = new TCanvas("xy_creation_interaction_TOT","xy_creation_interaction_TOT");		//crea canvas e la nomina

xy_creationTOT->Draw();					//disegna histogramma xy creazione p cariche
xy_creationTOT->SetMarkerColor(4);
xy_creationTOT->SetMarkerStyle(8);
xy_creationTOT->SetLineColor(4);
xy_creationTOT->SetTitle("xy_creation_interaction_TOT");		//setta titolo plot

xy_charge_TOT->Draw("SAME");					//disegna nello stesso grafico xy interazione p cariche
xy_charge_TOT->SetMarkerColor(5);
xy_charge_TOT->SetMarkerStyle(3);
xy_charge_TOT->SetLineColor(5);

TLegend* leg = new TLegend(0.12,0.8,0.3,0.9);			//crea legenda
leg->AddEntry(xy_creationTOT, "xy creation TOT", "lp");	// aggiunge entry  a legenda
leg->AddEntry(xy_charge_TOT, "xy interaction TOT", "lp");	
leg->SetFillColor(0);					// setta colore sfondo legenda
leg->Draw();						// disegna legenda

xy_creation_interaction_TOT->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)
xy_creationTOT->SetTitle("xy_creation_TOT");	// riesetta titolo originale dopo aver salvato canvas (altrimenti rimane cambiato anche nel plot xy solo creation)



//canvas Z interaction e produced TOT insieme:

ttree_charge_TOT->Draw("z_creation_TOT","","goff");			// disegna z di interaction per prendere max e min
TH1D* htempo_creationTOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	//prende histo e lo metto in histo temporaneo per prendere max e min	

G4double z_min_creationTOT= htempo_creationTOT->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico creation-interaction
G4double z_max_creationTOT= htempo_creationTOT->GetXaxis()->GetBinCenter(htempo_creationTOT->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico creation-interaction

ttree_charge_TOT->Draw("z_charge_TOT","","goff");			// disegna z di interaction per prendere max e min
TH1D* htempo_interactionTOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	//prende histo e lo metto in histo temporaneo per prendere max e min	

G4double z_min_interactionTOT= htempo_interactionTOT->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico creation-interaction
G4double z_max_interactionTOT= htempo_interactionTOT->GetXaxis()->GetBinCenter(htempo_interactionTOT->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico creation-interaction


G4double z_min_creation_interaction_TOT;			//variabili per minimo e max TOT
G4double z_max_creation_interaction_TOT;
//scelta min e max
z_min_creationTOT < z_min_interactionTOT ? z_min_creation_interaction_TOT = z_min_creationTOT : z_min_creation_interaction_TOT = z_min_interactionTOT ;
z_max_creationTOT > z_max_interactionTOT ? z_max_creation_interaction_TOT = z_max_creationTOT : z_max_creation_interaction_TOT = z_max_interactionTOT ;


TH1D* h_z_creationTOT = new TH1D("h_z_creationTOT", "z_creation_TOT", 500, z_min_creation_interaction_TOT, z_max_creation_interaction_TOT);		//inizializza histo dove copiare z creation da ttree
ttree_charge_TOT->Draw("z_creation_TOT>>h_z_creationTOT","","goff");				//copia histo 

TH1D*  h_z_interactionTOT = new TH1D("h_z_interactionTOT", "z_interactionTOT", 500, z_min_creation_interaction_TOT, z_max_creation_interaction_TOT); //inizializza histo dove copiare z interaction 
ttree_charge_TOT->Draw("z_charge_TOT>>h_z_interactionTOT","","goff");			     //copia histo


TCanvas *z_creation_interaction_TOT = new TCanvas("z_creation_interaction_TOT","z_creation_interaction_TOT");	//crea canvas e la rinomina

h_z_interactionTOT->Draw();
h_z_interactionTOT->SetLineColor(2);
h_z_interactionTOT->SetFillColor(2);
h_z_interactionTOT->SetFillStyle(3005);
h_z_interactionTOT->GetXaxis()->SetLimits(z_min_creation_interaction_TOT, z_max_creation_interaction_TOT); 	//setta range grafico z
h_z_interactionTOT->SetTitle("z_creation_interaction_TOT");

h_z_creationTOT->Draw("SAME");		//disegna histo in canvas
h_z_creationTOT->SetLineColor(4);
h_z_creationTOT->SetFillColor(4);
h_z_creationTOT->SetFillStyle(3003);



TLegend* legz = new TLegend(0.12,0.8,0.3,0.9);			//crea legenda
legz->AddEntry(h_z_creationTOT, "z creation TOT", "F");	// aggiunge entry  a legenda
legz->AddEntry(h_z_interactionTOT, "z interaction TOT", "F");	
legz->SetFillColor(0);					// setta colore sfondo legenda
legz->Draw();						// disegna legenda


z_creation_interaction_TOT->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)

delete h_z_creationTOT;				//elimina histo temporaneo
delete h_z_interactionTOT;				//elimina histo temporaneo
delete htempo_creationTOT;
delete htempo_interactionTOT;



//canvas compton photoelectric TOT insieme:

ttree_charge_TOT->Draw("Compton_creation_TOT","","goff");			// disegna z di interaction
TH1D* comptonTOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	// assegna x ad histo

ttree_charge_TOT->Draw("photo_creation_TOT","","goff");			// disegna z di produced
TH1D* photoTOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	// assegna x ad histo

TCanvas * compton_photo_creation_TOT = new TCanvas("compton_photo_creation_TOT","compton_photo_creation_TOT");		//crea canvas e la nomina

comptonTOT->Draw();
comptonTOT->SetLineColor(2);
comptonTOT->SetFillColor(2);
comptonTOT->SetFillStyle(3005);
comptonTOT->SetTitle("compton_photo_creation_TOT");

photoTOT->Draw("SAME");		//disegna histo in canvas
photoTOT->SetLineColor(4);
photoTOT->SetFillColor(4);
photoTOT->SetFillStyle(3003);


TLegend* legc = new TLegend(0.12,0.8,0.3,0.9);			//crea legenda
legc->AddEntry(comptonTOT, "compton creation TOT", "F");	// aggiunge entry  a legenda
legc->AddEntry(photoTOT, "photoelectric creation TOT", "F");	
legc->SetFillColor(0);					// setta colore sfondo legenda
legc->Draw();						// disegna legenda


compton_photo_creation_TOT->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)

delete comptonTOT;				//elimina histo temporaneo
delete photoTOT;				//elimina histo temporaneo



//canvas MSC, ION, EBREM, TOT insieme:

ttree_charge_TOT->Draw("msc_interaction_TOT","","goff");			// disegna msc tot
TH1D* mscTOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	// assegna x ad histo

ttree_charge_TOT->Draw("ion_interaction_TOT","","goff");			// disegna ioni di produced
TH1D* ionTOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	// assegna x ad histo

ttree_charge_TOT->Draw("eBrem_interaction_TOT","","goff");			// disegna ioni di produced
TH1D* eBremTOT = (TH1D*)ttree_charge_TOT->GetHistogram()->Clone();	// assegna x ad histo

TCanvas * msc_ion_ebrem_interaction_TOT = new TCanvas("msc_ion_ebrem_interaction_TOT","msc_ion_ebrem_interaction_TOT");		//crea canvas e la nomina

mscTOT->Draw();
mscTOT->SetLineColor(2);
mscTOT->SetFillColor(2);
mscTOT->SetFillStyle(3005);
mscTOT->SetTitle("msc_ion_ebrem_interaction_TOT");

ionTOT->Draw("SAME");		//disegna histo in canvas
ionTOT->SetLineColor(4);
ionTOT->SetFillColor(4);
ionTOT->SetFillStyle(3003);

eBremTOT->Draw("SAME");		//disegna histo in canvas
eBremTOT->SetLineColor(3);
eBremTOT->SetFillColor(3);
eBremTOT->SetFillStyle(3004);


TLegend* legint = new TLegend(0.12,0.75,0.4,0.9);			//crea legenda
legint->AddEntry(mscTOT, "msc interaction TOT", "F");	// aggiunge entry  a legenda
legint->AddEntry(ionTOT, "ionization interaction TOT", "F");
legint->AddEntry(eBremTOT, "bremsstrahlung interaction TOT", "F");	
legint->SetFillColor(0);					// setta colore sfondo legenda
legint->Draw();						// disegna legenda


msc_ion_ebrem_interaction_TOT->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)

delete mscTOT;				//elimina histo temporaneo
delete ionTOT;				//elimina histo temporaneo
delete eBremTOT;				//elimina histo temporaneo


	}		//chiude if charge tot



//X-Y GAP1 particelle interagenti

if (ttree_charge_GAP1->GetEntries() != 0) {
	ttree_charge_GAP1->Draw("x_charge_GAP1","","goff");			// disegna x
	TH1D* h_x_GAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	// assegna x ad histo
	//G4double x_min_GAP1=h_x_GAP1->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	//G4double x_max_GAP1=h_x_GAP1->GetXaxis()->GetBinCenter(h_x_GAP1->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_x_GAP1;

	ttree_charge_GAP1->Draw("y_charge_GAP1","","goff");			// disegna y
	TH1D* h_y_GAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	// assegna x ad histo
	//G4double y_min_GAP1=h_y_GAP1->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	//G4double y_max_GAP1=h_y_GAP1->GetXaxis()->GetBinCenter(h_y_GAP1->GetNbinsX());  //prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_y_GAP1;

	TH2D* xy_GAP1 = new TH2D("xy_charge_GAP1", "xy_charge_GAP1", 500, x_min_TOT, x_max_TOT,500, y_min_TOT, y_max_TOT);    //n bin x, min-x, max-x, nbin y, y-min, y-max) 
	ttree_charge_GAP1->Draw("y_charge_GAP1:x_charge_GAP1>>xy_charge_GAP1");   // 1 è la y, 2 la x
	xy_GAP1->GetYaxis()->SetTitle("y");
	xy_GAP1->GetXaxis()->SetTitle("x");


//X-Y GAP1 coordinate posizione in cui sono state create particelle cariche

	ttree_charge_GAP1->Draw("x_creation_GAP1","","goff");			// disegna x
	TH1D* h_x_creationGAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	// assegna x ad histo
	//G4double x_min_creationGAP1=h_x_creationGAP1->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	//G4double x_max_creationGAP1=h_x_creationGAP1->GetXaxis()->GetBinCenter(h_x_creationGAP1->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_x_creationGAP1;

	ttree_charge_GAP1->Draw("y_creation_GAP1","","goff");			// disegna y
	TH1D* h_y_creationGAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	// assegna x ad histo
	//G4double y_min_creationGAP1=h_y_creationGAP1->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	//G4double y_max_creationGAP1=h_y_creationGAP1->GetXaxis()->GetBinCenter(h_y_creationGAP1->GetNbinsX());  //prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_y_creationGAP1;

	TH2D* xy_creationGAP1 = new TH2D("xy_creationGAP1", "xy_creationGAP1", 500, x_min_TOT, x_max_TOT,500, y_min_TOT, y_max_TOT);    //n bin x, min-x, max-x, nbin y, y-min, y-max) 
	ttree_charge_GAP1->Draw("y_creation_GAP1:x_creation_GAP1>>xy_creationGAP1");   // 1 è la y, 2 la x
	xy_creationGAP1->GetYaxis()->SetTitle("y");
	xy_creationGAP1->GetXaxis()->SetTitle("x");



// X-Y creation + interaction GAP1

//canvas con interaction e produced GAP1 insieme:

TCanvas * xy_creation_interaction_GAP1 = new TCanvas("xy_creation_interaction_GAP1","xy_creation_interaction_GAP1");		//crea canvas e la nomina

xy_creationGAP1->Draw();					//disegna histogramma xy creazione p cariche
xy_creationGAP1->SetMarkerColor(4);
xy_creationGAP1->SetMarkerStyle(8);
xy_creationGAP1->SetLineColor(4);
xy_creationGAP1->SetTitle("xy_creation_interaction_GAP1");		//setta titolo 

xy_GAP1->Draw("SAME");					//disegna nello stesso grafico xy interazione p cariche
xy_GAP1->SetMarkerColor(5);
xy_GAP1->SetMarkerStyle(3);
xy_GAP1->SetLineColor(5);

TLegend* leg = new TLegend(0.12,0.8,0.3,0.9);			//crea legenda
leg->AddEntry(xy_creationGAP1, "xy creation GAP1", "lp");	// aggiunge entry  a legenda
leg->AddEntry(xy_GAP1, "xy interaction GAP1", "lp");	
leg->SetFillColor(0);					// setta colore sfondo legenda
leg->Draw();						// disegna legenda

xy_creation_interaction_GAP1->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)
xy_creationGAP1->SetTitle("xy_creation_GAP1");	//risetta titolo dopo aver salvato la canvas, per grafico xy solo creation che viene salvato successivamente



//canvas Z interaction e produced GAP1 insieme:

ttree_charge_GAP1->Draw("z_creation_GAP1","","goff");			// disegna z di interaction per prendere max e min
TH1D* htempo_creationGAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	//prende histo e lo metto in histo temporaneo per prendere max e min	

G4double z_min_creationGAP1= htempo_creationGAP1->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico creation-interaction
G4double z_max_creationGAP1= htempo_creationGAP1->GetXaxis()->GetBinCenter(htempo_creationGAP1->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico creation-interaction


ttree_charge_GAP1->Draw("z_charge_GAP1","","goff");			// disegna z di interaction per prendere max e min
TH1D* htempo_interactionGAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	//prende histo e lo metto in histo temporaneo per prendere max e min	

G4double z_min_interactionGAP1= htempo_interactionGAP1->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico creation-interaction
G4double z_max_interactionGAP1= htempo_interactionGAP1->GetXaxis()->GetBinCenter(htempo_interactionGAP1->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico creation-interaction


G4double z_min_creation_interaction_GAP1;			//variabili per minimo e max GAP1
G4double z_max_creation_interaction_GAP1;
//scelta min e max
z_min_creationGAP1 < z_min_interactionGAP1 ? z_min_creation_interaction_GAP1 = z_min_creationGAP1 : z_min_creation_interaction_GAP1 = z_min_interactionGAP1 ;
z_max_creationGAP1 > z_max_interactionGAP1 ? z_max_creation_interaction_GAP1 = z_max_creationGAP1 : z_max_creation_interaction_GAP1 = z_max_interactionGAP1 ;


TH1D* h_z_creationGAP1 = new TH1D("h_z_creationGAP1", "z_creation_GAP1", 500, z_min_creation_interaction_GAP1, z_max_creation_interaction_GAP1);		//inizializza histo dove copiare z creation da ttree
ttree_charge_GAP1->Draw("z_creation_GAP1>>h_z_creationGAP1","","goff");				//copia histo 

TH1D*  h_z_interactionGAP1 = new TH1D("h_z_interactionGAP1", "z_interactionGAP1", 500, z_min_creation_interaction_GAP1, z_max_creation_interaction_GAP1); //inizializza histo dove copiare z interaction 
ttree_charge_GAP1->Draw("z_charge_GAP1>>h_z_interactionGAP1","","goff");			     //copia histo


TCanvas *z_creation_interaction_GAP1 = new TCanvas("z_creation_interaction_GAP1","z_creation_interaction_GAP1");	//crea canvas e la rinomina

h_z_interactionGAP1->Draw();
h_z_interactionGAP1->SetLineColor(2);
h_z_interactionGAP1->SetFillColor(2);
h_z_interactionGAP1->SetFillStyle(3005);
h_z_interactionGAP1->GetXaxis()->SetLimits(z_min_creation_interaction_GAP1, z_max_creation_interaction_GAP1); 	//setta range grafico z
h_z_interactionGAP1->SetTitle("z_creation_interaction_GAP1");


h_z_creationGAP1->Draw("SAME");		//disegna histo in canvas
h_z_creationGAP1->SetLineColor(4);
h_z_creationGAP1->SetFillColor(4);
h_z_creationGAP1->SetFillStyle(3003);


TLegend* legz = new TLegend(0.12,0.8,0.3,0.9);			//crea legenda
legz->AddEntry(h_z_creationGAP1, "z creation GAP1", "F");	// aggiunge entry  a legenda
legz->AddEntry(h_z_interactionGAP1, "z interaction GAP1", "F");	
legz->SetFillColor(0);					// setta colore sfondo legenda
legz->Draw();						// disegna legenda


z_creation_interaction_GAP1->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)

delete h_z_creationGAP1;				//elimina histo temporaneo
delete h_z_interactionGAP1;				//elimina histo temporaneo
delete htempo_creationGAP1;
delete htempo_interactionGAP1;


//canvas compton photoelectric GAP1 insieme:

ttree_charge_GAP1->Draw("Compton_creation_GAP1","","goff");			// disegna z di interaction
TH1D* comptonGAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	// assegna x ad histo

ttree_charge_GAP1->Draw("photo_creation_GAP1","","goff");			// disegna z di produced
TH1D* photoGAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	// assegna x ad histo

TCanvas * compton_photo_creation_GAP1 = new TCanvas("compton_photo_creation_GAP1","compton_photo_creation_GAP1");		//crea canvas e la nomina

comptonGAP1->Draw();
comptonGAP1->SetLineColor(2);
comptonGAP1->SetFillColor(2);
comptonGAP1->SetFillStyle(3005);
comptonGAP1->SetTitle("z_creation_interaction_GAP1");

photoGAP1->Draw("SAME");		//disegna histo in canvas
photoGAP1->SetLineColor(4);
photoGAP1->SetFillColor(4);
photoGAP1->SetFillStyle(3003);


TLegend* legc = new TLegend(0.12,0.8,0.3,0.9);			//crea legenda
legc->AddEntry(comptonGAP1, "compton creation GAP1", "F");	// aggiunge entry  a legenda
legc->AddEntry(photoGAP1, "photoelectric creation GAP1", "F");	
legc->SetFillColor(0);					// setta colore sfondo legenda
legc->Draw();						// disegna legenda


compton_photo_creation_GAP1->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)

delete comptonGAP1;				//elimina histo temporaneo
delete photoGAP1;				//elimina histo temporaneo

//canvas MSC, ION, EBREM, GAP1 insieme:

ttree_charge_GAP1->Draw("msc_interaction_GAP1","","goff");			// disegna msc GAP1
TH1D* mscGAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	// assegna x ad histo

ttree_charge_GAP1->Draw("ion_interaction_GAP1","","goff");			// disegna ioni di produced
TH1D* ionGAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	// assegna x ad histo

ttree_charge_GAP1->Draw("eBrem_interaction_GAP1","","goff");			// disegna ioni di produced
TH1D* eBremGAP1 = (TH1D*)ttree_charge_GAP1->GetHistogram()->Clone();	// assegna x ad histo

TCanvas * msc_ion_ebrem_interaction_GAP1 = new TCanvas("msc_ion_ebrem_interaction_GAP1","msc_ion_ebrem_interaction_GAP1");		//crea canvas e la nomina

mscGAP1->Draw();
mscGAP1->SetLineColor(2);
mscGAP1->SetFillColor(2);
mscGAP1->SetFillStyle(3005);
mscGAP1->SetTitle("msc_ion_ebrem_interaction_GAP1");

ionGAP1->Draw("SAME");		//disegna histo in canvas
ionGAP1->SetLineColor(4);
ionGAP1->SetFillColor(4);
ionGAP1->SetFillStyle(3003);

eBremGAP1->Draw("SAME");		//disegna histo in canvas
eBremGAP1->SetLineColor(3);
eBremGAP1->SetFillColor(3);
eBremGAP1->SetFillStyle(3004);


TLegend* legint = new TLegend(0.12,0.75,0.4,0.9);			//crea legenda
legint->AddEntry(mscGAP1, "msc interaction GAP1", "F");	// aggiunge entry  a legenda
legint->AddEntry(ionGAP1, "ionization interaction GAP1", "F");
legint->AddEntry(eBremGAP1, "bremsstrahlung interaction GAP1", "F");	
legint->SetFillColor(0);					// setta colore sfondo legenda
legint->Draw();						// disegna legenda


msc_ion_ebrem_interaction_GAP1->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)

delete mscGAP1;				//elimina histo temporaneo
delete ionGAP1;				//elimina histo temporaneo
delete eBremGAP1;				//elimina histo temporaneo

	}		//chiude if charge GAP1




//X-Y GAP2 particelle interagenti

if (ttree_charge_GAP2->GetEntries() != 0) {
	ttree_charge_GAP2->Draw("x_charge_GAP2","","goff");			// disegna x
	TH1D* h_x_GAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	// assegna x ad histo
	G4double x_min_GAP2=h_x_GAP2->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	G4double x_max_GAP2=h_x_GAP2->GetXaxis()->GetBinCenter(h_x_GAP2->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_x_GAP2;

	ttree_charge_GAP2->Draw("y_charge_GAP2","","goff");			// disegna y
	TH1D* h_y_GAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	// assegna x ad histo
	G4double y_min_GAP2=h_y_GAP2->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	G4double y_max_GAP2=h_y_GAP2->GetXaxis()->GetBinCenter(h_y_GAP2->GetNbinsX());  //prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_y_GAP2;

	TH2D* xy_GAP2 = new TH2D("xy_charge_GAP2", "xy_charge_GAP2", 500, x_min_TOT, x_max_TOT,500, y_min_TOT, y_max_TOT);    //n bin x, min-x, max-x, nbin y, y-min, y-max) 
	ttree_charge_GAP2->Draw("y_charge_GAP2:x_charge_GAP2>>xy_charge_GAP2");   // 1 è la y, 2 la x
	xy_GAP2->GetYaxis()->SetTitle("y");
	xy_GAP2->GetXaxis()->SetTitle("x");


//X-Y GAP2 coordinate posizione in cui sono state create particelle cariche


	ttree_charge_GAP2->Draw("x_creation_GAP2","","goff");			// disegna x
	TH1D* h_x_creationGAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	// assegna x ad histo
	G4double x_min_creationGAP2=h_x_creationGAP2->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	G4double x_max_creationGAP2=h_x_creationGAP2->GetXaxis()->GetBinCenter(h_x_creationGAP2->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_x_creationGAP2;

	ttree_charge_GAP2->Draw("y_creation_GAP2","","goff");			// disegna y
	TH1D* h_y_creationGAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	// assegna x ad histo
	G4double y_min_creationGAP2=h_y_creationGAP2->GetXaxis()->GetBinCenter(0);		// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	G4double y_max_creationGAP2=h_y_creationGAP2->GetXaxis()->GetBinCenter(h_y_creationGAP2->GetNbinsX());  //prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_y_creationGAP2;

	TH2D* xy_creationGAP2 = new TH2D("xy_creationGAP2", "xy_creationGAP2", 500, x_min_TOT, x_max_TOT,500, y_min_TOT, y_max_TOT);    //n bin x, min-x, max-x, nbin y, y-min, y-max) 
	ttree_charge_GAP2->Draw("y_creation_GAP2:x_creation_GAP2>>xy_creationGAP2");   // 1 è la y, 2 la x
	xy_creationGAP2->GetYaxis()->SetTitle("y");
	xy_creationGAP2->GetXaxis()->SetTitle("x");


//canvas con interaction e produced GAP2 insieme:

TCanvas * xy_creation_interaction_GAP2 = new TCanvas("xy_creation_interaction_GAP2","xy_creation_interaction_GAP2");		//crea canvas e la nomina

xy_creationGAP2->Draw();					//disegna histogramma xy creazione p cariche
xy_creationGAP2->SetMarkerColor(4);
xy_creationGAP2->SetMarkerStyle(8);
xy_creationGAP2->SetLineColor(4);
xy_creationGAP2->SetTitle("xy_creation_interaction_GAP2");		//setta titolo 

xy_GAP2->Draw("SAME");					//disegna nello stesso grafico xy interazione p cariche
xy_GAP2->SetMarkerColor(5);
xy_GAP2->SetMarkerStyle(3);
xy_GAP2->SetLineColor(5);

TLegend* leg = new TLegend(0.12,0.8,0.3,0.9);			//crea legenda
leg->AddEntry(xy_creationGAP2, "xy creation GAP2", "lp");	// aggiunge entry  a legenda
leg->AddEntry(xy_GAP2, "xy interaction GAP2", "lp");	
leg->SetFillColor(0);					// setta colore sfondo legenda
leg->Draw();						// disegna legenda

xy_creation_interaction_GAP2->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)
xy_creationGAP2->SetTitle("xy_creation_GAP2");	//risetta titolo dopo aver salvato la canvas, per grafico xy solo creation che viene salvato successivamente



//canvas Z interaction e produced GAP2 insieme:

ttree_charge_GAP2->Draw("z_creation_GAP2","","goff");			// disegna z di interaction per prendere max e min
TH1D* htempo_creationGAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	//prende histo e lo metto in histo temporaneo per prendere max e min	

G4double z_min_creationGAP2= htempo_creationGAP2->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico creation-interaction
G4double z_max_creationGAP2= htempo_creationGAP2->GetXaxis()->GetBinCenter(htempo_creationGAP2->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico creation-interaction


ttree_charge_GAP2->Draw("z_charge_GAP2","","goff");			// disegna z di interaction per prendere max e min
TH1D* htempo_interaction_GAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	//prende histo e lo metto in histo temporaneo per prendere max e min	

G4double z_min_interactionGAP2= htempo_interaction_GAP2->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico creation-interaction
G4double z_max_interactionGAP2= htempo_interaction_GAP2->GetXaxis()->GetBinCenter(htempo_interaction_GAP2->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico creation-interaction


G4double z_min_creation_interaction_GAP2;			//variabili per minimo e max GAP2
G4double z_max_creation_interaction_GAP2;

//scelta min e max
z_min_creationGAP2 < z_min_interactionGAP2 ? z_min_creation_interaction_GAP2 = z_min_creationGAP2 : z_min_creation_interaction_GAP2 = z_min_interactionGAP2;
z_max_creationGAP2 > z_max_interactionGAP2 ? z_max_creation_interaction_GAP2 = z_max_creationGAP2 : z_max_creation_interaction_GAP2 = z_max_interactionGAP2 ;


TH1D* h_z_creationGAP2 = new TH1D("h_z_creationGAP2", "z_creation_GAP2", 500, z_min_creation_interaction_GAP2, z_max_creation_interaction_GAP2);		//inizializza histo dove copiare z creation da ttree
ttree_charge_GAP2->Draw("z_creation_GAP2>>h_z_creationGAP2","","goff");				//copia histo 

TH1D*  h_z_interactionGAP2 = new TH1D("h_z_interactionGAP2", "z_interactionGAP2", 500, z_min_creation_interaction_GAP2, z_max_creation_interaction_GAP2); //inizializza histo dove copiare z interaction 
ttree_charge_GAP2->Draw("z_charge_GAP2>>h_z_interactionGAP2","","goff");			     //copia histo


TCanvas *z_creation_interaction_GAP2 = new TCanvas("z_creation_interaction_GAP2","z_creation_interaction_GAP2");	//crea canvas e la rinomina

h_z_interactionGAP2->Draw();
h_z_interactionGAP2->SetLineColor(2);
h_z_interactionGAP2->SetFillColor(2);
h_z_interactionGAP2->SetFillStyle(3005);
h_z_interactionGAP2->GetXaxis()->SetLimits(z_min_creation_interaction_GAP2, z_max_creation_interaction_GAP2); 	//setta range grafico z
h_z_interactionGAP2->SetTitle("z_creation_interaction_GAP2");


h_z_creationGAP2->Draw("SAME");		//disegna histo in canvas
h_z_creationGAP2->SetLineColor(4);
h_z_creationGAP2->SetFillColor(4);
h_z_creationGAP2->SetFillStyle(3003);


TLegend* legz = new TLegend(0.12,0.8,0.3,0.9);			//crea legenda
legz->AddEntry(h_z_creationGAP2, "z creation GAP2", "F");	// aggiunge entry  a legenda
legz->AddEntry(h_z_interactionGAP2, "z interaction GAP2", "F");	
legz->SetFillColor(0);					// setta colore sfondo legenda
legz->Draw();						// disegna legenda


z_creation_interaction_GAP2->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)

delete h_z_creationGAP2;				//elimina histo temporaneo
delete h_z_interactionGAP2;				//elimina histo temporaneo
delete htempo_creationGAP2;
delete htempo_interaction_GAP2;




//canvas compton photoelectric GAP2 insieme:

ttree_charge_GAP2->Draw("Compton_creation_GAP2","","goff");			// disegna z di interaction
TH1D* comptonGAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	// assegna x ad histo

ttree_charge_GAP2->Draw("photo_creation_GAP2","","goff");			// disegna z di produced
TH1D* photoGAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	// assegna x ad histo

TCanvas * compton_photo_creation_GAP2 = new TCanvas("compton_photo_creation_GAP2","compton_photo_creation_GAP2");		//crea canvas e la nomina

comptonGAP2->Draw();
comptonGAP2->SetLineColor(2);
comptonGAP2->SetFillColor(2);
comptonGAP2->SetFillStyle(3005);
comptonGAP2->SetTitle("z_creation_interaction_GAP2");

photoGAP2->Draw("SAME");		//disegna histo in canvas
photoGAP2->SetLineColor(4);
photoGAP2->SetFillColor(4);
photoGAP2->SetFillStyle(3003);


TLegend* legc = new TLegend(0.12,0.8,0.3,0.9);			//crea legenda
legc->AddEntry(comptonGAP2, "compton creation GAP2", "F");	// aggiunge entry  a legenda
legc->AddEntry(photoGAP2, "photoelectric creation GAP2", "F");	
legc->SetFillColor(0);					// setta colore sfondo legenda
legc->Draw();						// disegna legenda


compton_photo_creation_GAP2->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)

delete comptonGAP2;				//elimina histo temporaneo
delete photoGAP2;				//elimina histo temporaneo



//canvas MSC, ION, EBREM, GAP2 insieme:

ttree_charge_GAP2->Draw("msc_interaction_GAP2","","goff");			// disegna msc GAP2
TH1D* mscGAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	// assegna x ad histo

ttree_charge_GAP2->Draw("ion_interaction_GAP2","","goff");			// disegna ioni di produced
TH1D* ionGAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	// assegna x ad histo

ttree_charge_GAP2->Draw("eBrem_interaction_GAP2","","goff");			// disegna ioni di produced
TH1D* eBremGAP2 = (TH1D*)ttree_charge_GAP2->GetHistogram()->Clone();	// assegna x ad histo

TCanvas * msc_ion_ebrem_interaction_GAP2 = new TCanvas("msc_ion_ebrem_interaction_GAP2","msc_ion_ebrem_interaction_GAP2");		//crea canvas e la nomina

mscGAP2->Draw();
mscGAP2->SetLineColor(2);
mscGAP2->SetFillColor(2);
mscGAP2->SetFillStyle(3005);
mscGAP2->SetTitle("msc_ion_ebrem_interaction_GAP2");

ionGAP2->Draw("SAME");		//disegna histo in canvas
ionGAP2->SetLineColor(4);
ionGAP2->SetFillColor(4);
ionGAP2->SetFillStyle(3003);

eBremGAP2->Draw("SAME");		//disegna histo in canvas
eBremGAP2->SetLineColor(3);
eBremGAP2->SetFillColor(3);
eBremGAP2->SetFillStyle(3004);


TLegend* legint = new TLegend(0.12,0.75,0.4,0.9);			//crea legenda
legint->AddEntry(mscGAP2, "msc interaction GAP2", "F");	// aggiunge entry  a legenda
legint->AddEntry(ionGAP2, "ionization interaction GAP2", "F");
legint->AddEntry(eBremGAP2, "bremsstrahlung interaction GAP2", "F");	
legint->SetFillColor(0);					// setta colore sfondo legenda
legint->Draw();						// disegna legenda


msc_ion_ebrem_interaction_GAP2->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)

delete mscGAP2;				//elimina histo temporaneo
delete ionGAP2;				//elimina histo temporaneo
delete eBremGAP2;				//elimina histo temporaneo

	}		//chiude if charge GAP2





//canvas EK incidenti ed Ek incidenti che producono sengale


ttree_incident_TOT->Draw("KINETIC_ENERGY_TOT","","goff");		// disegna histo per prendere max e min
TH1D* htempor = (TH1D*)ttree_incident_TOT->GetHistogram()->Clone();	//prende histo e lo metto in histo temporaneo per prendere max e min	

G4double min_Ek_incidenti_tot= htempor->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico creation-interaction
G4double max_Ek_incidenti_tot= htempor->GetXaxis()->GetBinCenter(htempor->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico creation-interaction


TH1D*  Ek_incidenti_tot = new TH1D("hKINETIC_ENERGY_TOT", "KINETIC_ENERGY_TOT", 100, min_Ek_incidenti_tot, max_Ek_incidenti_tot);		//inizializza histo dove copiare EK tot da ttree 
ttree_incident_TOT->Draw("KINETIC_ENERGY_TOT>>hKINETIC_ENERGY_TOT","","goff");				//copia histo 

TH1D*  Ek_incidenti_signal = new TH1D("hEk_incident_signal", "KINETIC_ENERGY_INCIDENT_SIGNAL", 100, min_Ek_incidenti_tot, max_Ek_incidenti_tot); //inizializza histo dove copiare EK signal da ttree 
ttree_Ek_incident_signal->Draw("Ek_incident_signal>>hEk_incident_signal","","goff");			     //copia histo


TCanvas * Ek_incident_signal = new TCanvas("Ek_TOT_incident-signal","Ek_TOT_incident-signal");		//crea canvas e la nomina

Ek_incidenti_tot->Draw();					//disegna histo incidenti tot 
Ek_incidenti_tot->SetLineColor(2);
Ek_incidenti_tot->SetFillColor(2);
Ek_incidenti_tot->SetFillStyle(3005);
Ek_incidenti_tot->SetTitle("Ek_incident_signal_TOT");

Ek_incidenti_signal->Draw("SAME");		//disegna histo in canvas
Ek_incidenti_signal->SetLineColor(4);
Ek_incidenti_signal->SetFillColor(4);
Ek_incidenti_signal->SetFillStyle(3001);


TLegend* legs = new TLegend(0.12,0.8,0.4,0.9);			//crea legenda
legs->AddEntry(Ek_incidenti_tot, "Ek incident TOT", "F");	// aggiunge entry  a legenda
legs->AddEntry(Ek_incidenti_signal, "EK incident TOT signal", "F");	
legs->SetFillColor(0);					// setta colore sfondo legenda
legs->Draw();						// disegna legenda


Ek_incident_signal->Write();		//salva la canvas nel file root (prima di ttree e tutto il resto)

//delete Ek_incidenti_tot;	--> rimandata eliminazione a dopo histo relative sensitivity			//elimina histo temporaneo usato per incidenti tot
//delete Ek_incidenti_signal;	--> rimandata eliminazione a dopo histo relative sensitivity			//elimina histo temporaneo usato per incidenti signal
delete htempor;						//elimina histo temporaneo usato per max e min



// RELATIVE SENSITIVITY (singnal/tot)

Ek_incidenti_signal->Divide(Ek_incidenti_tot);		//divide singal per TOT in modo da avere sensitività relativa per ogni bin

Ek_incidenti_signal->Draw();
Ek_incidenti_signal->SetName("relative_sensitivity");
Ek_incidenti_signal->SetTitle("relative_sensitivity");
Ek_incidenti_signal->GetXaxis()->SetTitle("Energy [MeV]");
Ek_incidenti_signal->GetYaxis()->SetTitle("Relative Sensitivity");
Ek_incidenti_signal->SetLineColor(4);
Ek_incidenti_signal->SetFillColor(4);
Ek_incidenti_signal->SetFillStyle(1001);

Ek_incidenti_signal->Write();		//salva histo relative sensitivity

delete Ek_incidenti_tot;	//elimina histo temporaneo usato per incidenti tot
delete Ek_incidenti_signal; 	//elimina histo temporaneo usato per incidenti signal


// EDEPZ energia media depositata vs z (normalizzata per n°eventi)


if (ttree_step->GetEntries() != 0) {
	ttree_step->Draw("z_step","","goff");			// disegna x
	TH1D* h_z_edepz = (TH1D*)ttree_step->GetHistogram()->Clone();	// assegna x ad histo
	G4double z_min_edepz=h_z_edepz->GetXaxis()->GetBinCenter(0);	// prende valore del primo bin, in modo da settare stesso range nel grafico 2d
	G4double z_max_edepz=h_z_edepz->GetXaxis()->GetBinCenter(h_z_edepz->GetNbinsX());	//prende valore dell'ultimo bin in modo da settare stesso range nel grafico 2d
	delete h_z_edepz;

	TH1D* edepz = new TH1D("edepz", "edepz", 500, z_min_edepz, z_max_edepz);    //n bin x, min-x, max-x, nbin y, y-min, y-max) 

	TBranch *br1 = ttree_step->GetBranch("z_step");				// copia TBranch per z
	TBranch *br2 = ttree_step->GetBranch("deposited_energy_step");		//copia tbranche per edep
	G4double value_z;							//inizializza variabile per passare valori z
	G4double value_edep;							//inizializza variabile per passare valori edep
	br1->SetAddress(&value_z);						//passa valori di branch a z
	br2->SetAddress(&value_edep);						//passa valori di branch a edep

	for (int i = 0; i <= ttree_step->GetEntries(); ++i){
		br1->GetEntry(i);							//prende valore i del branch z
		br2->GetEntry(i);							// prende valore i del branch edep
		edepz->Fill(value_z, ((value_edep)/ttree_step->GetEntries()));	// riempie histo edep vs z (DIVISO PER entries per normalizzare(non si può su tutti eventi perchè max size per ttree e ne crea piu di uno di ttree step)
		edepz->GetYaxis()->SetTitle("edep");
		edepz->GetXaxis()->SetTitle("z");
		}
	}

} // fine metodo fill istogrammi finali







//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// SALVATAGGIO FINALE DA CHIAMARE A FINE RUN

void HistoManager::save()
{ 
  if (rootFile) {
    rootFile->Write();       // Writing the histograms to the file
    rootFile->Delete("htemp;1");		// eliminazione histogramma temporaneo
    rootFile->Close();        // and closing the tree (and the file)
    G4cout << "\n----> Histogram Ttrees are saved \n" << G4endl;

 }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


