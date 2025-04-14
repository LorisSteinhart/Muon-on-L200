#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ChargedGeantino.hh"
#include "Randomize.hh"



class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
    MyPrimaryGenerator();
    ~MyPrimaryGenerator();

    virtual void GeneratePrimaries(G4Event*);
    void OpenFile();
    void ChangeFileName(G4String newFile);

private:
    G4ParticleGun   *fParticleGun;
    G4String        fFileName;
    std::ifstream      fInputFile;
};

#endif