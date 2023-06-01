#include "GIF++DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4PVPlacement.hh"

#include "G4SDManager.hh"
#include "G4VisAttributes.hh"

#include "G4GDMLParser.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4GDMLReadStructure.hh"


// Standard includes
#include <list>
#include <string>
#include <string.h>
#include <cmath>
//


// Geant4 includes
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4ProductionCuts.hh"
#include "G4Region.hh"
#include "G4Trd.hh"
#include "G4Box.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4GeometryManager.hh"
#include "G4UnitsTable.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppDetectorConstruction::GIFppDetectorConstruction():
 	G4VUserDetectorConstruction(),
	//fCheckOverlaps(true)
	minorBase(0), majorBase(0), Height(0), Xsource_position(0)			// dimensioni trapezio camera (area effettiva)


{  majorBase=584.1*mm;				//definizione valori dimensioni trapezio camera
   minorBase=866.3*mm;	
   Height  =1613.0*mm;
   Xsource_position= 330.0*mm;
   XJuraWall=4800.0*mm;				// RICORDARSI SPESSORE JURA = 1600mm -> mezzo spessore= 800 mm
   XSaleveWall=-1200.0*mm;			// RICORDARSI SPESSORE SALEVE = 800mm -> mezzo spessore= 400 mm
   ZUpstreamWall=-6800.0*mm;			// RICORDARSI SPESSORE upstream wall = 1600mm -> mezzo spessore= 800 mm

   Xkodel=1045.0*mm;
   Hkodel=27.0*mm;
   Zkodel=-4779.0*mm;
		
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppDetectorConstruction::~GIFppDetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* GIFppDetectorConstruction::Construct()
{ 
	std::map<G4String,G4Colour> mapColors;
	mapColors["White"] = G4Colour::White();
	mapColors["Grey"] = G4Colour::Grey();
	mapColors["Black"] = G4Colour::Black();
	mapColors["Red"] = G4Colour::Red();
	mapColors["Green"] = G4Colour::Green();
	mapColors["Blue"] = G4Colour::Blue();
	mapColors["Cyan"] = G4Colour::Cyan();
	mapColors["Magenta"] = G4Colour::Magenta();
	mapColors["Yellow"] = G4Colour::Yellow();
	mapColors["Purple"] = G4Colour(0.5,0,1);
	mapColors["Orange"] = G4Colour(1,0.5,0);

	G4VisAttributes* worldVisAtt= new G4VisAttributes(false, G4Colour::Red());
	GIFppDataManager* dataManager = GIFppDataManager::GetInstance();


	G4VPhysicalVolume* physWorld;
	G4GDMLParser parser;
	parser.SetOverlapCheck(fCheckOverlaps);
	parser.Read(dataManager->getFilenameGDML(), fCheckOverlaps);

	physWorld = parser.GetWorldVolume(); //world volume
	remove("GIFpp.gdml");
	parser.Write("GIFpp.gdml", physWorld, false);

	physWorld->GetLogicalVolume()->SetVisAttributes(worldVisAtt);  
	const G4LogicalVolumeStore* lvs = G4LogicalVolumeStore::GetInstance();
	std::vector<G4LogicalVolume*>::const_iterator lvciter;

	for( lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++ )
	{
		G4bool isVisible = true;
		G4String strColor="White";
		G4bool foundAuxiliaryInfo = false;
		G4GDMLAuxListType auxInfo = parser.GetVolumeAuxiliaryInformation(*lvciter);
		std::vector<G4GDMLAuxStructType>::const_iterator ipair = auxInfo.begin();
		for( ipair = auxInfo.begin(); ipair != auxInfo.end(); ipair++ )
		{
			foundAuxiliaryInfo = true;
			G4String str=ipair->type;
			G4String val=ipair->value;
			if(str == "Visible")
			{
				if(val == "true")
				{
					isVisible = true;
				}
				else
				{
					isVisible = false;
				}
			}
			if(str == "Color")
			{
				strColor = val;
			}
		}
		if(foundAuxiliaryInfo)
		{
			(*lvciter)->SetVisAttributes(new G4VisAttributes(isVisible, mapColors[strColor]));
		}
		
	}

	// Print materials
	G4cout << *(G4Material::GetMaterialTable()) << G4endl; 

	//



//+++++++++++++++++++++++++++++++++++ RIVELATORE KODEL ++++++++++++++++++++++++++++++++++++++



 //------------------------------------------------------ materials


  G4NistManager* manager = G4NistManager::Instance() ;		//inizializza tabella materiali standard


  // define Elements
   G4Element* elO  = manager->FindOrBuildElement("O");
   G4Element* elC  = manager->FindOrBuildElement("C");		//per: isobutano, etc
   G4Element* elH  = manager->FindOrBuildElement("H");

   G4Element* elF  = manager->FindOrBuildElement("F");		
   G4Element* elS  = manager->FindOrBuildElement("S");



//-------- MATERIALI

// inzializzazione costanti per creare elementi e materiali 

  //G4double a;  // atomic mass
  //G4double z;  // atomic number

   G4int numel(0), natoms(0) ;		//numero elementi, numero atomi
   G4double density(0.), temperature(0.), pressure(0.), fractionMass(0.)  ;
   G4String name, symbol ;


  // materiali da Nist
 
   G4Material *Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al") ;
   G4Material *Cu = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu") ;
   G4Material *Graphite = G4NistManager::Instance()->FindOrBuildMaterial("G4_GRAPHITE") ;


   // gases at STP conditions 

   G4Material* empty = manager->FindOrBuildMaterial("G4_Galactic");
 

//nuovi materiali a partire da NIST
// Aluminum core (per honeycomb core) alluminio con densità inferiore
   density=0.5*g/cm3;
   G4Material* AlCore = G4NistManager::Instance()->BuildMaterialWithNewDensity("AlCore","G4_Al",density);




// materiali composti


   //Air   
   G4Material* Air = new G4Material(name = "Air", density= 1.29*mg/cm3, numel=2);
    Air->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_N"), fractionMass = 70.*perCent);
    Air->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_O"), fractionMass = 30.*perCent);


   //paper (cellulosa)

   density = 0.3*g/cm3;
   G4Material* paper = new G4Material("paper" , density, numel=3);
   paper->AddElement(elC, natoms=6);
   paper->AddElement(elH, natoms=10);
   paper->AddElement(elO, natoms=5);




   //PET

   density = 1.37*g/cm3;
   G4Material* PET = new G4Material("PET" , density, numel=3);
   PET->AddElement(elC, natoms=10);
   PET->AddElement(elH, natoms=8);
   PET->AddElement(elO, natoms=4);



   //High Pressure Laminate HPL   
   G4Material* HPL = new G4Material(name = "HPL", density= 1.4*g/cm3, numel=3);
   HPL->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_H"), fractionMass = 5.74*perCent);
   HPL->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_C"), fractionMass = 77.46*perCent);
   HPL->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_O"), fractionMass = 16.8*perCent);




   //EVA (Etilene vinil acetato)

//-Etilene
   G4Material* etylene = new G4Material(name = "etylene", density= 1.2612*g/cm3, numel=2);
   etylene->AddElement(elC, natoms=2);
   etylene->AddElement(elH, natoms=4);

// vinyl acetato
   G4Material* vinyl_acetato = new G4Material(name = "vinyl_acetato", density= 0.93*g/cm3, numel=3);
   vinyl_acetato->AddElement(elC, natoms=4);
   vinyl_acetato->AddElement(elH, natoms=6);
   vinyl_acetato->AddElement(elO, natoms=2);

//EVA
   G4Material* EVA = new G4Material(name = "EVA", density= 0.934*g/cm3, numel=2);
   EVA->AddMaterial(etylene,fractionMass = 10* perCent);
   EVA->AddMaterial(vinyl_acetato,fractionMass = 90* perCent);




  // RPC mixture gas components

   // iso-Butane (methylpropane), STP
   density = 2.67*mg/cm3;
   G4Material* isobutane = new G4Material(name = "isoC4H10", density, numel=2) ;
   isobutane->AddElement(elC,4);
   isobutane->AddElement(elH,10);

   // Freon
   density = 4.55*mg/cm3;
   G4Material* C2H2F4 = new G4Material(name = "Freon", density, numel=3) ;
   C2H2F4->AddElement(elC, natoms=2);
   C2H2F4->AddElement(elH, natoms=2);
   C2H2F4->AddElement(elF, natoms=4);
 
   // esafluoruro di zolfo SF6
   density = 6.14*mg/cm3;
   G4Material* SF6 = new G4Material(name = "SF6", density, numel=2) ;
   SF6->AddElement(elS, natoms=1);
   SF6->AddElement(elF, natoms=6);

   //miscela RPC
   density = 3.569*mg/cm3;
   G4Material* RPCgas = new G4Material(name = "RPCGas", density, numel=3) ;
   RPCgas->AddMaterial(isobutane, fractionMass = 4.5* perCent) ;
   RPCgas->AddMaterial(C2H2F4, fractionMass = 95.2* perCent) ;
   RPCgas->AddMaterial(SF6, fractionMass = 0.3* perCent) ;




	// Print materials
	G4cout << *(G4Material::GetMaterialTable()) << G4endl; 



//----- VISUALIZZAZIONE ATTRIBUTI (colore layer)

  // Visual attributes
   G4VisAttributes *electrodeAttributes = new G4VisAttributes(G4Color::Grey()) ;
   //cathodeAttributes->SetForceWireframe(true) ;
   G4VisAttributes *gasAttributes = new G4VisAttributes(G4Color::White()) ;
   //g10Attributes->SetForceWireframe(true) ;
   G4VisAttributes *petAttributes = new G4VisAttributes(G4Color::Blue()) ;
   //gasAttributes->SetForceWireframe(true) ;
   G4VisAttributes *evaAttributes = new G4VisAttributes(G4Color::Red()) ;
   //gemAttributes->SetForceWireframe(true) ;
   G4VisAttributes *cuAttributes = new G4VisAttributes(G4Color::Yellow()) ;
  //insAttributes->SetForceWireframe(true) ;
   G4VisAttributes *honeyAttributes = new G4VisAttributes(G4Color::Green()) ;
  //insAttributes->SetForceWireframe(true) ;
   G4VisAttributes *graphiteAttributes = new G4VisAttributes(G4Color::Magenta()) ;
  //insAttributes->SetForceWireframe(true) ;
   G4VisAttributes *fakeAttributes = new G4VisAttributes(G4Color::Cyan()) ;
  //insAttributes->SetForceWireframe(true) ;


  //------------------------------------------------------ volumes

// Rotation Matrix for layers   (ruota elementi disegnati)
G4RotationMatrix* rotationPlacement = new G4RotationMatrix() ;
rotationPlacement->rotateY(M_PI / 2.0) ;		//ruota asse y, per avere x larghezza e z spessore
//rotationPlacement->rotateX(M_PI / 2.0) ;		//rotazione asse x, per avere asse trapezio verticale



/*
//----  WORLD----  commentato perchè gia inserito nella parte di codice di Dorothea

   G4double worldSizeX = 6.*m;				// MEZZE dimensioni world 
   G4double worldSizeY = 6.*m;
   G4double worldSizeZ = 6.*m;

   G4Box* worldBox = new G4Box("WorldBox", worldSizeX, worldSizeY, worldSizeZ) ;			//crea box world
   G4LogicalVolume* worldLog = new G4LogicalVolume(worldBox, Air, "WorldLog") ;		//crea volume logico world

   G4VPhysicalVolume* worldPhys = new G4PVPlacement(0, G4ThreeVector(), worldLog, "WorldSpace", 0, false, fCheckOverlaps) ;	//posiziona volume world
  

 // Set visual attributes
   G4VisAttributes *worldAttributes = new G4VisAttributes ;		//rende non visibile il volume world
   worldAttributes->SetVisibility(false) ;
   worldLog->SetVisAttributes(worldAttributes) ;

*/


//----- VOLUMI TRAPEZI CAMERA (UNO DI SEGUITO ALL'ALTRO),   RE4-2 KODEL DG 1.4 1.4


#define Nstrati 41						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati[Nstrati]= 					//array nome strati
		{  

		"fakeTop",  								//falso layer di aria
		"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2",		//honeycomb panel  TOP
		"PET_insulationTOP",							//pet insulation TOP
		"paper",								//layer paper

		"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1",	//HPL1_1
		"GasGap1",								//GASGAP 1
		"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2",	// HPL1_2

		"PETstrip1","Copper_Strip", "PETstrip2",					//STRIP

		"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1",	//HPL2_1
		"GasGap2",								//GASGAP 2
		"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2",	// HPL2_2
		
		"PET_insulationBOT",							//pet insulation BOT
		"Bakelite",								//bakelite layer
		"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2",		//honeycomb panel BOTTOM
		"fakeBottom",								//falso layer di aria
		
		};



		std::string NomeStratiLog[Nstrati];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati; A++) { 
		    NomeStratiLog[A]=NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati[Nstrati] =				//array materiali 
		{
		
		Air,  								//falso layer di aria
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		Cu, PET, PET, EVA, Graphite, HPL,				//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		RPCgas,								//GASGAP 2
		HPL, Graphite, EVA, PET, PET, Cu,				// HPL2_2
		
		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM
		Air,								//falso layer di aria

		};



		G4double spessoreStrati[Nstrati]=				//array spessori
		{

		0.01*mm,								//falso layer di aria
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 1.35*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		1.4*mm,								//GASGAP 1
		1.35*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					
		
		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 1.35*mm,		//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		1.4*mm,								//GASGAP 2
		1.35*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2"		
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
		0.01*mm,							//falso layer di aria
		
		};

		

		//volumi LOGICI
		
		std::string HPLstring="HPL";					//stringhe per controllo e assegnazione colore a volumi
		std::string Bakelitestring="Bakelite";
		std::string GasGapstring="GasGap";
		std::string PETstring="PET";
		std::string EVAstring="EVA";
		std::string Copperstring="Copper";
		std::string Honeycombstring="Honeycomb";
		std::string Graphitestring="graphite";
		std::string Fakestring="fake";



		for(G4int lyr=0;lyr<Nstrati;lyr++){				//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Trd* strato= new G4Trd(NomeStrati[lyr],spessoreStrati[lyr]/2, spessoreStrati[lyr]/2,minorBase/2,majorBase/2, Height/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato = new G4LogicalVolume(strato, MatStrati[lyr], NomeStratiLog[lyr]) ;


			
			if (NomeStrati[lyr].find(HPLstring)==0 or NomeStrati[lyr].find(Bakelitestring)==0) {  		//ciclo per settare colore volumi
				logicStrato->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));}

			
			else if (NomeStrati[lyr].find(GasGapstring)==0) {
				logicStrato->SetVisAttributes(new G4VisAttributes(*gasAttributes));}


			else if (NomeStrati[lyr].find(PETstring)==0) {
				logicStrato->SetVisAttributes(new G4VisAttributes(*petAttributes));}


			else if (NomeStrati[lyr].find(EVAstring)==0) {
				logicStrato->SetVisAttributes(new G4VisAttributes(*evaAttributes));}


			else if (NomeStrati[lyr].find(Copperstring)==0) {
				logicStrato->SetVisAttributes(new G4VisAttributes(*cuAttributes));}

			
			else if (NomeStrati[lyr].find(Honeycombstring)==0) {
				logicStrato->SetVisAttributes(new G4VisAttributes(*honeyAttributes));}

			
			else if (NomeStrati[lyr].find(Graphitestring)==0) {
				logicStrato->SetVisAttributes(new G4VisAttributes(*graphiteAttributes));}

			
			else if (NomeStrati[lyr].find(Fakestring)==0) {
				logicStrato->SetVisAttributes(new G4VisAttributes(*fakeAttributes));}




			trdCollection.push_back(strato) ;				//mette forma volume nel vettore
		        trdLogCollection.push_back(logicStrato) ;		     	//mette ogni volume logico nel vettore
			}
 



	//VOLUMI FISICI

