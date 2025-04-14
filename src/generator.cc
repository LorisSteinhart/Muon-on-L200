#include "generator.hh"
#include "construction.hh"


MyPrimaryGenerator::MyPrimaryGenerator()
{
  // will be overwritten anyway
  fParticleGun = new G4ParticleGun(1);
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition *particle = particleTable->FindParticle("mu-");

  G4ThreeVector pos(0.,0.,-20.*km);
  G4ThreeVector mom(0.,0.,1.);

  fParticleGun->SetParticlePosition(pos);
  fParticleGun->SetParticleMomentumDirection(mom);
  fParticleGun->SetParticleMomentum(100.*GeV);
  fParticleGun->SetParticleDefinition(particle);
  
  fFileName = "/home/loris/sim/l200_withoutOpticalProcesses_Grabmayr/build/combined_file_org.dat";
  OpenFile();

}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
  delete fParticleGun;
  // delete fMessenger;
  if(fInputFile.is_open())
    fInputFile.close();
}

void MyPrimaryGenerator::OpenFile()
{
  G4cout << "Attempting to open file: " << fFileName << G4endl;
  fInputFile.open(fFileName, std::ifstream::in);
  if(!(fInputFile.is_open()))
  {  
     G4cerr << "Musung file not valid! Name: " << fFileName << G4endl;
  }
  else
  {
     G4cout << "Successfully opened file: " << fFileName << G4endl;
  }
}


void MyPrimaryGenerator::ChangeFileName(G4String newFile)
{
  if(fFileName != newFile)  // check if the new file is equal to the other
  {
    if(fInputFile.is_open())
      fInputFile.close();  // close the old file
    G4cout << "opening file: " << newFile << G4endl;
    fFileName = newFile;
    OpenFile();  // open the new one
  }
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *event)
{
  G4int    nEvent = 0;
  G4double time   = 0.0;
  G4double energy = 0.0 * MeV;
  G4double px, py, pz;
  G4double theta, phi;
  G4double x = 0, y = 0, z = 0;
  G4int    particleID = 0;
  G4double fZShift = 200.0 * cm;

  fInputFile >> nEvent >> particleID >> energy >> x >> y >> z >> px >> py >> pz; 


  G4double particle_time = time * s;
  energy                 = energy * GeV;
  theta                  = theta * rad;
  phi                    = phi * rad;
  x                      = x * cm;
  y                      = y * cm;
  z                      = (z * cm);

  G4ParticleTable* theParticleTable = G4ParticleTable::GetParticleTable();

  G4String particleName = " ";

  if(particleID == 10)
    particleName = "mu+";
  else
    particleName = "mu-";

  G4ThreeVector momentumDir(px, py, pz);

  fParticleGun->SetParticleMomentumDirection(momentumDir);

  fParticleGun->SetParticleEnergy(energy);

  fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));

  fParticleGun->GeneratePrimaryVertex(event);
}

