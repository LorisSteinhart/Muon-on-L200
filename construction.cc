#include "construction.hh"
#include "G4GDMLParser.hh"
#include "G4SDManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ScoringManager.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSFlatSurfaceFlux.hh"
#include "G4SDParticleFilter.hh"
#include "G4VSensitiveDetector.hh"
#include <string> // Für die Verwendung von std::string Methoden

MyDetectorConstruction::MyDetectorConstruction() //Constructor
{}

MyDetectorConstruction::~MyDetectorConstruction() //Destructor
{}

// Finds a specific logical volume in a physical volume 
G4LogicalVolume* FindALogicalVolume(G4String nameOfLogicalVolume, G4VPhysicalVolume* physicalVolume){
    if (!physicalVolume){        
        G4cerr << "Input for the function FindALogicalVolume() is NOT a physicalVolume!" << G4endl;
        return nullptr;
    } 

    G4LogicalVolume* logicalVolume = physicalVolume->GetLogicalVolume();
    if (!logicalVolume){
        G4cerr << "Input for the function FindALogicalVolume() has NO a logicalVolume!" << G4endl;
        return nullptr;
    }

    // Durchlaufe alle Kinder der aktuellen physischen Volumes
    for (G4int i = 0; i < logicalVolume->GetNoDaughters(); ++i) {
        G4VPhysicalVolume* childVolume = logicalVolume->GetDaughter(i);
        // G4cout << childVolume->GetName() << G4endl;
        // if (childVolume->GetName() == nameOfLogicalVolume) {
        if (G4StrUtil::contains(childVolume->GetName(), nameOfLogicalVolume)) {            
            return childVolume->GetLogicalVolume();
        }   
        // Rekursiver Aufruf für Kinder-Volumen
        G4LogicalVolume* result = FindALogicalVolume(nameOfLogicalVolume, childVolume);
        if (result){
            G4cout << "Gadolinium logical volume succesfully found!" << G4endl;
            return result;
        }         
    }
    return nullptr;
}

void PrintAllVolumes(G4VPhysicalVolume* physicalVolume, int depth = 0) {
    if (!physicalVolume) {
        G4cerr << "Null physical volume encountered!" << G4endl;
        return;
    }

    // Einrückung basierend auf der Tiefe im Baum für bessere Lesbarkeit
    G4String indentation(depth * 2, ' ');

    // Namen des aktuellen physikalischen Volumens drucken
    G4String physicalName = physicalVolume->GetName();
    G4LogicalVolume* logicalVolume = physicalVolume->GetLogicalVolume();
    G4String logicalName = logicalVolume->GetName();
    if (G4StrUtil::contains(logicalName, "sipm")) {
        G4cout << indentation << "Physical Volume: " << physicalName 
        << " | Logical Volume: " << logicalName << G4endl;
    }

    // Durchlaufe alle physikalischen Tochtervolumina des aktuellen logischen Volumens
    for (G4int i = 0; i < logicalVolume->GetNoDaughters(); ++i) {
        G4VPhysicalVolume* daughterVolume = logicalVolume->GetDaughter(i);

        // Rekursiver Aufruf für jede Tochter
        PrintAllVolumes(daughterVolume, depth + 1);
    }
}

