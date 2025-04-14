#include "action.hh"
#include "RMGGrabmayrGCReader.hh"

MyActionInitialization::MyActionInitialization()
{}

MyActionInitialization::~MyActionInitialization()
{}

void MyActionInitialization::Build() const{
    MyPrimaryGenerator *generator = new MyPrimaryGenerator();
    SetUserAction(generator);

    MyRunAction *runAction = new MyRunAction();
    SetUserAction(runAction);
    RMGGrabmayrGCReader::GetInstance();
    
    MyEventAction *eventAction = new MyEventAction(runAction);
    SetUserAction(eventAction);

    MySteppingAction *steppingAction = new MySteppingAction(eventAction);
    SetUserAction(steppingAction);
}