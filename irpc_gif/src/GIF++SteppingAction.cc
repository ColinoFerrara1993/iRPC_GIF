#include "GIF++SteppingAction.hh"
#include "GIF++RunAction.hh"
#include "GIF++EventAction.hh"
#include "HistoManager.hh"

#include "GIF++DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"


// metodi per accere da ALTRI FILE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppSteppingAction* GIFppSteppingAction::fgInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppSteppingAction* GIFppSteppingAction::Instance()
{
// Static acces function via G4RunManager 

  return fgInstance;
}      

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//oggetti membri

GIFppSteppingAction::GIFppSteppingAction():

// p incidenti
 p_incidente(0),
 p_incidente_top(0),
 p_incidente_bot(0),

 ID_TOT(0),			//ID particelle incidenti sia top che bot
 PID_TOT(0),			//PID parent ID particelle incidenti sia top che bot
 PDG_TOT(0),			//PDG particelle incidenti sia top che bot
 Ek_TOT(0),		//Energia cinetica particelle incidenti si top che bot

 ID_TOP(0),			//ID particelle incidenti  top 
 PID_TOP(0),			//PID parent ID particelle incidenti top
 PDG_TOP(0),			//PDG particelle incidenti  top 
 Ek_TOP(0),		//Energia cinetica particelle incidenti top 

 z_TOP(0),		//posizione particelle incidenti Top				
 x_TOP(0),
 y_TOP(0) ,

 ID_BOT(0),			//ID particelle incidenti bot
 PID_BOT(0),			//PID parent ID particelle incidenti  bot
 PDG_BOT(0),			//PDG particelle incidenti bot
 Ek_BOT(0),		//Energia cinetica particelle incidenti bot

 z_BOT(0) ,		//posizione particelle incidenti BOT				
 x_BOT(0),
 y_BOT(0) ,

//p cariche in gaps
 p_carica(0),
 p_carica_gap1(0),
 p_carica_gap2(0),

 ID_carica_TOT(0),		//ID particelle incidenti sia GAP1 CHE GAP2
 PID_carica_TOT(0),			//PID parent ID particelle incidenti sia GAP1 CHE GAP2
 PDG_carica_TOT(0),			//PDG particelle incidenti sia GAP1 CHE GAP2
 Ek_carica_TOT(0),		//Energia cinetica particelle incidenti sia GAP1 CHE GAP2

 ID_carica_GAP1(0),		
 PID_carica_GAP1(0),			
 PDG_carica_GAP1(0),			
 Ek_carica_GAP1(0),		

 ID_carica_GAP2(0),		
 PID_carica_GAP2(0),			
 PDG_carica_GAP2(0),			
 Ek_carica_GAP2(0),	

 z_gapTOT(0),		//posizione sulla superficie gap1+GAP2 sulla quale particella carica interagisce (edep>0)
 x_gapTOT(0),
 y_gapTOT(0),

 z_gap1(0) ,		//posizione particella carica gap1 sulla quale particella carica interagisce (edep>0)				
 x_gap1(0) ,
 y_gap1(0) ,

 z_gap2(0) ,		//posizione particella carica gap2 sulla quale particella carica interagisce (edep>0)			
 x_gap2(0) ,
 y_gap2(0) ,

 msc_interaction_TOT(0),		// contatore numero interazioni con effetto multiple scattering dell'elettrone in GAP1+GAP2 
 eIoni_interaction_TOT(0),	// contatore numero interazioni con effetto ionizzazione dell'elettrone in GAP1+GAP2 
 eBrem_interaction_TOT(0),	// contatore numero interazioni con effetto Bremstra dell'elettrone in GAP1+GAP2 

 msc_interaction_GAP1(0),		// contatore numero interazioni con effetto multiple scattering dell'elettrone in GAP1 
 eIoni_interaction_GAP1(0),	// contatore numero interazioni con effetto ionizzazione dell'elettrone in GAP1
 eBrem_interaction_GAP1(0),	// contatore numero interazioni con effetto Bremstra dell'elettrone in GAP1 
 
 msc_interaction_GAP2(0),	// contatore numero interazioni con effetto multiple scattering dell'elettrone in GAP2
 eIoni_interaction_GAP2(0),	// contatore numero interazioni con effetto ionizzazione dell'elettrone in GAP2
 eBrem_interaction_GAP2(0),	// contatore numero interazioni con effetto Bremstra dell'elettrone in GAP2
 

