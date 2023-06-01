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
/// \file GIFppPhysicsList.cc
/// \brief Implementation of the GIFppPhysicsList class

#include "GIF++PhysicsList.hh"
#include "G4UnitsTable.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppPhysicsList::GIFppPhysicsList() 
	: G4VModularPhysicsList(){
		SetVerboseLevel(1);

		defaultCutValue = 0.1*CLHEP::mm;
		RegisterPhysics(new G4EmLivermorePhysics());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GIFppPhysicsList::~GIFppPhysicsList()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GIFppPhysicsList::SetCuts()
{
	   

	if (verboseLevel >0){
		G4cout << "PhysicsList::SetCuts:";
		G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
	}
	G4VUserPhysicsList::SetCutsWithDefault();
	if (verboseLevel >0) DumpCutValuesTable();
}  
