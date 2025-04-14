#include "physics.hh"

#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4RunManager.hh"
#include "G4NeutronCaptureProcess.hh"
#include "G4ParticleHPCaptureData.hh"
#include "G4HadronPhysicsShielding.hh"
#include "G4BaryonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4ShortLivedConstructor.hh"
#include "G4StoppingPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4ThermalNeutrons.hh"

#include "Shielding.hh"
#include "RMGNeutronCaptureProcess.hh"


MyPhysicsList::MyPhysicsList() {
  

    this->DefineCommands();
}

void MyPhysicsList::ConstructParticle() {

  G4cout <<  "Constructing particles" <<G4endl;

  G4BosonConstructor::ConstructParticle();
  G4LeptonConstructor::ConstructParticle();
  G4MesonConstructor::ConstructParticle();
  G4BaryonConstructor::ConstructParticle();
  G4IonConstructor::ConstructParticle();
  G4ShortLivedConstructor::ConstructParticle();
}

void MyPhysicsList::ConstructProcess() {
    G4cout << "Entering ConstructProcess" << G4endl;
    
    G4VUserPhysicsList::AddTransportation();
    G4cout << "AddTransportation" << G4endl;
    G4VPhysicsConstructor* em_constructor = new G4EmStandardPhysics();
    em_constructor->ConstructProcess();
    G4cout << "Using Standard electromagnetic physics" << G4endl;

    G4cout << "Adding extra electromagnetic physics" << G4endl;

    auto em_extra_physics = new G4EmExtraPhysics(G4VModularPhysicsList::verboseLevel);
    em_extra_physics->Synch(true);
    em_extra_physics->GammaNuclear(true);
    em_extra_physics->MuonNuclear(true);
    em_extra_physics->ConstructProcess();

    G4cout << "Adding hadronic elastic physics" << G4endl;
    G4VPhysicsConstructor* hElasticPhysics = new G4HadronElasticPhysicsHP(G4VModularPhysicsList::verboseLevel);
    hElasticPhysics->ConstructProcess();

    G4cout << "Adding neutron thermal scattering elastic physics" << G4endl;
    G4VPhysicsConstructor* hThermalScatteringPhysics = new G4ThermalNeutrons(G4VModularPhysicsList::verboseLevel);
    hThermalScatteringPhysics->ConstructProcess();

    G4VPhysicsConstructor *hPhysics = new G4HadronPhysicsShielding();
    
    hPhysics->ConstructProcess();
    G4cout << "hPhysics->ConstructProcess();" << G4endl;

    // Apply RMG custom neutron capture
    // Mostly similar to examples/extended/Hadr04
    auto pManager = G4Neutron::Neutron()->GetProcessManager();    
    G4cout << "pManager " << pManager << G4endl;

    // auto processVector = pManager->GetProcessList();
    G4cout << "processVector" << G4endl;
    G4ProcessVector* processList = pManager->GetProcessList();

    G4int numberOfProcesses = pManager->GetProcessListLength();  // Anzahl der Prozesse
    G4cout << "numberOfProcesses: " << numberOfProcesses << G4endl;
    G4cout << "Registered Processes for Neutron:" << G4endl;
    for (G4int i = 0; i < numberOfProcesses; ++i) {
        G4VProcess* process = (*processList)[i];  // Zugriff auf den Prozess
        if (process) {  // Sicherstellen, dass der Prozess nicht nullptr ist
            G4cout << "Process Name: " << process->GetProcessName() << G4endl;  // Ausgabe des Namens
        }
    }

    // Find the existing neutron capture process
    auto neutronCaptureProcess = dynamic_cast<G4NeutronCaptureProcess*>(pManager->GetProcess("nCapture"));
    // G4cout << "neutronCaptureProcess: " << test<< G4endl;
    // Overwrite the old Process, keeping all of the interactions
    if (neutronCaptureProcess) {
        G4cout << "Overwriting NeutronCaptureProcess" << G4endl;
        pManager->RemoveProcess(neutronCaptureProcess);
        auto RMGProcess = new RMGNeutronCaptureProcess();
        // HP cross section data set not naturally in G4NeutronCaptureProcess
        auto dataSet = new G4ParticleHPCaptureData();
        RMGProcess->AddDataSet(dataSet);
        // Move all interactions to the new process
        for (auto& el : neutronCaptureProcess->GetHadronicInteractionList()) {
            RMGProcess->RegisterMe(el);
        }
        pManager->AddDiscreteProcess(RMGProcess);
    }
    G4cout << "Adding stopping physics" << G4endl;
    G4VPhysicsConstructor* stoppingPhysics = new G4StoppingPhysics(G4VModularPhysicsList::verboseLevel);
    stoppingPhysics->ConstructProcess();

    G4cout << "Adding ion physics" << G4endl;
    G4VPhysicsConstructor* ionPhysics = new G4IonPhysics(G4VModularPhysicsList::verboseLevel);
    ionPhysics->ConstructProcess(); 

    G4cout << "Adding radioactive decay physics" << G4endl;
    auto decay_physics = new G4DecayPhysics(G4VModularPhysicsList::verboseLevel);
    decay_physics->ConstructProcess();
    auto rad_decay_physics = new G4RadioactiveDecayPhysics(G4VModularPhysicsList::verboseLevel);
    rad_decay_physics->ConstructProcess();
    const auto the_ion_table = G4ParticleTable::GetParticleTable()->GetIonTable();
    G4cout << "Entries in ion table " << G4endl;

    G4cout << "Exiting ConstructProcess" << G4endl;
}



MyPhysicsList::~MyPhysicsList() {
}

void MyPhysicsList::DefineCommands() {
    fMessenger = std::make_unique<G4GenericMessenger>(this, "/RMG/Processes/", "Commands for controlling physics processes");


  fMessenger->DeclareProperty("UseGrabmayrsGammaCascades", fUseGrabmayrGammaCascades)
      .SetGuidance("Use custom RMGNeutronCapture to apply Grabmayrs gamma cascades.")
      .SetStates(G4State_PreInit);
}
