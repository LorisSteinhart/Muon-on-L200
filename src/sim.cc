#include <iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4ScoringManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Shielding.hh"

#include "construction.hh"
#include "physics.hh"
#include "action.hh"

// #include "G4Random.hh"
// #include <ctime>

int main(int argc, char** argv)
{   
    G4ScoringManager::GetScoringManager();
    G4RunManager *runManager = new G4RunManager();
    /* Initialize all your implemented stuff*/
    runManager->SetUserInitialization(new MyDetectorConstruction()); //Uses your Detectorconstruction
    runManager->SetUserInitialization(new MyPhysicsList());     // Use your physics
    G4cout << "After PhysicsList" << G4endl;
    // G4VModularPhysicsList *physics = new Shielding();    
    //physics->RegisterPhysics(new G4OpticalPhysics);
    // runManager->SetUserInitialization(physics);
    runManager->SetUserInitialization(new MyActionInitialization()); // Particle gun

    runManager->Initialize();                                // To use the created geometry, needs physics list

    G4UIExecutive *ui = 0;

    if (argc == 1)
    {
        ui = new G4UIExecutive(argc, argv);
    }

    G4VisManager *visManager = new G4VisExecutive();
    visManager->Initialize();

    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    if(ui)
    {
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
    }
    else
    {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
    }

    return 0;
}