#include "G4RunManager.hh"
#include "G4ScoringManager.hh"
#include "G4UImanager.hh"

#include "Randomize.hh"

#include "GIF++DetectorConstruction.hh"
#include "GIF++PhysicsList.hh"
#include "GIF++PrimaryGeneratorAction.hh"
#include "GIF++RunAction.hh"
#include "GIF++EventAction.hh"
//#include "GIF++StackingAction.hh"
#include "GIF++SteppingAction.hh"
//#include "GIF++UserScoreWriter.hh"
#include "GIF++DataManager.hh"
#include "HistoManager.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#define G4_TRACKING_DEBUG 1
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


int main(int argc,char** argv)
{
	// Choose the Random engine
	//
	CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

	//set random seed with system time
	G4long seed = time(NULL);
	CLHEP::HepRandom::setTheSeed(seed);

	GIFppDataManager* dataManager = GIFppDataManager::GetInstance();

	//GIFppUserScoreWriter* scoreWriter = new GIFppUserScoreWriter();

	// Construct the default run manager
	//
	G4RunManager * runManager = new G4RunManager;

	//G4ScoringManager* scoringManager =  G4ScoringManager::GetScoringManager();
	//scoringManager->SetScoreWriter(scoreWriter);


	//if (argc!=1)   // batch mode
	//{
	//	dataManager->setFilenameGDML(argv[2]);
	//	dataManager->setDescription(argv[3]);
	//}
	//else
	//{
		dataManager->setFilenameGDML("./macros/GIFpp_DS_US.gdml");
		dataManager->setDescription("GIFpp");
	//}

	// Set mandatory initialization classes
	GIFppDetectorConstruction* detectorConstruction = new GIFppDetectorConstruction();

	runManager->SetUserInitialization(detectorConstruction);
	//
	runManager->SetUserInitialization(new GIFppPhysicsList);

  // set an HistoManager 
  HistoManager*  histo = new HistoManager();


	// Set user action classes
	//
	runManager->SetUserAction(new GIFppPrimaryGeneratorAction());
	
	GIFppRunAction* runAction = new GIFppRunAction();
	//
	runManager->SetUserAction(runAction);
	//
	runManager->SetUserAction(new GIFppEventAction);
	//
	//runManager->SetUserAction(new GIFppStackingAction);  
	
	runManager->SetUserAction(new GIFppSteppingAction); 

	// Initialize G4 kernel
	//
	runManager->Initialize();

#ifdef G4VIS_USE
	// Initialise visualisation
	G4VisManager* visManager = new G4VisExecutive;
	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
	// G4VisManager* visManager = new G4VisExecutive("Quiet");
	visManager->Initialize();
#endif

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	if (argc!=1)   // batch mode
	{
		G4String fileName = argv[1];
		G4String command = "/control/execute ";
		UImanager->ApplyCommand(command+fileName);
	}
	else
	{  // interactive mode : define UI session
#ifdef G4UI_USE
		G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE 
		UImanager->ApplyCommand("/control/execute ./macros/vis.mac");
#endif
		ui->SessionStart();

		delete ui;
#endif
	}

	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	// owned and deleted by the run manager, so they should not be deleted 
	// in the main() program !

#ifdef G4VIS_USE
	delete visManager;
#endif
	delete runManager;

	return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
