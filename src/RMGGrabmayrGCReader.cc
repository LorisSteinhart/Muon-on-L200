#include "RMGGrabmayrGCReader.hh"

#include "G4Tokenizer.hh"
#include "Randomize.hh"

// #include "RMGLog.hh"


G4ThreadLocal RMGGrabmayrGCReader* RMGGrabmayrGCReader::instance = nullptr;

RMGGrabmayrGCReader* RMGGrabmayrGCReader::GetInstance() {
  if (instance == nullptr) { instance = new RMGGrabmayrGCReader(); }
  return instance;
}

RMGGrabmayrGCReader::RMGGrabmayrGCReader() { 
  G4cout << "Entering DefineCommads of MNGGrabmayrReader" << G4endl;
  DefineCommands(); 
  }

RMGGrabmayrGCReader::~RMGGrabmayrGCReader() { CloseFiles(); }

void RMGGrabmayrGCReader::CloseFiles() {
  // RMGLog::Out(RMGLog::detail, "Closing gamma cascade files");
  G4cout << "Closing gamma cascade files" << G4endl;
  for (const auto& el : fCascadeFiles) {
    if (el.second->is_open()) { el.second->close(); }
  }
}

// Returns true if there exists a cascade file for the Isotope Z, A
G4bool RMGGrabmayrGCReader::IsApplicable(G4int z, G4int a) {
  std::pair<G4int, G4int> key = std::make_pair(z, a);
  auto it = fCascadeFiles.find(key);
  return it != fCascadeFiles.end();
}

// Returns the next cascade for the Isotope Z, A.
GammaCascadeLine RMGGrabmayrGCReader::GetNextEntry(G4int z, G4int a) {
  // Find the corresponding file
  std::pair<G4int, G4int> key = std::make_pair(z, a);
  auto it = fCascadeFiles.find(key);
  if (it == fCascadeFiles.end())
    // RMGLog::OutFormat(RMGLog::fatal, "Isotope Z: {} A: {} does not exist.", z, a);
    G4cerr << "Isotope Z: " << z << "  A: " << a << " does not exist." << G4endl;
  // read next line from file
  std::string line;
  do {
    if (!std::getline(*(it->second), line)) {
      // if end-of-file is reached, reset file and read first line
      // RMGLog::Out(RMGLog::detail, "Gamma cascade file EOF reached, re-opening the file");
      G4cout << "Gamma cascade file EOF reached, re-opening the file" << G4endl;
      it->second->clear();                 // clear EOF flag
      it->second->seekg(0, std::ios::beg); // move to beginning of file
      if (!std::getline(*(it->second), line)) {
        // RMGLog::Out(RMGLog::fatal, "Failed to read next line after re-opening the file. Exit!");
        G4cerr << "Failed to read next line after re-opening the file. Exit!" << G4endl;
      }
    }
  } while (line[0] == '%' || (line.find("version") !=
                                 std::string::npos)); // This could be outsourced to SetStartLocation

  // parse line and return as struct
  GammaCascadeLine gamma_cascade;
  std::istringstream iss(line);
  iss >> gamma_cascade.en >> gamma_cascade.ex >> gamma_cascade.m >> gamma_cascade.em;
  gamma_cascade.eg.reserve(gamma_cascade.m);
  int eg_value;
  for (int i = 0; i < gamma_cascade.m; i++) {
    if (!(iss >> eg_value)) {
      // RMGLog::Out(RMGLog::fatal, "Failed to read gamma energy from file. Exit!");
      G4cerr << "Failed to read gamma energy from file. Exit!" << G4endl;
    }
    gamma_cascade.eg.push_back(eg_value);
  }
  return gamma_cascade;
}


void RMGGrabmayrGCReader::SetStartLocation(std::ifstream& file) {
  if (!file.is_open())
    // RMGLog::Out(RMGLog::fatal, "The file is not open to set start location! Exit.");
    G4cerr << "Input for the function FindALogicalVolume() is NOT a physicalVolume!" << G4endl;
  file.clear();                 // clear EOF flag
  file.seekg(0, std::ios::beg); // move to beginning of file
  // Skip Header
  std::string line;
  int header_length = 0;
  do {
    std::getline(file, line);
    header_length++;
  } while (line[0] == '%' || (line.find("version") != std::string::npos));

  // In case the Random start location macro is set
  if (fGammaCascadeRandomStartLocation) {
    int n_entries_in_file = 0;
    // Seems excessiv to read through the entire file, there has to be a quicker way?
    while (std::getline(file, line)) n_entries_in_file++;

    file.clear();                 // clear EOF flag
    file.seekg(0, std::ios::beg); // move to beginning of file

    int start_location = (int)(n_entries_in_file * G4UniformRand() + header_length);

    // RMGLog::Out(RMGLog::detail, "Random start location: ", start_location);
    G4cout << "Gamma cascade file EOF reached, re-opening the file" << G4endl;
    for (int j = 0; j < start_location; j++) std::getline(file, line);
  }
}