// informazioni (posizione e processo creazione) delle particelle cariche che hanno interagito
 z_gapTOT_creation(0),		//posizione sulla superficie gap1+GAP2 sulla quale si crea particella carica (getvertex)
 x_gapTOT_creation(0),
 y_gapTOT_creation(0),

 z_gap1_creation(0) ,		//posizione particella carica gap1  sulla quale si crea particella carica (getvertex)				
 x_gap1_creation(0) ,
 y_gap1_creation(0) ,

 z_gap2_creation(0) ,		//posizione particella carica gap2  sulla quale si crea particella carica (getvertex)		
 x_gap2_creation(0) ,
 y_gap2_creation(0) ,

 compton_creator_TOT(0),		// contatore numero di volte che il primo elettrone è formato per effetto compton in GAP1+GAP2
 photo_creator_TOT(0),			// contatore numero di volte che il primo elettrone è formato per effetto fotoelettrico in GAP1+GAP2

 compton_creator_GAP1(0),		// contatore numero di volte che il primo elettrone è formato per effetto compton in GAP1
 photo_creator_GAP1(0),			// contatore numero di volte che il primo elettrone è formato per effetto fotoelettrico in GAP1

 compton_creator_GAP2(0),		// contatore numero di volte che il primo elettrone è formato per effetto compton in GAP2
 photo_creator_GAP2(0),			// contatore numero di volte che il primo elettrone è formato per effetto fotoelettrico in GAP2

//energia SOLO dei fotoni incidenti che poi producono segnale
Ek_incident_signal(0), incident_signal_status(0),

//info per step
 z(0) ,		//posizione ad ognis step in kodel			
 x(0) ,
 y(0) ,

 edep_kodel(0),			//energia depositata ad ogni step in kodel
 eIon_kodel(0),			// solo energia ionizzazione ad ogni step

compton_kodel(0),		// interazioni per effetto compton in kodel
photo_kodel(0),			// interazioni per effetto fotoelettrico in kodel
ion_kodel(0),			// interazioni per effetto ionizzazione in kodel
msc_kodel(0),			// interazioni per effetto multiple scattering in kodel
eBrem_kodel(0),			// // interazioni per effetto Bremstra in kodel

// info comune a tutti per check su p cariche
 edep(0)			//energia depositata ad ogni step



