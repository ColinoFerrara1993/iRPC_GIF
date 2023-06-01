//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file GIFppDataManager.hh
/// \brief Definition of the GIFppDataManager class

#ifndef GIFppDataManager_h
#define GIFppDataManager_h 1


#include "globals.hh"
#include "G4ThreeVector.hh"
#include <map>
#include <vector>


class GIFppDataManager
{
public:
	static GIFppDataManager* GetInstance();
	static void Dispose();

	void setDescription(G4String description);
	void setFilenameGDML(G4String filenameGDML);
	G4String getFilenameGDML();
	G4String getDescription();
	G4int getNumberOfEvents();
	void setNumberOfEvents(G4int numberOfEvents);

private:
	GIFppDataManager();
	~GIFppDataManager(){};
	G4String fDescription;
	G4String fFilenameGDML;
	G4double fNumberOfEvents;
	static GIFppDataManager* fDataManager;


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

