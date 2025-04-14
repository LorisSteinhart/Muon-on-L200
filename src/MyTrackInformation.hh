#ifndef MYTRACKINFORMATION_HH
#define MYTRACKINFORMATION_HH

#include "G4VUserTrackInformation.hh"
#include "G4Track.hh"

class MyTrackInformation : public G4VUserTrackInformation {
public:
    MyTrackInformation(int marker = 0) : generationMarker(marker) {}
    MyTrackInformation(const MyTrackInformation* info) : G4VUserTrackInformation() {
        generationMarker = info->generationMarker;
    }

    void SetGenerationMarker(int marker) { generationMarker = marker; }
    int GetGenerationMarker() const { return generationMarker; }

private:
    int generationMarker;
};

#endif