//-- variabile per shift rispetto a world

G4cout << "----------------------RE4-2 KODEL ------------------------------------" << G4endl ;

G4double ZTranslation=Zkodel;	//posizione di partenza su asse z da dove posizionare primo layer (rispetto a sorgente che è a Z=0)

G4int cpN=1;		// indice numero strato per stampare a video informazioni

//

     for(size_t i=0 ; i<trdCollection.size() ; i++) {   		// i counts as the copyNo per loop

	ZTranslation -= trdCollection.at(i)->GetXHalfLength1() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection.at(i)->GetXHalfLength1()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN << ") " << trdCollection.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The mean Z position is " << G4BestUnit(ZTranslation,"Length") << " And the start Z thickness position is " << G4BestUnit((ZTranslation+(thickness/2)),"Length") << G4endl ;



       G4VPhysicalVolume* trapezoidalPhysic = new G4PVPlacement(		//crea volume fisico					
	  rotationPlacement,							//rotazione
	  G4ThreeVector(Xkodel,Hkodel,ZTranslation),		//vettore posizione
	  trdLogCollection.at(i),						//nome del volume logico
	  trdCollection.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN,									//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       ZTranslation -= trdCollection.at(i)->GetXHalfLength1() ;			// incremento traslazione della seconda metà dello spessore
       cpN++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------------------------------------------------------------" << G4endl ;








//----- TROLLEY 1------------------------------- (la distanza Z che viene printata è la stessa per tutte le camere perche trolley è in verticale, cambia la x)

G4cout << "---------------  TROLLEY 1  ----------------------"<< G4endl ;

G4double X_T1= XJuraWall-(800)-(1120);		// x jura= 4800, meno mezzo spessore del muro 800, meno distanza di S4(prima camera vicina a muro (T1 verticale))
G4double H_T1= 50.0 *mm;	
G4double ZTranslation_T1_S4= ZUpstreamWall+(800)+(2340);	 //posizione upstream -6800, + mezzo spessore upstream=800 + distanza camere da muro (uguale x tutte)

G4double H_T1_basse= -881.85 *mm;				//altezza camere basse s5 e s6



//      ++T1_S4++

std::vector<G4Trd*>           trdCollection_T1_S4 ;			//collezione del nome dei volumi
std::vector<G4LogicalVolume*> trdLogCollection_T1_S4 ;		//collezione dei volumi logici


#define Nstrati_T1_S4 39						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati_T1_S4[Nstrati_T1_S4]= 					//array nome strati
		{  


		"T1_S4_Honeycomb_skin1_1", "T1_S4_Honeycomb_core1","T1_S4_Honeycomb_skin1_2",		//honeycomb panel  TOP
		"T1_S4_PET_insulationTOP",							//pet insulation TOP
		"T1_S4_paper",								//layer paper

		"T1_S4_Copper1_1", "T1_S4_PET1_1","T1_S4_PET1_2", "T1_S4_EVA1_1","T1_S4_graphite1_1","T1_S4_HPL1_1",	//HPL1_1
		"T1_S4_GasGap1",								//GASGAP 1
		"T1_S4_HPL1_2","T1_S4_graphite1_2", "T1_S4_EVA1_2", "T1_S4_PET1_3","T1_S4_PET1_4","T1_S4_Copper1_2",	// HPL1_2

		"T1_S4_PETstrip1","T1_S4_Copper_Strip", "T1_S4_PETstrip2",					//STRIP

		"T1_S4_Copper2_1", "T1_S4_PET2_1","T1_S4_PET2_2", "T1_S4_EVA2_1","T1_S4_graphite2_1","T1_S4_HPL2_1",	//HPL2_1
		"T1_S4_GasGap2",								//GASGAP 2
		"T1_S4_HPL2_2","T1_S4_graphite2_2", "T1_S4_EVA2_2", "T1_S4_PET2_3","T1_S4_PET2_4","T1_S4_Copper2_2",	// HPL2_2
		
		"T1_S4_PET_insulationBOT",							//pet insulation BOT
		"T1_S4_Bakelite",								//bakelite layer
		"T1_S4_Honeycomb_skin2_1", "T1_S4_Honeycomb_core2","T1_S4_Honeycomb_skin2_2",		//honeycomb panel BOTTOM

		
		};



		std::string NomeStratiLog_T1_S4[Nstrati_T1_S4];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati_T1_S4; A++) { 
		    NomeStratiLog[A]="T1_S4_"+NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati_T1_S4[Nstrati_T1_S4] =				//array materiali 
		{
		
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		Cu, PET, PET, EVA, Graphite, HPL,				//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		RPCgas,								//GASGAP 2
		HPL, Graphite, EVA, PET, PET, Cu,				// HPL2_2
		
		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM

		};



		G4double spessoreStrati_T1_S4[Nstrati_T1_S4]=				//array spessori
		{

	
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		2.0*mm,								//GASGAP 1
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					
		
		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,		//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		2.0*mm,								//GASGAP 2
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2"		
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
	
		
		};

		

		//volumi LOGICI
	


		for(G4int lyr_T1_S4=0;lyr_T1_S4<Nstrati_T1_S4;lyr_T1_S4++){			//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Trd* strato_T1_S4= new G4Trd(NomeStrati_T1_S4[lyr_T1_S4],spessoreStrati_T1_S4[lyr_T1_S4]/2, spessoreStrati_T1_S4[lyr_T1_S4]/2,minorBase/2,majorBase/2, Height/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato_T1_S4 = new G4LogicalVolume(strato_T1_S4, MatStrati_T1_S4[lyr_T1_S4], NomeStratiLog_T1_S4[lyr_T1_S4]) ;

			
			logicStrato_T1_S4->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


			trdCollection_T1_S4.push_back(strato_T1_S4) ;				//mette forma volume nel vettore
		        trdLogCollection_T1_S4.push_back(logicStrato_T1_S4) ;		     	//mette ogni volume logico nel vettore
			}
 



	//VOLUMI FISICI


G4int cpN_T1_S4=1;		// indice numero strato per stampare a video informazioni

//

	G4cout << " Detector T1_S4"<< G4endl ;
     for(size_t i=0 ; i<trdCollection_T1_S4.size() ; i++) {   		// i counts as the copyNo per loop

	X_T1 -= trdCollection_T1_S4.at(i)->GetXHalfLength1() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)
	G4double thickness=trdCollection_T1_S4.at(i)->GetXHalfLength1()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN_T1_S4 << ") " << trdCollection_T1_S4.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The position is " << G4BestUnit(ZTranslation_T1_S4,"Length") << G4endl ;

       G4VPhysicalVolume* trapezoidalPhysic_T1_S4 = new G4PVPlacement(		//crea volume fisico					
	  new G4RotationMatrix(),							//rotazione
	  G4ThreeVector(X_T1,H_T1,ZTranslation_T1_S4),				//vettore posizione
	  trdLogCollection_T1_S4.at(i),						//nome del volume logico
	  trdCollection_T1_S4.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN_T1_S4,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       X_T1 -= trdCollection_T1_S4.at(i)->GetXHalfLength1() ;			// incremento traslazione della seconda metà dello spessore
       cpN_T1_S4++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------Added T1_S4  ------------------------------" << G4endl ;




//      ++T1_S3++

std::vector<G4Trd*>           trdCollection_T1_S3 ;			//collezione del nome dei volumi
std::vector<G4LogicalVolume*> trdLogCollection_T1_S3 ;		//collezione dei volumi logici


#define Nstrati_T1_S3 39						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati_T1_S3[Nstrati_T1_S3]= 					//array nome strati
		{  


		"T1_S3_Honeycomb_skin1_1", "T1_S3_Honeycomb_core1","T1_S3_Honeycomb_skin1_2",		//honeycomb panel  TOP
		"T1_S3_PET_insulationTOP",							//pet insulation TOP
		"T1_S3_paper",								//layer paper

		"T1_S3_Copper1_1", "T1_S3_PET1_1","T1_S3_PET1_2", "T1_S3_EVA1_1","T1_S3_graphite1_1","T1_S3_HPL1_1",	//HPL1_1
		"T1_S3_GasGap1",								//GASGAP 1
		"T1_S3_HPL1_2","T1_S3_graphite1_2", "T1_S3_EVA1_2", "T1_S3_PET1_3","T1_S3_PET1_4","T1_S3_Copper1_2",	// HPL1_2

		"T1_S3_PETstrip1","T1_S3_Copper_Strip", "T1_S3_PETstrip2",					//STRIP

		"T1_S3_Copper2_1", "T1_S3_PET2_1","T1_S3_PET2_2", "T1_S3_EVA2_1","T1_S3_graphite2_1","T1_S3_HPL2_1",	//HPL2_1
		"T1_S3_GasGap2",								//GASGAP 2
		"T1_S3_HPL2_2","T1_S3_graphite2_2", "T1_S3_EVA2_2", "T1_S3_PET2_3","T1_S3_PET2_4","T1_S3_Copper2_2",	// HPL2_2
		
		"T1_S3_PET_insulationBOT",							//pet insulation BOT
		"T1_S3_Bakelite",								//bakelite layer
		"T1_S3_Honeycomb_skin2_1", "T1_S3_Honeycomb_core2","T1_S3_Honeycomb_skin2_2",		//honeycomb panel BOTTOM

		
		};



		std::string NomeStratiLog_T1_S3[Nstrati_T1_S3];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati_T1_S3; A++) { 
		    NomeStratiLog[A]="T1_S3_"+NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati_T1_S3[Nstrati_T1_S3] =				//array materiali 
		{
		
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		Cu, PET, PET, EVA, Graphite, HPL,				//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		RPCgas,								//GASGAP 2
		HPL, Graphite, EVA, PET, PET, Cu,				// HPL2_2
		
		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM

		};



		G4double spessoreStrati_T1_S3[Nstrati_T1_S3]=				//array spessori
		{

	
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		2.0*mm,								//GASGAP 1
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					
		
		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,		//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		2.0*mm,								//GASGAP 2
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2"		
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
	
		
		};

		

		//volumi LOGICI
	


		for(G4int lyr_T1_S3=0;lyr_T1_S3<Nstrati_T1_S3;lyr_T1_S3++){			//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Trd* strato_T1_S3= new G4Trd(NomeStrati_T1_S3[lyr_T1_S3],spessoreStrati_T1_S3[lyr_T1_S3]/2, spessoreStrati_T1_S3[lyr_T1_S3]/2,minorBase/2,majorBase/2, Height/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato_T1_S3 = new G4LogicalVolume(strato_T1_S3, MatStrati_T1_S3[lyr_T1_S3], NomeStratiLog_T1_S3[lyr_T1_S3]) ;

			
			logicStrato_T1_S3->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


			trdCollection_T1_S3.push_back(strato_T1_S3) ;				//mette forma volume nel vettore
		        trdLogCollection_T1_S3.push_back(logicStrato_T1_S3) ;		     	//mette ogni volume logico nel vettore
			}
 



	//VOLUMI FISICI

G4double XTranslation_T1_S3= X_T1 -100;	// 100mm di distanza rispetto alla camera precedente

G4int cpN_T1_S3=1;		// indice numero strato per stampare a video informazioni

//

	G4cout << " Detector T1_S3"<< G4endl ;
     for(size_t i=0 ; i<trdCollection_T1_S3.size() ; i++) {   		// i counts as the copyNo per loop

	XTranslation_T1_S3 -= trdCollection_T1_S3.at(i)->GetXHalfLength1() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection_T1_S3.at(i)->GetXHalfLength1()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN_T1_S3 << ") " << trdCollection_T1_S3.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The position is " << G4BestUnit(ZTranslation_T1_S4,"Length") << G4endl ;

       G4VPhysicalVolume* trapezoidalPhysic_T1_S3 = new G4PVPlacement(		//crea volume fisico					
	  new G4RotationMatrix(),							//rotazione
	  G4ThreeVector(XTranslation_T1_S3,H_T1,ZTranslation_T1_S4),				//vettore posizione
	  trdLogCollection_T1_S3.at(i),						//nome del volume logico
	  trdCollection_T1_S3.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN_T1_S3,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       XTranslation_T1_S3 -= trdCollection_T1_S3.at(i)->GetXHalfLength1() ;			// incremento traslazione della seconda metà dello spessore
       cpN_T1_S3++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------Added T1_S3  ------------------------------" << G4endl ;






//      ++T1_S2++

std::vector<G4Trd*>           trdCollection_T1_S2 ;			//collezione del nome dei volumi
std::vector<G4LogicalVolume*> trdLogCollection_T1_S2 ;		//collezione dei volumi logici


#define Nstrati_T1_S2 39						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati_T1_S2[Nstrati_T1_S2]= 					//array nome strati
		{  


		"T1_S2_Honeycomb_skin1_1", "T1_S2_Honeycomb_core1","T1_S2_Honeycomb_skin1_2",		//honeycomb panel  TOP
		"T1_S2_PET_insulationTOP",							//pet insulation TOP
		"T1_S2_paper",								//layer paper

		"T1_S2_Copper1_1", "T1_S2_PET1_1","T1_S2_PET1_2", "T1_S2_EVA1_1","T1_S2_graphite1_1","T1_S2_HPL1_1",	//HPL1_1
		"T1_S2_GasGap1",								//GASGAP 1
		"T1_S2_HPL1_2","T1_S2_graphite1_2", "T1_S2_EVA1_2", "T1_S2_PET1_3","T1_S2_PET1_4","T1_S2_Copper1_2",	// HPL1_2

		"T1_S2_PETstrip1","T1_S2_Copper_Strip", "T1_S2_PETstrip2",					//STRIP

		"T1_S2_Copper2_1", "T1_S2_PET2_1","T1_S2_PET2_2", "T1_S2_EVA2_1","T1_S2_graphite2_1","T1_S2_HPL2_1",	//HPL2_1
		"T1_S2_GasGap2",								//GASGAP 2
		"T1_S2_HPL2_2","T1_S2_graphite2_2", "T1_S2_EVA2_2", "T1_S2_PET2_3","T1_S2_PET2_4","T1_S2_Copper2_2",	// HPL2_2
		
		"T1_S2_PET_insulationBOT",							//pet insulation BOT
		"T1_S2_Bakelite",								//bakelite layer
		"T1_S2_Honeycomb_skin2_1", "T1_S2_Honeycomb_core2","T1_S2_Honeycomb_skin2_2",		//honeycomb panel BOTTOM

		
		};



		std::string NomeStratiLog_T1_S2[Nstrati_T1_S2];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati_T1_S2; A++) { 
		    NomeStratiLog[A]="T1_S2_"+NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati_T1_S2[Nstrati_T1_S2] =				//array materiali 
		{
		
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		Cu, PET, PET, EVA, Graphite, HPL,				//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		RPCgas,								//GASGAP 2
		HPL, Graphite, EVA, PET, PET, Cu,				// HPL2_2
		
		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM

		};



		G4double spessoreStrati_T1_S2[Nstrati_T1_S2]=				//array spessori
		{

	
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		2.0*mm,								//GASGAP 1
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					
		
		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,		//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		2.0*mm,								//GASGAP 2
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2"		
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
	
		
		};

		

		//volumi LOGICI
	


		for(G4int lyr_T1_S2=0;lyr_T1_S2<Nstrati_T1_S2;lyr_T1_S2++){			//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Trd* strato_T1_S2= new G4Trd(NomeStrati_T1_S2[lyr_T1_S2],spessoreStrati_T1_S2[lyr_T1_S2]/2, spessoreStrati_T1_S2[lyr_T1_S2]/2,minorBase/2,majorBase/2, Height/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato_T1_S2 = new G4LogicalVolume(strato_T1_S2, MatStrati_T1_S2[lyr_T1_S2], NomeStratiLog_T1_S2[lyr_T1_S2]) ;

			
			logicStrato_T1_S2->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


			trdCollection_T1_S2.push_back(strato_T1_S2) ;				//mette forma volume nel vettore
		        trdLogCollection_T1_S2.push_back(logicStrato_T1_S2) ;		     	//mette ogni volume logico nel vettore
			}
 



	//VOLUMI FISICI

G4double XTranslation_T1_S2= XTranslation_T1_S3 -100;	// 100mm di distanza rispetto alla camera precedente

G4int cpN_T1_S2=1;		// indice numero strato per stampare a video informazioni

//

	G4cout << " Detector T1_S2"<< G4endl ;
     for(size_t i=0 ; i<trdCollection_T1_S2.size() ; i++) {   		// i counts as the copyNo per loop

	XTranslation_T1_S2 -= trdCollection_T1_S2.at(i)->GetXHalfLength1() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection_T1_S2.at(i)->GetXHalfLength1()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN_T1_S2 << ") " << trdCollection_T1_S2.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The position is " << G4BestUnit(ZTranslation_T1_S4,"Length") << G4endl ;

       G4VPhysicalVolume* trapezoidalPhysic_T1_S2 = new G4PVPlacement(		//crea volume fisico					
	  new G4RotationMatrix(),							//rotazione
	  G4ThreeVector(XTranslation_T1_S2,H_T1,ZTranslation_T1_S4),				//vettore posizione
	  trdLogCollection_T1_S2.at(i),						//nome del volume logico
	  trdCollection_T1_S2.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN_T1_S2,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       XTranslation_T1_S2 -= trdCollection_T1_S2.at(i)->GetXHalfLength1() ;			// incremento traslazione della seconda metà dello spessore
       cpN_T1_S2++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------Added T1_S2  ------------------------------" << G4endl ;







//      ++T1_S1++

std::vector<G4Trd*>           trdCollection_T1_S1 ;			//collezione del nome dei volumi
std::vector<G4LogicalVolume*> trdLogCollection_T1_S1 ;		//collezione dei volumi logici


#define Nstrati_T1_S1 39						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati_T1_S1[Nstrati_T1_S1]= 					//array nome strati
		{  


		"T1_S1_Honeycomb_skin1_1", "T1_S1_Honeycomb_core1","T1_S1_Honeycomb_skin1_2",		//honeycomb panel  TOP
		"T1_S1_PET_insulationTOP",							//pet insulation TOP
		"T1_S1_paper",								//layer paper

		"T1_S1_Copper1_1", "T1_S1_PET1_1","T1_S1_PET1_2", "T1_S1_EVA1_1","T1_S1_graphite1_1","T1_S1_HPL1_1",	//HPL1_1
		"T1_S1_GasGap1",								//GASGAP 1
		"T1_S1_HPL1_2","T1_S1_graphite1_2", "T1_S1_EVA1_2", "T1_S1_PET1_3","T1_S1_PET1_4","T1_S1_Copper1_2",	// HPL1_2

		"T1_S1_PETstrip1","T1_S1_Copper_Strip", "T1_S1_PETstrip2",					//STRIP

		"T1_S1_Copper2_1", "T1_S1_PET2_1","T1_S1_PET2_2", "T1_S1_EVA2_1","T1_S1_graphite2_1","T1_S1_HPL2_1",	//HPL2_1
		"T1_S1_GasGap2",								//GASGAP 2
		"T1_S1_HPL2_2","T1_S1_graphite2_2", "T1_S1_EVA2_2", "T1_S1_PET2_3","T1_S1_PET2_4","T1_S1_Copper2_2",	// HPL2_2
		
		"T1_S1_PET_insulationBOT",							//pet insulation BOT
		"T1_S1_Bakelite",								//bakelite layer
		"T1_S1_Honeycomb_skin2_1", "T1_S1_Honeycomb_core2","T1_S1_Honeycomb_skin2_2",		//honeycomb panel BOTTOM

		
		};



		std::string NomeStratiLog_T1_S1[Nstrati_T1_S1];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati_T1_S1; A++) { 
		    NomeStratiLog[A]="T1_S1_"+NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati_T1_S1[Nstrati_T1_S1] =				//array materiali 
		{
		
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		Cu, PET, PET, EVA, Graphite, HPL,				//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		RPCgas,								//GASGAP 2
		HPL, Graphite, EVA, PET, PET, Cu,				// HPL2_2
		
		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM

		};



		G4double spessoreStrati_T1_S1[Nstrati_T1_S1]=				//array spessori
		{

	
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		2.0*mm,								//GASGAP 1
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					



		
		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,		//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		2.0*mm,								//GASGAP 2
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2"		
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
	
		
		};

		

		//volumi LOGICI
	


		for(G4int lyr_T1_S1=0;lyr_T1_S1<Nstrati_T1_S1;lyr_T1_S1++){			//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Trd* strato_T1_S1= new G4Trd(NomeStrati_T1_S1[lyr_T1_S1],spessoreStrati_T1_S1[lyr_T1_S1]/2, spessoreStrati_T1_S1[lyr_T1_S1]/2,minorBase/2,majorBase/2, Height/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato_T1_S1 = new G4LogicalVolume(strato_T1_S1, MatStrati_T1_S1[lyr_T1_S1], NomeStratiLog_T1_S1[lyr_T1_S1]) ;

			
			logicStrato_T1_S1->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


			trdCollection_T1_S1.push_back(strato_T1_S1) ;				//mette forma volume nel vettore
		        trdLogCollection_T1_S1.push_back(logicStrato_T1_S1) ;		     	//mette ogni volume logico nel vettore
			}
 



	//VOLUMI FISICI

G4double XTranslation_T1_S1= XTranslation_T1_S2 -100;	// 100mm di distanza rispetto alla camera precedente

G4int cpN_T1_S1=1;		// indice numero strato per stampare a video informazioni

//

	G4cout << " Detector T1_S1"<< G4endl ;
     for(size_t i=0 ; i<trdCollection_T1_S1.size() ; i++) {   		// i counts as the copyNo per loop

	XTranslation_T1_S1 -= trdCollection_T1_S1.at(i)->GetXHalfLength1() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection_T1_S1.at(i)->GetXHalfLength1()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN_T1_S1 << ") " << trdCollection_T1_S1.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The position is " << G4BestUnit(ZTranslation_T1_S4,"Length") << G4endl ;

       G4VPhysicalVolume* trapezoidalPhysic_T1_S1 = new G4PVPlacement(		//crea volume fisico					
	  new G4RotationMatrix(),							//rotazione
	  G4ThreeVector(XTranslation_T1_S1,H_T1,ZTranslation_T1_S4),				//vettore posizione
	  trdLogCollection_T1_S1.at(i),						//nome del volume logico
	  trdCollection_T1_S1.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN_T1_S1,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       XTranslation_T1_S1 -= trdCollection_T1_S1.at(i)->GetXHalfLength1() ;			// incremento traslazione della seconda metà dello spessore
       cpN_T1_S1++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------Added T1_S1  ------------------------------" << G4endl ;






//      ++T1_S6++

std::vector<G4Trd*>           trdCollection_T1_S6 ;			//collezione del nome dei volumi
std::vector<G4LogicalVolume*> trdLogCollection_T1_S6 ;		//collezione dei volumi logici


#define Nstrati_T1_S6 39						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati_T1_S6[Nstrati_T1_S6]= 					//array nome strati
		{  


		"T1_S6_Honeycomb_skin1_1", "T1_S6_Honeycomb_core1","T1_S6_Honeycomb_skin1_2",		//honeycomb panel  TOP
		"T1_S6_PET_insulationTOP",							//pet insulation TOP
		"T1_S6_paper",								//layer paper

		"T1_S6_Copper1_1", "T1_S6_PET1_1","T1_S6_PET1_2", "T1_S6_EVA1_1","T1_S6_graphite1_1","T1_S6_HPL1_1",	//HPL1_1
		"T1_S6_GasGap1",								//GASGAP 1
		"T1_S6_HPL1_2","T1_S6_graphite1_2", "T1_S6_EVA1_2", "T1_S6_PET1_3","T1_S6_PET1_4","T1_S6_Copper1_2",	// HPL1_2

		"T1_S6_PETstrip1","T1_S6_Copper_Strip", "T1_S6_PETstrip2",					//STRIP

		"T1_S6_Copper2_1", "T1_S6_PET2_1","T1_S6_PET2_2", "T1_S6_EVA2_1","T1_S6_graphite2_1","T1_S6_HPL2_1",	//HPL2_1
		"T1_S6_GasGap2",								//GASGAP 2
		"T1_S6_HPL2_2","T1_S6_graphite2_2", "T1_S6_EVA2_2", "T1_S6_PET2_3","T1_S6_PET2_4","T1_S6_Copper2_2",	// HPL2_2
		
		"T1_S6_PET_insulationBOT",							//pet insulation BOT
		"T1_S6_Bakelite",								//bakelite layer
		"T1_S6_Honeycomb_skin2_1", "T1_S6_Honeycomb_core2","T1_S6_Honeycomb_skin2_2",		//honeycomb panel BOTTOM

		
		};



		std::string NomeStratiLog_T1_S6[Nstrati_T1_S6];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati_T1_S6; A++) { 
		    NomeStratiLog[A]="T1_S6_"+NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati_T1_S6[Nstrati_T1_S6] =				//array materiali 
		{
		
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		Cu, PET, PET, EVA, Graphite, HPL,				//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		RPCgas,								//GASGAP 2
		HPL, Graphite, EVA, PET, PET, Cu,				// HPL2_2
		
		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM

		};



		G4double spessoreStrati_T1_S6[Nstrati_T1_S6]=				//array spessori
		{

	
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		2.0*mm,								//GASGAP 1
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					
		
		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,		//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		2.0*mm,								//GASGAP 2
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2"		
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
	
		
		};

		

		//volumi LOGICI
	


		for(G4int lyr_T1_S6=0;lyr_T1_S6<Nstrati_T1_S6;lyr_T1_S6++){			//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Trd* strato_T1_S6= new G4Trd(NomeStrati_T1_S6[lyr_T1_S6],spessoreStrati_T1_S6[lyr_T1_S6]/2, spessoreStrati_T1_S6[lyr_T1_S6]/2,minorBase/2,majorBase/2, Height/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato_T1_S6 = new G4LogicalVolume(strato_T1_S6, MatStrati_T1_S6[lyr_T1_S6], NomeStratiLog_T1_S6[lyr_T1_S6]) ;

			
			logicStrato_T1_S6->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


			trdCollection_T1_S6.push_back(strato_T1_S6) ;				//mette forma volume nel vettore
		        trdLogCollection_T1_S6.push_back(logicStrato_T1_S6) ;		     	//mette ogni volume logico nel vettore
			}
 



	//VOLUMI FISICI

G4double XTranslation_T1_S6= X_T1 +100;	// 100mm di distanza tra s4(prima camera vicina a muro) e s6 (s6 piu distante)

G4int cpN_T1_S6=1;		// indice numero strato per stampare a video informazioni

//

	G4cout << " Detector T1_S6"<< G4endl ;
     for(size_t i=0 ; i<trdCollection_T1_S6.size() ; i++) {   		// i counts as the copyNo per loop

	XTranslation_T1_S6 -= trdCollection_T1_S6.at(i)->GetXHalfLength1() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection_T1_S6.at(i)->GetXHalfLength1()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN_T1_S6 << ") " << trdCollection_T1_S6.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The position is " << G4BestUnit(XTranslation_T1_S6,"Length") << G4endl ;

       G4VPhysicalVolume* trapezoidalPhysic_T1_S6 = new G4PVPlacement(		//crea volume fisico					
	  new G4RotationMatrix(),							//rotazione
	  G4ThreeVector(XTranslation_T1_S6,H_T1_basse,ZTranslation_T1_S4),				//vettore posizione
	  trdLogCollection_T1_S6.at(i),						//nome del volume logico
	  trdCollection_T1_S6.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN_T1_S6,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       XTranslation_T1_S6 -= trdCollection_T1_S6.at(i)->GetXHalfLength1() ;			// incremento traslazione della seconda metà dello spessore
       cpN_T1_S6++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------Added T1_S6  ------------------------------" << G4endl ;





//      ++T1_S5++

std::vector<G4Trd*>           trdCollection_T1_S5 ;			//collezione del nome dei volumi
std::vector<G4LogicalVolume*> trdLogCollection_T1_S5 ;		//collezione dei volumi logici


#define Nstrati_T1_S5 39						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati_T1_S5[Nstrati_T1_S5]= 					//array nome strati
		{  


		"T1_S5_Honeycomb_skin1_1", "T1_S5_Honeycomb_core1","T1_S5_Honeycomb_skin1_2",		//honeycomb panel  TOP
		"T1_S5_PET_insulationTOP",							//pet insulation TOP
		"T1_S5_paper",								//layer paper

		"T1_S5_Copper1_1", "T1_S5_PET1_1","T1_S5_PET1_2", "T1_S5_EVA1_1","T1_S5_graphite1_1","T1_S5_HPL1_1",	//HPL1_1
		"T1_S5_GasGap1",								//GASGAP 1
		"T1_S5_HPL1_2","T1_S5_graphite1_2", "T1_S5_EVA1_2", "T1_S5_PET1_3","T1_S5_PET1_4","T1_S5_Copper1_2",	// HPL1_2

		"T1_S5_PETstrip1","T1_S5_Copper_Strip", "T1_S5_PETstrip2",					//STRIP

		"T1_S5_Copper2_1", "T1_S5_PET2_1","T1_S5_PET2_2", "T1_S5_EVA2_1","T1_S5_graphite2_1","T1_S5_HPL2_1",	//HPL2_1
		"T1_S5_GasGap2",								//GASGAP 2
		"T1_S5_HPL2_2","T1_S5_graphite2_2", "T1_S5_EVA2_2", "T1_S5_PET2_3","T1_S5_PET2_4","T1_S5_Copper2_2",	// HPL2_2
		
		"T1_S5_PET_insulationBOT",							//pet insulation BOT
		"T1_S5_Bakelite",								//bakelite layer
		"T1_S5_Honeycomb_skin2_1", "T1_S5_Honeycomb_core2","T1_S5_Honeycomb_skin2_2",		//honeycomb panel BOTTOM

		
		};



		std::string NomeStratiLog_T1_S5[Nstrati_T1_S5];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati_T1_S5; A++) { 
		    NomeStratiLog[A]="T1_S5_"+NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati_T1_S5[Nstrati_T1_S5] =				//array materiali 
		{
		
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		Cu, PET, PET, EVA, Graphite, HPL,				//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		RPCgas,								//GASGAP 2
		HPL, Graphite, EVA, PET, PET, Cu,				// HPL2_2
		
		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM

		};



		G4double spessoreStrati_T1_S5[Nstrati_T1_S5]=				//array spessori
		{

	
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		2.0*mm,								//GASGAP 1
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					
		
		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,		//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		2.0*mm,								//GASGAP 2
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2"		
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
	
		
		};

		

		//volumi LOGICI
	


		for(G4int lyr_T1_S5=0;lyr_T1_S5<Nstrati_T1_S5;lyr_T1_S5++){			//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Trd* strato_T1_S5= new G4Trd(NomeStrati_T1_S5[lyr_T1_S5],spessoreStrati_T1_S5[lyr_T1_S5]/2, spessoreStrati_T1_S5[lyr_T1_S5]/2, minorBase/2,majorBase/2, Height/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato_T1_S5 = new G4LogicalVolume(strato_T1_S5, MatStrati_T1_S5[lyr_T1_S5], NomeStratiLog_T1_S5[lyr_T1_S5]) ;

			
			logicStrato_T1_S5->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


			trdCollection_T1_S5.push_back(strato_T1_S5) ;				//mette forma volume nel vettore
		        trdLogCollection_T1_S5.push_back(logicStrato_T1_S5) ;		     	//mette ogni volume logico nel vettore
			}
 



	//VOLUMI FISICI

G4double XTranslation_T1_S5= XTranslation_T1_S6 -500;	// DISTANZA TRA LE 2 CAMERE BASSE

G4int cpN_T1_S5=1;		// indice numero strato per stampare a video informazioni

//

	G4cout << " Detector T1_S5"<< G4endl ;
     for(size_t i=0 ; i<trdCollection_T1_S5.size() ; i++) {   		// i counts as the copyNo per loop

	XTranslation_T1_S5 -= trdCollection_T1_S5.at(i)->GetXHalfLength1() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection_T1_S5.at(i)->GetXHalfLength1()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN_T1_S5 << ") " << trdCollection_T1_S5.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The position is " << G4BestUnit(XTranslation_T1_S5,"Length") << G4endl ;

       G4VPhysicalVolume* trapezoidalPhysic_T1_S5 = new G4PVPlacement(		//crea volume fisico					
	  new G4RotationMatrix(),							//rotazione
	  G4ThreeVector(XTranslation_T1_S5,H_T1_basse,ZTranslation_T1_S4),				//vettore posizione
	  trdLogCollection_T1_S5.at(i),						//nome del volume logico
	  trdCollection_T1_S5.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN_T1_S5,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       XTranslation_T1_S5 -= trdCollection_T1_S5.at(i)->GetXHalfLength1() ;			// incremento traslazione della seconda metà dello spessore
       cpN_T1_S5++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------Added T1_S5  ------------------------------" << G4endl ;




// PIANO CARRELLO T1


G4VSolid* trolley1= new G4Box("trolley1",934/2, 15/2, 2025/2 ); 		// larghezza, spessore, lunghezza
G4LogicalVolume* logic_trolley = new G4LogicalVolume(trolley1, Al , "trolley1") ;		

			
logic_trolley ->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


G4VPhysicalVolume* physic_trolley = new G4PVPlacement(		//crea volume fisico					
	  new G4RotationMatrix(),							//rotazione
	  G4ThreeVector(XTranslation_T1_S2,-1500*mm,ZTranslation_T1_S4),			//vettore posizione  (altezza -1590 del pavimento + 275 altezza carrello)
	  logic_trolley,						//nome del volume logico
	  "carrello_physic",					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  1,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)


G4cout << "-------------------Added TROLLEY 1  ------------------------------" << G4endl ;





//   GT CHAMBERS


//      ++GT1++

std::vector<G4Box*>           trdCollection_GT1 ;			//collezione del nome dei volumi
std::vector<G4LogicalVolume*> trdLogCollection_GT1 ;		//collezione dei volumi logici


#define Nstrati_GT1 39						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati_GT1[Nstrati_GT1]= 					//array nome strati
		{  


		"GT1_Honeycomb_skin1_1", "GT1_Honeycomb_core1","GT1_Honeycomb_skin1_2",		//honeycomb panel  TOP
		"GT1_PET_insulationTOP",							//pet insulation TOP
		"GT1_paper",								//layer paper

		"GT1_Copper1_1", "GT1_PET1_1","GT1_PET1_2", "GT1_EVA1_1","GT1_graphite1_1","GT1_HPL1_1",	//HPL1_1
		"GT1_GasGap1",								//GASGAP 1
		"GT1_HPL1_2","GT1_graphite1_2", "GT1_EVA1_2", "GT1_PET1_3","GT1_PET1_4","GT1_Copper1_2",	// HPL1_2

		"GT1_PETstrip1","GT1_Copper_Strip", "GT1_PETstrip2",					//STRIP

		"GT1_Copper2_1", "GT1_PET2_1","GT1_PET2_2", "GT1_EVA2_1","GT1_graphite2_1","GT1_HPL2_1",	//HPL2_1
		"GT1_GasGap2",								//GASGAP 2
		"GT1_HPL2_2","GT1_graphite2_2", "GT1_EVA2_2", "GT1_PET2_3","GT1_PET2_4","GT1_Copper2_2",	// HPL2_2
		
		"GT1_PET_insulationBOT",							//pet insulation BOT
		"GT1_Bakelite",								//bakelite layer
		"GT1_Honeycomb_skin2_1", "GT1_Honeycomb_core2","GT1_Honeycomb_skin2_2",		//honeycomb panel BOTTOM

		
		};



		std::string NomeStratiLog_GT1[Nstrati_GT1];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati_GT1; A++) { 
		    NomeStratiLog[A]="GT1_"+NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati_GT1[Nstrati_GT1] =				//array materiali 
		{
		
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		Cu, PET, PET, EVA, Graphite, HPL,				//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		RPCgas,								//GASGAP 2
		HPL, Graphite, EVA, PET, PET, Cu,				// HPL2_2
		
		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM

		};



		G4double spessoreStrati_GT1[Nstrati_GT1]=				//array spessori
		{

	
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		2.0*mm,								//GASGAP 1
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					
		
		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,		//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		2.0*mm,								//GASGAP 2
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2"		
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
	
		
		};

		

		//volumi LOGICI
	


		for(G4int lyr_GT1=0;lyr_GT1<Nstrati_GT1;lyr_GT1++){			//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Box* strato_GT1= new G4Box(NomeStrati_GT1[lyr_GT1],spessoreStrati_GT1[lyr_GT1]/2, 500/2, 700/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato_GT1 = new G4LogicalVolume(strato_GT1, MatStrati_GT1[lyr_GT1], NomeStratiLog_GT1[lyr_GT1]) ;

			
			logicStrato_GT1->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


			trdCollection_GT1.push_back(strato_GT1) ;				//mette forma volume nel vettore
		        trdLogCollection_GT1.push_back(logicStrato_GT1) ;		     	//mette ogni volume logico nel vettore
			}
 


	//VOLUMI FISICI

G4double ZTranslation_GT1= Zkodel-150;	// 150mm di distanza tra kodel e gt1

G4int cpN_GT1=1;		// indice numero strato per stampare a video informazioni

//

	G4cout << " Detector GT1"<< G4endl ;
     for(size_t i=0 ; i<trdCollection_GT1.size() ; i++) {   		// i counts as the copyNo per loop

	ZTranslation_GT1 -= trdCollection_GT1.at(i)->GetXHalfLength() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection_GT1.at(i)->GetXHalfLength()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN_GT1 << ") " << trdCollection_GT1.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The position is " << G4BestUnit(ZTranslation_GT1,"Length") << G4endl ;

       G4VPhysicalVolume* trapezoidalPhysic_GT1 = new G4PVPlacement(		//crea volume fisico					
	  rotationPlacement,							//rotazione
	  G4ThreeVector(Xkodel,Hkodel,ZTranslation_GT1),				//vettore posizione
	  trdLogCollection_GT1.at(i),						//nome del volume logico
	  trdCollection_GT1.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN_GT1,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       ZTranslation_GT1 -= trdCollection_GT1.at(i)->GetXHalfLength() ;			// incremento traslazione della seconda metà dello spessore
       cpN_GT1++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------Added GT1  ------------------------------" << G4endl ;



//      ++GT2++

std::vector<G4Box*>           trdCollection_GT2 ;			//collezione del nome dei volumi
std::vector<G4LogicalVolume*> trdLogCollection_GT2 ;		//collezione dei volumi logici


#define Nstrati_GT2 39						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati_GT2[Nstrati_GT2]= 					//array nome strati
		{  


		"GT2_Honeycomb_skin1_1", "GT2_Honeycomb_core1","GT2_Honeycomb_skin1_2",		//honeycomb panel  TOP
		"GT2_PET_insulationTOP",							//pet insulation TOP
		"GT2_paper",								//layer paper

		"GT2_Copper1_1", "GT2_PET1_1","GT2_PET1_2", "GT2_EVA1_1","GT2_graphite1_1","GT2_HPL1_1",	//HPL1_1
		"GT2_GasGap1",								//GASGAP 1
		"GT2_HPL1_2","GT2_graphite1_2", "GT2_EVA1_2", "GT2_PET1_3","GT2_PET1_4","GT2_Copper1_2",	// HPL1_2

		"GT2_PETstrip1","GT2_Copper_Strip", "GT2_PETstrip2",					//STRIP

		"GT2_Copper2_1", "GT2_PET2_1","GT2_PET2_2", "GT2_EVA2_1","GT2_graphite2_1","GT2_HPL2_1",	//HPL2_1
		"GT2_GasGap2",								//GASGAP 2
		"GT2_HPL2_2","GT2_graphite2_2", "GT2_EVA2_2", "GT2_PET2_3","GT2_PET2_4","GT2_Copper2_2",	// HPL2_2
		
		"GT2_PET_insulationBOT",							//pet insulation BOT
		"GT2_Bakelite",								//bakelite layer
		"GT2_Honeycomb_skin2_1", "GT2_Honeycomb_core2","GT2_Honeycomb_skin2_2",		//honeycomb panel BOTTOM

		
		};



		std::string NomeStratiLog_GT2[Nstrati_GT2];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati_GT2; A++) { 
		    NomeStratiLog[A]="GT2_"+NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati_GT2[Nstrati_GT2] =				//array materiali 
		{
		
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		Cu, PET, PET, EVA, Graphite, HPL,				//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		RPCgas,								//GASGAP 2
		HPL, Graphite, EVA, PET, PET, Cu,				// HPL2_2
		
		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM

		};



		G4double spessoreStrati_GT2[Nstrati_GT2]=				//array spessori
		{

	
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		2.0*mm,								//GASGAP 1
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					
		
		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,		//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"		
		2.0*mm,								//GASGAP 2
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2"		
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
	
		
		};

		

		//volumi LOGICI
	


		for(G4int lyr_GT2=0;lyr_GT2<Nstrati_GT2;lyr_GT2++){			//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Box* strato_GT2= new G4Box(NomeStrati_GT2[lyr_GT2],spessoreStrati_GT2[lyr_GT2]/2, 500/2, 700/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato_GT2 = new G4LogicalVolume(strato_GT2, MatStrati_GT2[lyr_GT2], NomeStratiLog_GT2[lyr_GT2]) ;

			
			logicStrato_GT2->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


			trdCollection_GT2.push_back(strato_GT2) ;				//mette forma volume nel vettore
		        trdLogCollection_GT2.push_back(logicStrato_GT2) ;		     	//mette ogni volume logico nel vettore
			}
 


	//VOLUMI FISICI

G4double ZTranslation_GT2= ZTranslation_GT1-150;	// 150mm di distanza tra GT1 E GT2

G4int cpN_GT2=1;		// indice numero strato per stampare a video informazioni

//

	G4cout << " Detector GT2"<< G4endl ;
     for(size_t i=0 ; i<trdCollection_GT2.size() ; i++) {   		// i counts as the copyNo per loop

	ZTranslation_GT2 -= trdCollection_GT2.at(i)->GetXHalfLength() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection_GT2.at(i)->GetXHalfLength()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN_GT2 << ") " << trdCollection_GT2.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The position is " << G4BestUnit(ZTranslation_GT2,"Length") << G4endl ;

       G4VPhysicalVolume* trapezoidalPhysic_GT2 = new G4PVPlacement(		//crea volume fisico					
	  rotationPlacement,							//rotazione
	  G4ThreeVector(Xkodel,Hkodel,ZTranslation_GT2),				//vettore posizione
	  trdLogCollection_GT2.at(i),						//nome del volume logico
	  trdCollection_GT2.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN_GT2,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       ZTranslation_GT2 -= trdCollection_GT2.at(i)->GetXHalfLength() ;			// incremento traslazione della seconda metà dello spessore
       cpN_GT2++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------Added GT2  ------------------------------" << G4endl ;




//      ++BARI-A++

std::vector<G4Box*>           trdCollection_BARIA ;			//collezione del nome dei volumi
std::vector<G4LogicalVolume*> trdLogCollection_BARIA ;		//collezione dei volumi logici


#define Nstrati_BARIA 26						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati_BARIA[Nstrati_BARIA]= 					//array nome strati
		{  


		"BARIA_Honeycomb_skin1_1", "BARIA_Honeycomb_core1","BARIA_Honeycomb_skin1_2",		//honeycomb panel  TOP
		"BARIA_PET_insulationTOP",							//pet insulation TOP
		"BARIA_paper",								//layer paper

		"BARIA_Copper1_1", "BARIA_PET1_1","BARIA_PET1_2", "BARIA_EVA1_1","BARIA_graphite1_1","BARIA_HPL1_1",	//HPL1_1
		"BARIA_GasGap1",								//GASGAP 1
		"BARIA_HPL1_2","BARIA_graphite1_2", "BARIA_EVA1_2", "BARIA_PET1_3","BARIA_PET1_4","BARIA_Copper1_2",	// HPL1_2

		"BARIA_PETstrip1","BARIA_Copper_Strip", "BARIA_PETstrip2",					//STRIP

		"BARIA_PET_insulationBOT",							//pet insulation BOT
		"BARIA_Bakelite",								//bakelite layer
		"BARIA_Honeycomb_skin2_1", "BARIA_Honeycomb_core2","BARIA_Honeycomb_skin2_2",		//honeycomb panel BOTTOM

		
		};



		std::string NomeStratiLog_BARIA[Nstrati_BARIA];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati_BARIA; A++) { 
		    NomeStratiLog[A]="BARIA_"+NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati_BARIA[Nstrati_BARIA] =				//array materiali 
		{
		
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM

		};



		G4double spessoreStrati_BARIA[Nstrati_BARIA]=				//array spessori
		{

	
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		2.0*mm,								//GASGAP 1
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
	
		
		};

		

		//volumi LOGICI
	


		for(G4int lyr_BARIA=0;lyr_BARIA<Nstrati_BARIA;lyr_BARIA++){			//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Box* strato_BARIA= new G4Box(NomeStrati_BARIA[lyr_BARIA],spessoreStrati_BARIA[lyr_BARIA]/2, 100/2, 100/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato_BARIA = new G4LogicalVolume(strato_BARIA, MatStrati_BARIA[lyr_BARIA], NomeStratiLog_BARIA[lyr_BARIA]) ;

			
			logicStrato_BARIA->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


			trdCollection_BARIA.push_back(strato_BARIA) ;				//mette forma volume nel vettore
		        trdLogCollection_BARIA.push_back(logicStrato_BARIA) ;		     	//mette ogni volume logico nel vettore
			}
 



	//VOLUMI FISICI

G4double ZTranslation_BARIA= ZTranslation_GT2-150;	// 150mm di distanza tra BARI E GT2

G4int cpN_BARIA=1;		// indice numero strato per stampare a video informazioni

//

	G4cout << " Detector BARIA"<< G4endl ;
     for(size_t i=0 ; i<trdCollection_BARIA.size() ; i++) {   		// i counts as the copyNo per loop

	ZTranslation_BARIA -= trdCollection_BARIA.at(i)->GetXHalfLength() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection_BARIA.at(i)->GetXHalfLength()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN_BARIA << ") " << trdCollection_BARIA.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The position is " << G4BestUnit(ZTranslation_BARIA,"Length") << G4endl ;

       G4VPhysicalVolume* trapezoidalPhysic_BARIA = new G4PVPlacement(		//crea volume fisico					
	  rotationPlacement,							//rotazione
	  G4ThreeVector(Xkodel,Hkodel,ZTranslation_BARIA),				//vettore posizione
	  trdLogCollection_BARIA.at(i),						//nome del volume logico
	  trdCollection_BARIA.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN_BARIA,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       ZTranslation_BARIA -= trdCollection_BARIA.at(i)->GetXHalfLength() ;			// incremento traslazione della seconda metà dello spessore
       cpN_BARIA++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------Added BARI A  ------------------------------" << G4endl ;





//      ++BARI-B++

std::vector<G4Box*>           trdCollection_BARIB ;			//collezione del nome dei volumi
std::vector<G4LogicalVolume*> trdLogCollection_BARIB ;		//collezione dei volumi logici


#define Nstrati_BARIB 26						//costante per definire NUMERO DI STRATI (con int non funziona)

	//CONTINOUS MATERIALS
		std::string NomeStrati_BARIB[Nstrati_BARIB]= 					//array nome strati
		{  


		"BARIB_Honeycomb_skin1_1", "BARIB_Honeycomb_core1","BARIB_Honeycomb_skin1_2",		//honeycomb panel  TOP
		"BARIB_PET_insulationTOP",							//pet insulation TOP
		"BARIB_paper",								//layer paper

		"BARIB_Copper1_1", "BARIB_PET1_1","BARIB_PET1_2", "BARIB_EVA1_1","BARIB_graphite1_1","BARIB_HPL1_1",	//HPL1_1
		"BARIB_GasGap1",								//GASGAP 1
		"BARIB_HPL1_2","BARIB_graphite1_2", "BARIB_EVA1_2", "BARIB_PET1_3","BARIB_PET1_4","BARIB_Copper1_2",	// HPL1_2

		"BARIB_PETstrip1","BARIB_Copper_Strip", "BARIB_PETstrip2",					//STRIP

		"BARIB_PET_insulationBOT",							//pet insulation BOT
		"BARIB_Bakelite",								//bakelite layer
		"BARIB_Honeycomb_skin2_1", "BARIB_Honeycomb_core2","BARIB_Honeycomb_skin2_2",		//honeycomb panel BOTTOM

		
		};



		std::string NomeStratiLog_BARIB[Nstrati_BARIB];				//ciclo per aggiungere Log a nome strati
		
		for(size_t A=0; A<Nstrati_BARIB; A++) { 
		    NomeStratiLog[A]="BARIB_"+NomeStrati[A]+"Log";
		    }



		G4Material* MatStrati_BARIB[Nstrati_BARIB] =				//array materiali 
		{
		
		Al, AlCore ,Al,								//honeycomb panel  TOP
		PET,								//pet insulation TOP
		paper,								//layer paper

		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
		RPCgas,								//GASGAP 1
		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"		

		PET,								//pet insulation BOT
		HPL,								//bakelite layer
		Al, AlCore, Al,								//honeycomb panel BOTTOM

		};



		G4double spessoreStrati_BARIB[Nstrati_BARIB]=				//array spessori
		{

	
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
		0.188*mm,							//pet insulation TOP
		1.0*mm,								//layer paper

		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 2.0*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"		
		2.0*mm,								//GASGAP 1
		2.0*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"		

		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"					
		
		0.188*mm,							//pet insulation BOT
		2.0*mm,								//bakelite layer
		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
	
		
		};

		

		//volumi LOGICI
	


		for(G4int lyr_BARIB=0;lyr_BARIB<Nstrati_BARIB;lyr_BARIB++){			//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore

			G4Box* strato_BARIB= new G4Box(NomeStrati_BARIB[lyr_BARIB],spessoreStrati_BARIB[lyr_BARIB]/2, 100/2, 100/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
			G4LogicalVolume* logicStrato_BARIB = new G4LogicalVolume(strato_BARIB, MatStrati_BARIB[lyr_BARIB], NomeStratiLog_BARIB[lyr_BARIB]) ;

			
			logicStrato_BARIB->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


			trdCollection_BARIB.push_back(strato_BARIB) ;				//mette forma volume nel vettore
		        trdLogCollection_BARIB.push_back(logicStrato_BARIB) ;		     	//mette ogni volume logico nel vettore
			}
 



	//VOLUMI FISICI

G4double ZTranslation_BARIB= ZTranslation_BARIA-10;	// 1 CM TRA BARI A  E BARI B

G4int cpN_BARIB=1;		// indice numero strato per stampare a video informazioni

//

	G4cout << " Detector BARIB"<< G4endl ;
     for(size_t i=0 ; i<trdCollection_BARIB.size() ; i++) {   		// i counts as the copyNo per loop

	ZTranslation_BARIB -= trdCollection_BARIB.at(i)->GetXHalfLength() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection_BARIB.at(i)->GetXHalfLength()*2 ;		//prende spessore del volume per stamparlo a video

      G4cout << "Volume (" << cpN_BARIB << ") " << trdCollection_BARIB.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The position is " << G4BestUnit(ZTranslation_BARIB,"Length") << G4endl ;

       G4VPhysicalVolume* trapezoidalPhysic_BARIB = new G4PVPlacement(		//crea volume fisico					
	  rotationPlacement,							//rotazione
	  G4ThreeVector(Xkodel,Hkodel,ZTranslation_BARIB),				//vettore posizione
	  trdLogCollection_BARIB.at(i),						//nome del volume logico
	  trdCollection_BARIB.at(i)->GetName(),					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  cpN_BARIB,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)	
       
       ZTranslation_BARIB -= trdCollection_BARIB.at(i)->GetXHalfLength() ;			// incremento traslazione della seconda metà dello spessore
       cpN_BARIB++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
       }

G4cout << "-------------------Added BARI B  ------------------------------" << G4endl ;







// SBARRA DAVANTI A KODEL

//sbarra lunga
G4VSolid* sbarra= new G4Box("sbarra", 50/2,1000/2, 55/2); 		// spessore, lunghezza, larghezza
G4LogicalVolume* logic_sbarra = new G4LogicalVolume(sbarra, Al , "sbarra") ;		

			
logic_sbarra ->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


G4VPhysicalVolume* physic_sbarra = new G4PVPlacement(		//crea volume fisico					
	  new G4RotationMatrix(),							//rotazione
	  G4ThreeVector(Xkodel+550,Hkodel,Zkodel+50),			//vettore posizione  (x sbarra spostata di 550 rispetto al centro kodel)
	  logic_sbarra,						//nome del volume logico
	  "sbarra",					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  1,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)



//sbarra bassa larga
G4VSolid* sbarra_larga= new G4Box("sbarra_larga", 60/2,244/2, 60/2); 		// spessore, lunghezza, larghezza
G4LogicalVolume* logic_sbarra_larga = new G4LogicalVolume(sbarra_larga, Al , "sbarra_larga") ;		

			
logic_sbarra_larga ->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


G4VPhysicalVolume* physic_sbarra_larga = new G4PVPlacement(		//crea volume fisico					
	  new G4RotationMatrix(),							//rotazione
	  G4ThreeVector(Xkodel+550+55,-312,Zkodel+50),	//vettore posizione  (x spostata di 550 rispetto a centro kodel + 55 della sbarra lunga)
	  logic_sbarra_larga,						//nome del volume logico
	  "sbarra_larga",					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  1,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)


//sbarra bassa stretta
G4VSolid* sbarra_stretta= new G4Box("sbarra_stretta", 50/2,150/2, 50/2); 		// spessore, lunghezza, larghezza
G4LogicalVolume* logic_sbarra_stretta = new G4LogicalVolume(sbarra_stretta, Al , "sbarra_stretta") ;		

			
logic_sbarra_stretta ->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));


G4VPhysicalVolume* physic_sbarra_stretta = new G4PVPlacement(		//crea volume fisico					
	  new G4RotationMatrix(),							//rotazione
	  G4ThreeVector(Xkodel+550-55,-312,Zkodel+50),	//vettore posizione   (x spostata di 550 rispetto a centro kodel - 55 della sbarra lunga)
	  logic_sbarra_stretta,						//nome del volume logico
	  "sbarra_stretta",					//nome volume fisico
	  physWorld->GetLogicalVolume(),					//volume madre in cui posizionare
	  false,								//pmany
	  1,								//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)





G4cout << "-------------------Added sbarre  ------------------------------" << G4endl ;



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


	//
	//always return the physical World

	return physWorld;
}