{ //metodo per accere da altri file
  fgInstance = this;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//eliminazione constructor
GIFppSteppingAction::~GIFppSteppingAction()
{ //metodo per accedere da altri file
 fgInstance = 0;  
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//funzioni membro

void GIFppSteppingAction::UserSteppingAction(const G4Step* step)			//funzione definita da utente: UserSteppingAction
{
  
/*
  // METODI PER PRENDE PARAMETRI 

   G4String particleName= step->GetTrack()->GetParticleDefinition()->GetParticleName();

   G4int PDG = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding(); 

   G4double particleCharge= step->GetTrack()->GetParticleDefinition()->GetPDGCharge();

  G4int trackID = step->GetTrack()->GetTrackID() ;			// track ID

  G4int parentID = step->GetTrack()->GetParentID() ;			// parent ID
 
   G4String processName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() ;  processo step 
   G4string creator_process= step->GetTrack()->GetCreatorProcess()->GetProcessName();    processo da cui si è formata particella (creator process)
 
   G4double eKin = step->GetTrack()->GetKineticEnergy();
 
  G4double edep = step->GetTotalEnergyDeposit();			//energia persa nel mezzo e secondari		
 
  G4double deltaE = step->GetDeltaEnergy();				//energia persa nel mezzo 

  G4double EnonI = step->GetNonIonizingEnergyDeposit();		// energia persa per non ionizzazione 

  G4double EIonization = step->GetTotalEnergyDeposit() - step->GetNonIonizingEnergyDeposit() ;   // energia persa per ionizzazionione

  const G4VTouchable* touchable = step->GetTrack()->GetTouchable();
   G4int volCopyNum = touchable->GetVolume()->GetCopyNo();
   G4String volName = touchable->GetVolume()->GetName() ;


// oopure si può fare anche cosi:
//   const G4VTouchable* touchable = step->GetPreStepPoint()->GetTouchable();
//   G4int volCopyNum = touchable->GetVolume()->GetCopyNo();
//   G4String volName = touchable->GetVolume()->GetName() ;


  G4double z = step->GetTrack()->GetPosition().z() ;
  G4double x = step->GetTrack()->GetPosition().x() ;
  G4double y = step->GetTrack()->GetPosition().y() ;
*/


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppSteppingAction::Instance()->StepReset();		// a inizio di ogni step resetta le variabili step

   // DECISIONE SU QUALI PARAMETRI PRENDERE

   G4double particleCharge= step->GetTrack()->GetParticleDefinition()->GetPDGCharge();		// carica particella

  const G4VTouchable* touchable = step->GetTrack()->GetTouchable();			 //prende volume
   G4int volCopyNum = touchable->GetVolume()->GetCopyNo();
   G4String volName = touchable->GetVolume()->GetName() ;

 G4double edep = step->GetTotalEnergyDeposit();				//energia persa nel mezzo e per i secondary


	//////    PARTICELLE INCIDENTI   ///////
  //contatore particelle incidenti
  if (((volName == "fakeBottom") || (volName == "fakeTop"))&& (p_incidente==0)) {    // particella incidente o sul falso layer top o bottom (per contare numero particelle incidenti) 
         p_incidente=1;					// quando c'è particella incidente, la variabile p_incidente deiventa 1  (x contatore)
	 G4int ID_TOT= step->GetTrack()->GetTrackID() ;						//ID particelle incidenti sia top che bot
	 G4int PID_TOT=	step->GetTrack()->GetParentID() ;					//PID parent ID particelle incidenti sia top che bot
	 G4int PDG_TOT=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding(); 		//PDG particelle incidenti sia top che bot
	 Ek_TOT= step->GetTrack()->GetKineticEnergy();					//Energia cinetica particelle incidenti si top che bot

 	G4double z_TOT = step->GetTrack()->GetPosition().z() ;					//posizione sulla superficie top sulla quale incidono particelle
  	G4double x_TOT = step->GetTrack()->GetPosition().x() ;
  	G4double y_TOT = step->GetTrack()->GetPosition().y() ;        


         HistoManager::Instance()-> Fill_incident_TOT (p_incidente,PDG_TOT, ID_TOT, PID_TOT,Ek_TOT,x_TOT,y_TOT,z_TOT );  //passa le variabili a ttree TOT
        }

  // contatore particelle incidenti solo su fake TOP
  if ((volName == "fakeTop") && (p_incidente_top==0) && (p_incidente_bot==0)) {    // particella incidente  sul falso layer top (per contare numero particelle incidenti) 
           p_incidente_top=1;						// quando c'è particella incidente, la variabile p_incidente deiventa 1  (x contatore)

	 G4int ID_TOP= step->GetTrack()->GetTrackID() ;						//ID particelle incidenti top 
	 G4int PID_TOP=	step->GetTrack()->GetParentID() ;					//PID parent ID particelle incidenti top 
	 G4int PDG_TOP=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding(); 		//PDG particelle incidenti top
	 G4double Ek_TOP= step->GetTrack()->GetKineticEnergy();					//Energia cinetica particelle incidenti top 
  
  	G4double z_TOP = step->GetTrack()->GetPosition().z() ;					//posizione sulla superficie top sulla quale incidono particelle
  	G4double x_TOP = step->GetTrack()->GetPosition().x() ;
  	G4double y_TOP = step->GetTrack()->GetPosition().y() ;
  
         HistoManager::Instance()-> Fill_incident_TOP (p_incidente_top,PDG_TOP, ID_TOP, PID_TOP,Ek_TOP,x_TOP,y_TOP,z_TOP );  //passa le variabili a ttree TOP

       }

  // contatore particelle incidenti solo su fake BOT
  if ((volName == "fakeBottom") && (p_incidente_top==0) && (p_incidente_bot==0)) {    	// particella incidente  sul falso layer top (per contare numero particelle incidenti) 
           p_incidente_bot=1;						// quando c'è particella incidente, la variabile p_incidente deiventa 1  (x contatore)

	 G4int ID_BOT= step->GetTrack()->GetTrackID() ;						//ID particelle incidenti bot
	 G4int PID_BOT=	step->GetTrack()->GetParentID() ;					//PID parent ID particelle incidenti  bot
	 G4int PDG_BOT=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding(); 		//PDG particelle incidenti  bot
	 G4double Ek_BOT= step->GetTrack()->GetKineticEnergy();					//Energia cinetica particelle incidenti bot 
 
 	G4double z_BOT = step->GetTrack()->GetPosition().z();				//posizione sulla superficie bot sulla quale incidono particelle
  	G4double x_BOT = step->GetTrack()->GetPosition().x();
  	G4double y_BOT = step->GetTrack()->GetPosition().y();

         HistoManager::Instance()-> Fill_incident_BOT (p_incidente_bot,PDG_BOT, ID_BOT, PID_BOT,Ek_BOT,x_BOT,y_BOT,z_BOT );  //passa le variabili a ttree BOT

         }	



//////////     PARTICELLE CARICHE IN GAS GAP    ///////////////////

  // contatore particelle cariche sia gap 1 che gap 2
  if (((volName == "GasGap1") || (volName == "GasGap2")) && (particleCharge !=0) && (edep >0) && (p_carica==0)) {  // particella carica e che ha depositato energia in gap 1 o gap 2
         p_carica= 1;					 // quando c'è particella carica, la variabile p_carica deiventa 1 (per contatore)  
  
	ID_carica_TOT = step->GetTrack()->GetTrackID() ;		
 	PID_carica_TOT = step->GetTrack()->GetParentID() ;			
 	PDG_carica_TOT = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding(); 		
 	Ek_carica_TOT = step->GetTrack()->GetKineticEnergy();		

  	G4double z_gapTOT = step->GetTrack()->GetPosition().z();				
  	G4double x_gapTOT = step->GetTrack()->GetPosition().x() ;
  	G4double y_gapTOT = step->GetTrack()->GetPosition().y();

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni")
	{eIoni_interaction_TOT=1;}

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "msc")
	{msc_interaction_TOT =1;}

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eBrem")
	{eBrem_interaction_TOT=1;}

	//ricava informazioni (posizione e processo) da cui si è formata particella carica che ha interagito
	G4ThreeVector posizione_creazione_TOT= step->GetTrack()->GetVertexPosition();    // vettore prende posizione 
	x_gapTOT_creation= posizione_creazione_TOT[0] ;		//posizione sulla superficie gap1 sulla quale si crea particella carica (getvertexposition)
	y_gapTOT_creation= posizione_creazione_TOT[1];		
	z_gapTOT_creation= posizione_creazione_TOT[2] ;	

	if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "compt")
	{compton_creator_TOT=1;}

	if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "phot")
	{photo_creator_TOT=1;}


         HistoManager::Instance()-> Fill_charge_TOT (p_carica,ID_carica_TOT,PID_carica_TOT,PDG_carica_TOT,Ek_carica_TOT, x_gapTOT, y_gapTOT, z_gapTOT,
eIoni_interaction_TOT, msc_interaction_TOT, eBrem_interaction_TOT, x_gapTOT_creation, y_gapTOT_creation, z_gapTOT_creation, compton_creator_TOT, photo_creator_TOT);  //passa le variabili a ttree p carica tot

          }
 
  // contatore particelle cariche in gas gap 1  (può essere passata anche in gap 2 se sufficiente energia)
 if ((volName == "GasGap1") && (particleCharge !=0) && (p_carica_gap1==0) && (edep >0) && (p_carica_gap2==0)) {
           p_carica_gap1=1;

	ID_carica_GAP1 = step->GetTrack()->GetTrackID() ;		
 	PID_carica_GAP1 = step->GetTrack()->GetParentID() ;			
 	PDG_carica_GAP1 = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding(); 		
 	Ek_carica_GAP1 = step->GetTrack()->GetKineticEnergy();

	G4double z_gap1 = step->GetTrack()->GetPosition().z();				//posizione sulla superficie gap2 sulla quale si crea particella carica
  	G4double x_gap1 = step->GetTrack()->GetPosition().x();
  	G4double y_gap1 = step->GetTrack()->GetPosition().y();

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni")
	{eIoni_interaction_GAP1=1;}

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "msc")
	{msc_interaction_GAP1 =1;}

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eBrem")
	{eBrem_interaction_GAP1=1;}


	//ricava informazioni (posizione e processo) da cui si è formata particella carica che ha interagito
	G4ThreeVector posizione_creazione_GAP1= step->GetTrack()->GetVertexPosition();    // vettore prende posizione 
	x_gap1_creation= posizione_creazione_GAP1[0] ;		//posizione sulla superficie gap1 sulla quale si crea particella carica (getvertexposition)
	y_gap1_creation= posizione_creazione_GAP1[1];		
	z_gap1_creation= posizione_creazione_GAP1[2] ;	

	if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "compt")
	{compton_creator_GAP1=1;}

	if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "phot")
	{photo_creator_GAP1=1;}


         HistoManager::Instance()-> Fill_charge_GAP1 (p_carica_gap1,ID_carica_GAP1,PID_carica_GAP1,PDG_carica_GAP1,Ek_carica_GAP1, x_gap1, y_gap1, z_gap1, eIoni_interaction_GAP1, msc_interaction_GAP1, eBrem_interaction_GAP1, x_gap1_creation, y_gap1_creation, z_gap1_creation, compton_creator_GAP1, photo_creator_GAP1 );  //passa le variabili a ttree p carica tot
	}
 

 // contatore particelle cariche in gas gap 2 (può essere passata anche in gap 1 se sufficiente energia)
 if ((volName == "GasGap2") && (particleCharge !=0) && (p_carica_gap2==0) && (edep >0) && (p_carica_gap1==0)) {
           p_carica_gap2=1;

	ID_carica_GAP2 = step->GetTrack()->GetTrackID() ;		
 	PID_carica_GAP2 = step->GetTrack()->GetParentID() ;			
 	PDG_carica_GAP2 = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding(); 		
 	Ek_carica_GAP2 = step->GetTrack()->GetKineticEnergy();

	G4double z_gap2 = step->GetTrack()->GetPosition().z();				//posizione sulla superficie gap2 sulla quale si crea particella carica
  	G4double x_gap2 = step->GetTrack()->GetPosition().x();
  	G4double y_gap2 = step->GetTrack()->GetPosition().y();

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni")
	{eIoni_interaction_GAP2=1;}

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "msc")
	{msc_interaction_GAP2=1;}

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eBrem")
	{eBrem_interaction_GAP2=1;}

	//ricava informazioni (posizione e processo) da cui si è formata particella carica che ha interagito
	G4ThreeVector posizione_creazione_GAP2= step->GetTrack()->GetVertexPosition();    // vettore prende posizione 
	x_gap2_creation= posizione_creazione_GAP2[0] ;		//posizione sulla superficie gap1 sulla quale si crea particella carica (getvertexposition)
	y_gap2_creation= posizione_creazione_GAP2[1];		
	z_gap2_creation= posizione_creazione_GAP2[2] ;	

	if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "compt")
	{compton_creator_GAP2=1;}

	if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "phot")
	{photo_creator_GAP2=1;}


         HistoManager::Instance()-> Fill_charge_GAP2 (p_carica_gap2,ID_carica_GAP2,PID_carica_GAP2,PDG_carica_GAP2,Ek_carica_GAP2,x_gap2, y_gap2, z_gap2, eIoni_interaction_GAP2, msc_interaction_GAP2, eBrem_interaction_GAP2, x_gap2_creation, y_gap2_creation, z_gap2_creation, compton_creator_GAP2, photo_creator_GAP2 );  //passa le variabili a ttree p carica tot
	}


