#include "run.hh"

MyRunAction::MyRunAction()
{
    nGd = 0;
    nWat = 0;
    nspSD = 0;
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    // Ntuple für Enriched Germanium erstellen
    man->CreateNtuple("GermaniumHits", "Hits on Germanium");
    man->CreateNtupleIColumn("Eventnumber");
    man->CreateNtupleSColumn("Particlename");
    man->CreateNtupleDColumn("X_Position(mm)");
    man->CreateNtupleDColumn("Y_Position(mm)");
    man->CreateNtupleDColumn("Z_Position(mm)");
    man->CreateNtupleDColumn("Energy");
    man->CreateNtupleDColumn("Time");
    man->CreateNtupleDColumn("TrackID");
    man->CreateNtupleSColumn("Process");
    man->CreateNtupleDColumn("ParentID");
    man->CreateNtupleDColumn("generationMarker");
    man->CreateNtupleSColumn("CreatorProcess");
    man->CreateNtupleDColumn("Edep");
    man->CreateNtupleSColumn("Volumen");
    man->FinishNtuple(0);

    // Ntuple für Gadolinium erstellen
    man->CreateNtuple("GadoliniumHits", "Hits on Gadolinium");
    man->CreateNtupleIColumn("Eventnumber");
    man->CreateNtupleSColumn("Particlename");
    man->CreateNtupleDColumn("X_Position(mm)");
    man->CreateNtupleDColumn("Y_Position(mm)");
    man->CreateNtupleDColumn("Z_Position(mm)");
    man->CreateNtupleDColumn("Energy");
    man->CreateNtupleDColumn("Time");
    man->CreateNtupleDColumn("primaryEnergy");
    man->FinishNtuple(1);

    // Ntuple für sphericalDetector erstellen
    man->CreateNtuple("sphericalDetectorHits", "Hits on sphericalDetector");
    man->CreateNtupleIColumn("Neutronhits");
    // man->CreateNtupleDColumn("Time");
    // man->CreateNtupleDColumn("generationMarker");
    // man->CreateNtupleDColumn("Edep");
    man->FinishNtuple(2);
}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run* run){
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    G4int runID = run->GetRunID();

    std::stringstream strRunID;  // converts to string i guess?
    strRunID << runID;
    nGd = 0;
    nWat = 0;
    nspSD = 0;
    man->OpenFile("output9"+strRunID.str()+"_gerdaSetup_shpericalCylinder_16x16x20_musun.csv");
}

void MyRunAction::EndOfRunAction(const G4Run*){
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man->Write();
    man->CloseFile();
    G4cout << "Gd Captures: " << nGd << G4endl;
    G4cout << "Water Captures: " << nWat << G4endl;
    G4cout << "Neutrons hitting the spSD: " << nspSD << G4endl;
}