void RMGGrabmayrGCReader::SetGammaCascadeFile(const G4int z, const G4int a, const G4String file_name) {

  // RMGLog::Out(RMGLog::detail, "Opening file ", file_name);
  G4cout << "Opening file " << file_name << G4endl;
  std::unique_ptr<std::ifstream> file = std::make_unique<std::ifstream>(file_name);

  if (z == 0 || a == 0)
    // RMGLog::OutFormat(RMGLog::fatal, "Isotope Z: {} A: {} does not exist.", z, a);   
    G4cerr << "Isotope Z: " << z << "  A: " << a << " does not exist." << G4endl;
  if (!file || !file->is_open())
    // RMGLog::Out(RMGLog::fatal, "Gamma cascade file: " + file_name + " not found! Exit.");
    G4cerr << "Gamma cascade file: " << file_name << " not found! Exit." << G4endl;

  SetStartLocation(*file);

  fCascadeFiles.insert({{z, a}, std::move(file)});
}

void RMGGrabmayrGCReader::RandomizeFiles() {
  // RMGLog::Out(RMGLog::detail, "(Un)-Randomizing start locations");
  G4cout << "(Un)-Randomizing start locations" << G4endl;
  for (auto& el : fCascadeFiles) { SetStartLocation(*el.second); }
}

void RMGGrabmayrGCReader::SetGammaCascadeRandomStartLocation(const int answer) {
  fGammaCascadeRandomStartLocation = answer;
  // RMGLog::Out(RMGLog::detail, "setting fGammaCascadeRandomStartLocation to: ", fGammaCascadeRandomStartLocation);
  G4cout << "setting fGammaCascadeRandomStartLocation to: " << fGammaCascadeRandomStartLocation << G4endl;
  RandomizeFiles();
}

void RMGGrabmayrGCReader::DefineCommands() {
  G4cout << "EnNtering Define Commands RMGGrabmayrGCReader" << G4endl;
  fGenericMessenger = std::make_unique<G4GenericMessenger>(this, "/RMG/GrabmayrGammaCascades/",
      "Control Peters gamma cascade model");

  fGenericMessenger
      ->DeclareMethod("SetGammaCascadeRandomStartLocation",
          &RMGGrabmayrGCReader::SetGammaCascadeRandomStartLocation)
      .SetGuidance("Set the whether the start location in the gamma cascade file is random or not")
      .SetGuidance("0 = don't")
      .SetGuidance("1 = do")
      .SetCandidates("0 1")
      .SetDefaultValue("0")
      .SetStates(G4State_PreInit, G4State_Idle);

  // SetGammaCascadeFile cannot be defined with the G4GenericMessenger (it has too many parameters).
  fUIMessenger = std::make_unique<GCMessenger>(this);
}


RMGGrabmayrGCReader::GCMessenger::GCMessenger(RMGGrabmayrGCReader* reader) : fReader(reader) {
  fGammaFileCmd = new G4UIcommand("/RMG/GrabmayrGammaCascades/SetGammaCascadeFile", this);
  fGammaFileCmd->SetGuidance("Set a gamma cascade file for neutron capture on a specified isotope");

  auto p_Z = new G4UIparameter("Z", 'i', false);
  p_Z->SetGuidance("Z of isotope");
  fGammaFileCmd->SetParameter(p_Z);

  auto p_A = new G4UIparameter("A", 'i', false);
  p_A->SetGuidance("A of isotope");
  fGammaFileCmd->SetParameter(p_A);

  auto p_file = new G4UIparameter("file", 's', false);
  p_file->SetGuidance("/path/to/file of gamma cascade");
  fGammaFileCmd->SetParameter(p_file);

  fGammaFileCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

RMGGrabmayrGCReader::GCMessenger::~GCMessenger() { delete fGammaFileCmd; }

void RMGGrabmayrGCReader::GCMessenger::SetNewValue(G4UIcommand* command, G4String newValues) {
  if (command == fGammaFileCmd) GammaFileCmd(newValues);
}

void RMGGrabmayrGCReader::GCMessenger::GammaFileCmd(const std::string& parameters) {
  G4Tokenizer next(parameters);

  auto Z = std::stoi(next());
  auto A = std::stoi(next());
  auto file = next();

  fReader->SetGammaCascadeFile(Z, A, file);
}
