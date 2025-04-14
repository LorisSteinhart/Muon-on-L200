#ifndef PHYSICS_HH
#define PHYSICS_HH

#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4GenericMessenger.hh"
#include "globals.hh"

class MyPhysicsList : public G4VModularPhysicsList {
    public:
        MyPhysicsList();        
        ~MyPhysicsList();
    protected:
    void ConstructParticle() override;
    void ConstructProcess() override;

    private:
        std::unique_ptr<G4GenericMessenger> fMessenger;
        int fUseGrabmayrGammaCascades = 0;
        void DefineCommands();
};


#endif