/*Creates Physical Volume, returns Pointer to said volume*/
G4VPhysicalVolume *MyDetectorConstruction::Construct(){
    G4GDMLParser parser;
    parser.SetOverlapCheck(true);
    parser.Read("gdmlFiles/l200_gerdaSetup.gdml");    
    // parser.Read("l200.gdml");    
    G4VPhysicalVolume* worldVolume = parser.GetWorldVolume(); 
    
    // G4VPhysicalVolume* cryostatLogical = worldVolume->GetLogicalVolume()->GetDaughter(1);
    // G4VPhysicalVolume* lAr = cryostatLogical->GetLogicalVolume()->GetDaughter(0);

{
    //     G4cout << "GetNoDaughters: " << lAr->GetLogicalVolume()->GetNoDaughters() << G4endl;
    //     G4cout << "GetDaughter 1: " << lAr->GetLogicalVolume()->GetDaughter(0) << G4endl;
    //     G4cout << "GetDaughter 2: " << lAr->GetLogicalVolume()->GetDaughter(1) << G4endl;
    //     G4cout << "GetDaughterName 1: " << lAr->GetLogicalVolume()->GetDaughter(0)->GetName() << G4endl;
    //     G4cout << "GetDaughterName 2: " << lAr->GetLogicalVolume()->GetDaughter(1)->GetName() << G4endl;
    //     // G4cout << "cryostatLogical GetDaughter: " << cryostatLogical->GetLogicalVolume()->GetDaughter(0) << G4endl;
    
    // G4int counter = 0;
    // G4VPhysicalVolume* wslr_ttx = lAr->GetLogicalVolume()->GetDaughter(0)->GetLogicalVolume()->GetDaughter(0);
    // G4cout << "GetDaughterName 1: " << wslr_ttx->GetName() << G4endl;
    // G4cout << "GetNoDaughters: " << wslr_ttx->GetLogicalVolume()->GetNoDaughters() << G4endl;
    // G4cout << "GetDaughterName 1: " << wslr_ttx->GetLogicalVolume()->GetDaughter(0)->GetName() << G4endl;


    // for (G4int i = 0; i < wslr_ttx->GetLogicalVolume()->GetNoDaughters(); ++i) {
        
    //     G4VPhysicalVolume* childVolume = wslr_ttx->GetLogicalVolume()->GetDaughter(i);
    //     G4cout << "child: " << childVolume->GetName() << G4endl;
    //     counter = counter+1;
    //     for (G4int j = 0; j < childVolume->GetLogicalVolume()->GetNoDaughters(); ++j) {
    //         G4VPhysicalVolume* childchildVolume = childVolume->GetLogicalVolume()->GetDaughter(j);
    //         G4cout << "childchild: " << childchildVolume->GetName() << G4endl;

    //         counter = counter+1;
    //     }
    // }
    // G4cout << "counter: " << counter << G4endl;

    // PrintVolumeNames(worldVolume);

    // G4VPhysicalVolume* worldVolume = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();
    // PrintAllVolumes(worldVolume);  
} 

    // G4cout << "FindGadoliniumLogicalVolume: "<< FindGadoliniumLogicalVolume(cryostatLogical)->GetName() << G4endl;
    // gadoliniumLogical = FindALogicalVolume("simp", worldVolume);


    return worldVolume;
}

