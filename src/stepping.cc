#include "stepping.hh"

MySteppingAction::MySteppingAction(MyEventAction *eventAction)
{
    fEventAction = eventAction;
}

MySteppingAction::~MySteppingAction()
{}

void MySteppingAction::UserSteppingAction(const G4Step *step)
{   
    // G4LogicalVolume *volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    
    // const MyDetectorConstruction *detectorConstruction = static_cast<const MyDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    
    // G4LogicalVolume *fScoringVolume = detectorConstruction->GetScoringVolume();
    
    // if(volume != fScoringVolume)
    //     return;
    
    // G4double edep = step->GetTotalEnergyDeposit();
    // fEventAction->AddEdep(edep);

    const G4Track* track = step->GetTrack();
    MyTrackInformation* trackInfo = dynamic_cast<MyTrackInformation*>(track->GetUserInformation());
    // G4cout << "trackInfo "  << G4endl;

    // Wenn der aktuelle Track eine Markierung hat
    if (trackInfo) {
        int currentMarker = trackInfo->GetGenerationMarker();
        
        G4cout << "Aktueller Track: " 
               << track->GetDefinition()->GetParticleName() 
               << ", Generation Marker: " << currentMarker 
               << ", Energie: " << track->GetKineticEnergy() / keV << " keV" << G4endl;

        // Falls Sekundärteilchen erzeugt wurden, übertrage die Markierung und gib die Information aus
        const std::vector<const G4Track*>* secondaries = step->GetSecondaryInCurrentStep();
        for (const auto* secondary : *secondaries) {
            MyTrackInformation* secondaryInfo = new MyTrackInformation(currentMarker + 1);
            const_cast<G4Track*>(secondary)->SetUserInformation(secondaryInfo);
            
            G4cout << "Erzeugtes Secondary: "
                   << secondary->GetDefinition()->GetParticleName()
                   << ", Neue Generation Marker: " << (currentMarker + 1)
                   << ", Energie: " << secondary->GetKineticEnergy() / keV << " keV" << G4endl;
        }
    }
}