//energia SOLO dei fotoni incidenti che poi producono segnale (per sapere energia dei fotoni che producono segnale)


if (p_incidente==1 && p_carica==1 && incident_signal_status==0) {
incident_signal_status=1;
Ek_incident_signal= Ek_TOT;
HistoManager::Instance()->Fill_ttree_incident_signal();		//metodo per copiare variabile Ek_incident_signal in histomanager, e riempire histo
}


// estrazione info per ogni step ma solo nel volume della kodel


if ( edep !=0 &&   (volName =="Honeycomb_skin1_1" || volName =="Honeycomb_core1" || volName =="Honeycomb_skin1_2"		
	|| volName == "PET_insulationTOP"						
	|| volName =="paper"								

	|| volName =="Copper1_1" || volName == "PET1_1" || volName =="PET1_2" || volName == "EVA1_1"|| volName =="graphite1_1"|| volName =="HPL1_1"	
	|| volName =="GasGap1"								
	|| volName =="HPL1_2"|| volName =="graphite1_2"|| volName == "EVA1_2"|| volName == "PET1_3"|| volName =="PET1_4"|| volName =="Copper1_2"	

	|| volName =="PETstrip1"|| volName =="Copper_Strip"|| volName == "PETstrip2"					

	|| volName =="Copper2_1"|| volName == "PET2_1"|| volName =="PET2_2"|| volName == "EVA2_1"|| volName =="graphite2_1"|| volName =="HPL2_1"	
	|| volName =="GasGap2"								
	|| volName =="HPL2_2"|| volName =="graphite2_2"|| volName == "EVA2_2"|| volName == "PET2_3"|| volName =="PET2_4"|| volName =="Copper2_2"	
		
	|| volName =="PET_insulationBOT"							
	|| volName =="Bakelite"								
	|| volName =="Honeycomb_skin2_1"|| volName == "Honeycomb_core2"|| volName =="Honeycomb_skin2_2") )	
	
