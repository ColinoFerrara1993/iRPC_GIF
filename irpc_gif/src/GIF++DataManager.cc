#include "GIF++DataManager.hh"



GIFppDataManager* GIFppDataManager::fDataManager = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

GIFppDataManager* GIFppDataManager::GetInstance() {
	if (!fDataManager) {
		fDataManager = new GIFppDataManager();
	}
	return fDataManager;
}

void GIFppDataManager::Dispose() {
	delete fDataManager;
	fDataManager = 0;
}


GIFppDataManager::GIFppDataManager():
fNumberOfEvents(0)
{
	

}

void GIFppDataManager::setDescription(G4String description)
{
	fDescription = description;
}

void GIFppDataManager::setFilenameGDML(G4String filenameGDML) 
{
	fFilenameGDML = filenameGDML;
}

G4String GIFppDataManager::getFilenameGDML() 
{
	return fFilenameGDML;
}

G4String GIFppDataManager::getDescription()
{
	return fDescription;
}


G4int GIFppDataManager::getNumberOfEvents()
{
	return fNumberOfEvents;
}

void GIFppDataManager::setNumberOfEvents(G4int numberOfEvents)
{
	fNumberOfEvents=numberOfEvents;
}