void MyDetectorConstruction::ConstructSDandField() {
    // Sensitive Detector erstellen
    // G4GDMLParser parser;
    // // parser.Read("ball.gdml");
    // parser.Read("l200.gdml");
    // G4VPhysicalVolume* worldVolume = parser.GetWorldVolume();
    // G4LogicalVolume* gadoliniumLogical = nullptr;    
    // G4VPhysicalVolume* cryostatLogical = worldVolume->GetLogicalVolume()->GetDaughter(1);
    // G4VPhysicalVolume* lAr = cryostatLogical->GetLogicalVolume()->GetDaughter(0);

    MySensitiveDetector *GeSD = new MySensitiveDetector("GeSD");
    MySensitiveDetector *GdSD = new MySensitiveDetector("GdSD");
    MySensitiveDetector *ArSD = new MySensitiveDetector("ArSD");    
    MySensitiveDetector *spSD = new MySensitiveDetector("spSD");
    // Hole alle logischen Volumina
    const G4LogicalVolumeStore* logicalVolumeStore = G4LogicalVolumeStore::GetInstance();    

    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    sdManager->AddNewDetector(GeSD);   // Registriere den ersten Detector
    sdManager->AddNewDetector(GdSD);   // Registriere den zweiten Detector
    sdManager->AddNewDetector(ArSD);   // Registriere den drittem Detector   
    sdManager->AddNewDetector(spSD);   // Registriere den vierten Detector

    for (auto logicalVolume : *logicalVolumeStore) {
        // Verwende die Standardmethode find, um zu prüfen, ob "sipm" im Namen des logischen Volumens enthalten ist
        
        G4cout << "Volume: " << logicalVolume->GetName() <<  G4endl;

        if (logicalVolume->GetMaterial()->GetName().find("EnrichedGermanium") == 0) {
            // Setze das Volumen als Sensitive Detector
            logicalVolume->SetSensitiveDetector(GeSD);
            G4cout << "Set " << logicalVolume->GetName() << " as a sensitive detector (GeSD)." << G4endl;
        }
        if (logicalVolume->GetName().find("gadolinium") != std::string::npos) {
            // Setze das Volumen als Sensitive Detector für Gadolinium
            logicalVolume->SetSensitiveDetector(GdSD);
            G4cout << "Set " << logicalVolume->GetName() << " as a sensitive detector (GdSD)." << G4endl;
        }
        // if (logicalVolume->GetMaterial()->GetName().find("metal_steel") != std::string::npos) {
        //     // Setze das Volumen als Sensitive Detector für Gadolinium
        //     logicalVolume->SetSensitiveDetector(ArSD);
        //     G4cout << "Set " << logicalVolume->GetName() << " as a sensitive detector (ArSD)." << G4endl;
        // }
        if (logicalVolume->GetName().find("lar") != std::string::npos) {
            
            // Logge die Anzahl der Töchter des LogicalVolume
            G4cout <<" lar gefunden " << logicalVolume->GetNoDaughters() << G4endl;

            // Wenn es Töchter gibt, durchlaufe sie
            for (int i = 0; i < logicalVolume->GetNoDaughters(); ++i) {
                // Hole die Tochter-PhysicalVolume
                G4VPhysicalVolume* daughterPV = logicalVolume->GetDaughter(i);
                if (daughterPV->GetName().find("string_gerda") != std::string::npos) {
                    // Setze den Sensitive Detector für das LogicalVolume
                    daughterPV->GetLogicalVolume()->SetSensitiveDetector(spSD);
                    G4cout << "Set " << daughterPV->GetName() << " as a sensitive detector (spSD)." << G4endl;
                }
            }
        }   
    }



    // // Kugel mit Radius 20 mm
    // G4Sphere* mySphere = new G4Sphere(
    //     "myScoringSphere",  // Name
    //     0.0 * mm,           // Innerer Radius (0 = volle Kugel)
    //     20.0 * mm,          // Äußerer Radius
    //     0.0 * deg,          // Start-Phi
    //     360.0 * deg,        // Delta-Phi (komplette Kugel)
    //     0.0 * deg,          // Start-Theta
    //     180.0 * deg         // Delta-Theta (volle Kugel)
    // );

    // // Logisches Volumen für die Kugel
    // G4Material* myMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
    // G4LogicalVolume* myCellLog = new G4LogicalVolume(mySphere, myMaterial, "myDetectorLV");
    // // Kugel-Position setzen (tbr = [0, 0, 0] → keine zusätzliche Translation in lokalem Frame)
    // G4ThreeVector position = G4ThreeVector(90.0 * mm, 40.0 * mm, 1000.0 * mm);  // Beispiel: Verschiebung in X, Y, Z

    // G4GDMLParser parser;
    // parser.Read("gdmlFiles/l200_gerdaSetup_x-90_y-40_z1000.gdml");  
    // G4VPhysicalVolume* worldPhys = parser.GetWorldVolume();
    // G4LogicalVolume* worldLog = worldPhys->GetLogicalVolume();

    // // Kugel im Welt-Volumen platzieren (worldLog muss existieren)
    // new G4PVPlacement(0, position, myCellLog, "myScoringSphere", worldLog, false, 0, true);

    // // Erstelle den MultiFunctionalDetector
    // G4MultiFunctionalDetector* myScorer = new G4MultiFunctionalDetector("myProtonScorer");

    // myCellLog->SetSensitiveDetector(myScorer);
    // G4VPrimitiveScorer* neutronSurfFlux = new G4PSFlatSurfaceFlux("nSurfFlux", 2, 0);
    // G4VSDFilter* neutronFilter = new G4SDParticleFilter("neutronFilter");
    // static_cast<G4SDParticleFilter*>(neutronFilter)->add("neutron");
    // neutronSurfFlux->SetFilter(neutronFilter);
    // myScorer->RegisterPrimitive(neutronSurfFlux);

}