{

  	G4double z = step->GetTrack()->GetPosition().z() ;					//posizione sulla superficie top sulla quale incidono particelle
  	G4double x = step->GetTrack()->GetPosition().x() ;
  	G4double y = step->GetTrack()->GetPosition().y() ;

	G4double edep_kodel= edep;
	G4double eIon_kodel= edep - step->GetNonIonizingEnergyDeposit() ;
	
	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "compt")
	{compton_kodel=1;} 
	
	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "phot")
	{photo_kodel=1;}
	
	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni")
	{ion_kodel=1;}

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "msc")
	{msc_kodel=1;}

	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eBrem")
	{eBrem_kodel=1;}


        HistoManager::Instance()->Fill_step(x,y,z,edep_kodel,eIon_kodel, photo_kodel, compton_kodel, msc_kodel, ion_kodel, eBrem_kodel );		// passa valori a histo

}


}    // chisura funzioneUserSteppingAction



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// definizione reset che viene chiamata all'inizio di ogni evento, nel file eventaction

void GIFppSteppingAction::Reset()
{
//p incidenti
  p_incidente = 0.;
  p_incidente_top = 0.;
  p_incidente_bot = 0.;

  ID_TOT= 0;
  PID_TOT=0;	
  PDG_TOT=0;	
  Ek_TOT= 0;

  ID_TOP= 0;
  PID_TOP=0;	
  PDG_TOP=0;	
  Ek_TOP= 0;

  z_TOT = 0 ;					
  x_TOT = 0 ;
  y_TOT = 0 ;

  z_TOP = 0 ;					
  x_TOP = 0 ;
  y_TOP = 0 ;

  ID_BOT= 0;
  PID_BOT=0;	
  PDG_BOT=0;	
  Ek_BOT= 0;


  z_BOT = 0 ;					
  x_BOT = 0 ;
  y_BOT = 0 ;

//p cariche
  p_carica = 0.; 
  p_carica_gap1 = 0.; 
  p_carica_gap2 = 0.; 

  ID_carica_TOT = 0 ;		
  PID_carica_TOT = 0;			
  PDG_carica_TOT = 0; 		
  Ek_carica_TOT = 0;

  ID_carica_GAP1 = 0 ;		
  PID_carica_GAP1 = 0;			
  PDG_carica_GAP1 = 0; 		
  Ek_carica_GAP1 = 0;

  ID_carica_GAP2 = 0 ;		
  PID_carica_GAP2 = 0;			
  PDG_carica_GAP2 = 0; 		
  Ek_carica_GAP2 = 0;

  z_gap1 = 0 ;					
  x_gap1 = 0 ;
  y_gap1 = 0 ;

  z_gap2 = 0 ;					
  x_gap2 = 0 ;
  y_gap2 = 0 ;

 msc_interaction_TOT =0;
 eIoni_interaction_TOT=0; 
 eBrem_interaction_TOT=0;	

 msc_interaction_GAP1=0;	
 eIoni_interaction_GAP1=0 ;	
 eBrem_interaction_GAP1=0 ;	
 
 msc_interaction_GAP2=0 ;		
 eIoni_interaction_GAP2=0 ;	
 eBrem_interaction_GAP2=0 ;	
 

// prende informazioni (posizione e processo di creazione) delle particelle che hanno interagito
 z_gapTOT_creation=0 ;			
 x_gapTOT_creation=0 ;
 y_gapTOT_creation=0 ;

 z_gap1_creation=0 ;				
 x_gap1_creation=0 ;
 y_gap1_creation=0 ;

 z_gap2_creation=0 ;				
 x_gap2_creation=0 ;
 y_gap2_creation=0 ;

 compton_creator_TOT=0 ;		
 photo_creator_TOT=0 ;			

 compton_creator_GAP1=0 ;		
 photo_creator_GAP1=0 ;			

 compton_creator_GAP2=0 ;		
 photo_creator_GAP2=0 ;		


//energia SOLO dei fotoni incidenti che poi producono segnale
Ek_incident_signal=0;

}





