#ifndef GIFppRunAction_h
#define GIFppRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "GIF++SteppingAction.hh"
#include "GIF++EventAction.hh"
#include "GIF++PrimaryGeneratorAction.hh"
#include "HistoManager.hh"




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class G4Run;
class GIFppSteppingAction;
class GIFppEventAction;
class GIFppPrimaryGeneratorAction;


//root class

class TFile;
class TH1D;
class TH2D;
class TH3D;
class TTree;
class TNtuple;
class TGraph;



class GIFppRunAction : public G4UserRunAction
{
  public:
    GIFppRunAction();
    virtual ~GIFppRunAction();

    // static access method  (per accere da altri file)
    static GIFppRunAction* Instance();

    virtual void BeginOfRunAction(const G4Run* aRun);
    virtual void EndOfRunAction(const G4Run* aRun);


private:

   static GIFppRunAction* fgInstance;  // per accere da altri file

   G4long fTime;				//conta tempo per eseguire run

	// variaibili da prendere da event (prese precedentemente da step)
    G4int     n_incidenti_final;			// contatore particelle incidenti bot + top
    G4int     n_incidenti_top_final;			// contatore particelle incidenti top
    G4int     n_incidenti_bot_final;			// contatore particelle incidenti bot

    G4int     n_cariche_final;			// contatore particelle cariche in gap 1 + gap2
    G4int  n_cariche_gap1_final;			// contatore particelle cariche in gap1
    G4int   n_cariche_gap2_final;			// contatore particelle cariche in gap2

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif





