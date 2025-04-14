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
    man->CreateNtupleDColumn("Time");
    man->CreateNtupleDColumn("generationMarker");
    man->CreateNtupleDColumn("Edep");
    man->CreateNtupleSColumn("Volumen");
    man->FinishNtuple(0);

    // Ntuple für sphericalDetector erstellen
    man->CreateNtuple("sensitiveDetectorHits", "Hits on sensitiveDetector");
    man->CreateNtupleIColumn("Neutronhits");
    man->FinishNtuple(1);
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
    man->OpenFile("output"+strRunID.str()+"_gerdaSetup_16x16x20_musun.csv");
}

void MyRunAction::EndOfRunAction(const G4Run*){
    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man->Write();
    man->CloseFile();
    G4cout << "Gd Captures: " << nGd << G4endl;
    G4cout << "Water Captures: " << nWat << G4endl;
    G4cout << "Neutrons hitting the spSD: " << nspSD << G4endl;
}
