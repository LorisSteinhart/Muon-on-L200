#include "detector.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{
    // quEff = new G4PhysicsOrderedFreeVector();

    // std::ifstream datafile;
    // datafile.open("eff.dat");

    // while(1)
    // {
    //     G4double wlen, queff;

    //     datafile >> wlen >> queff;

    //     if(datafile.eof())
    //         break;

    //     G4cout << wlen << " " << queff << std::endl;

    //     quEff->InsertValues(wlen, queff/100.);
    // }

    // datafile.close();

    // quEff->SetSpline(false);
}

MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *ROhist)
{
    // const MyRunAction* fRunAction = static_cast<const MyRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());

    // G4cout << "Entering ProcessHits" << G4endl;
    G4Track *track = step->GetTrack();

    // track->SetTrackStatus(fStopAndKill);
    G4StepPoint *preStepPoint = step->GetPreStepPoint();
    G4StepPoint *postStepPoint = step->GetPostStepPoint();

    // const G4StepPoint* endPoint = step->GetPostStepPoint();
    G4VProcess* process = const_cast<G4VProcess*>(postStepPoint->GetProcessDefinedStep());
    const G4VProcess* creatorProcess = track->GetCreatorProcess();
    
    G4ParticleDefinition* particle = track->GetDefinition();
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable(); // Load the particle table with all stored properties
    G4String requiredparticleName ="neutron";
    G4ParticleDefinition *requiredParticle = particleTable->FindParticle(requiredparticleName);

    G4ThreeVector posPhoton = preStepPoint->GetPosition();
    G4ThreeVector momPhoton = preStepPoint->GetMomentum();

    G4double time = preStepPoint->GetGlobalTime();
    G4double wlen = (1.239841939*eV/momPhoton.mag())*1E+03;
    G4ParticleDefinition* particleDef = track->GetDefinition();
    G4String particleName = particleDef->GetParticleName();

    G4int parentID = track->GetParentID();
    G4int trackID = track->GetTrackID();
    G4String parentName = "noParent";
    G4int generationMarker = 0;
    G4double edep = step->GetTotalEnergyDeposit();
    // G4cout << "Position of " << particleName << ": " << posPhoton << G4endl;

    // const G4VTouchable *touchable = step->GetPreStepPoint()->GetTouchable();

    // G4int copyNo = touchable->GetCopyNumber();

    // //G4cout << "Copy number: " << copyNo << G4endl;

    // G4VPhysicalVolume *physVol = touchable->GetVolume();
    // G4ThreeVector posDetector = physVol->GetTranslation();

    // #ifndef G4MULTITHREADED
    //     //G4cout << "Detector position: " << posDetector << G4endl;
    //     //G4cout << "Photon wavelength: " << wlen << G4endl;
    // #endif

    G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    G4AnalysisManager *man = G4AnalysisManager::Instance();
    
    std::map<G4int, G4String> parentParticleNames; 
    if (GetName() == "GeSD"){ 
    // if(false){
        // G4cout << "GeSD case " << particleName << G4endl;
        // Name des aktuellen Volumens
        G4TouchableHandle touchable = preStepPoint->GetTouchableHandle();
        G4String volumeName = touchable->GetVolume()->GetName();
        auto trackInfo = track->GetUserInformation();
        MyTrackInformation* MytrackInfo = dynamic_cast<MyTrackInformation*>(trackInfo);

        // G4cout << "trackInfo " << trackInfo << G4endl;
        // Wenn eine Markierung existiert, dann prüfe den Generation Marker
        if (MytrackInfo) {
            // G4cout << "Entering if trackInfo" << G4endl;
            generationMarker = MytrackInfo->GetGenerationMarker();
            if (generationMarker > 1){
                // G4cout << "Es wurde der Fall 'generationMarker > 1' bei Eventnummer " << evt << " detektiert." << G4endl;
            }
        }
    
        man->FillNtupleIColumn(0, 0, evt);               // Eventnumber
        man->FillNtupleSColumn(0, 1, particleName);      // Particlename
        G4ThreeVector position = postStepPoint->GetPosition();
        man->FillNtupleDColumn(0, 2, position.getX());   // X_Position
        man->FillNtupleDColumn(0, 3, position.getY());   // Y_Position
        man->FillNtupleDColumn(0, 4, position.getZ());   // Z_Position
        man->FillNtupleDColumn(0, 5, preStepPoint->GetKineticEnergy()); // Energy
        man->FillNtupleDColumn(0, 6, time);
        man->FillNtupleDColumn(0, 7, trackID);
        man->FillNtupleSColumn(0, 8, process->GetProcessName());
        man->FillNtupleDColumn(0, 9, parentID);
        man->FillNtupleDColumn(0, 10, generationMarker);        
        if(creatorProcess){
            man->FillNtupleSColumn(0, 11, creatorProcess->GetProcessName());
        }
        
        man->FillNtupleDColumn(0, 12, step->GetTotalEnergyDeposit());
        man->FillNtupleSColumn(0, 13, volumeName);
        man->AddNtupleRow(0);
    }

    if(GetName() == "GdSD" && particle == requiredParticle && step->GetTrack()->GetTrackStatus() == fStopAndKill && process->GetProcessName() == "RMGnCapture"){
    // if(false){
        // G4cout << GetName() << G4endl;
        // G4cout << "GdSD case " << particleName << G4endl;
        // G4cout << "GdSD case " << process->GetProcessName() << G4endl;
        // G4HadronicProcess* hproc = dynamic_cast<G4HadronicProcess*>(process);
        // const G4Isotope* target = NULL;
        // G4cout << "process->GetTargetIsotope() " << process->GetTargetIsotope() << G4endl;
        // if (hproc) target = hproc->GetTargetIsotope();
        // G4String targetName = "XXXX";  
        if (process) {
            // targetName = target->GetName();
            // G4cout << "evt: " << evt << G4endl;
            man->FillNtupleIColumn(1,0,evt);
            man->FillNtupleSColumn(1,1,particleName);
            G4ThreeVector position = postStepPoint->GetPosition();
            G4double x_position = position.getX();
            G4double y_position = position.getY();
            G4double z_position = position.getZ();
            // G4cout << "x: " << x_position << G4endl;
            // G4cout << "y: " << y_position << G4endl;
            // G4cout << "z: " << z_position << G4endl;
            man->FillNtupleDColumn(1, 2, x_position);
            man->FillNtupleDColumn(1, 3, y_position);
            man->FillNtupleDColumn(1, 4, z_position);
            G4double energy = preStepPoint->GetKineticEnergy();
            man->FillNtupleDColumn(1, 5, energy);
            man->FillNtupleDColumn(1, 6, time);
            man->FillNtupleDColumn(1, 7, track->GetVertexKineticEnergy());
            man->AddNtupleRow(1);
            
            // if(targetName.contains("Gd"))
            // {
            //     fEventAction->AddnGd(1);
            // }
            // if(targetName.contains("H1") || targetName.contains("O16"))
            // {
            //     fEventAction->AddnWat(1);
            // }
        }
    }

    if(GetName() == "spSD" && particle == requiredParticle && step->IsFirstStepInVolume()){
        // fRunAction->AddnWat(1);   
        man->FillNtupleIColumn(2,0,1); 
        man->AddNtupleRow(2);    
    }

    


    // if (GetName() == "ArSD"){         
    //     auto trackInfo = track->GetUserInformation();
    //     MyTrackInformation* MytrackInfo = dynamic_cast<MyTrackInformation*>(trackInfo);

    //     // G4cout << "trackInfo " << trackInfo << G4endl;
    //     // Wenn eine Markierung existiert, dann prüfe den Generation Marker
    //     if (MytrackInfo) {
    //         // G4cout << "Entering if trackInfo" << G4endl;
    //         generationMarker = MytrackInfo->GetGenerationMarker();
    //         if (generationMarker > 1){
    //             // G4cout << "Es wurde der Fall 'generationMarker > 1' bei Eventnummer " << evt << " detektiert." << G4endl;
    //         }
    //     }
    
    //     man->FillNtupleIColumn(2, 0, evt);               // Eventnumber
    //     // man->FillNtupleSColumn(2, 1, particleName);      // Particlename
    //     // G4ThreeVector position = postStepPoint->GetPosition();
    //     // man->FillNtupleDColumn(2, 2, position.getX());   // X_Position
    //     // man->FillNtupleDColumn(2, 3, position.getY());   // Y_Position
    //     // man->FillNtupleDColumn(2, 4, position.getZ());   // Z_Position
    //     // man->FillNtupleDColumn(2, 5, preStepPoint->GetKineticEnergy()); // Energy
    //     man->FillNtupleDColumn(2, 1, time);
    //     // man->FillNtupleDColumn(2, 7, trackID);
    //     // man->FillNtupleSColumn(2, 8, process->GetProcessName());
    //     // man->FillNtupleDColumn(2, 9, parentID);
    //     man->FillNtupleDColumn(2, 2, generationMarker);        
    //     // if(creatorProcess){
    //     //     man->FillNtupleSColumn(2, 11, creatorProcess->GetProcessName());
    //     // }
        
    //     man->FillNtupleDColumn(2, 3, step->GetTotalEnergyDeposit());
    //     man->AddNtupleRow(2);
    // }

    

    return true;
}