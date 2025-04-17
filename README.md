# Gd Neutron Flux Simulation for GERDA (PGT Setup)

This repository contains the full simulation framework used in the study of neutron flux in the Post-GERDA-Test-Setup (PGT), focusing on neutron capture events on natural Gadolinium within a high-purity germanium detector environment.

## Overview

The simulation is based on a GEANT4-based geometry. It includes a detailed model of the modified L200 setup used as a stand-in for the original GERDA detector configuration. A dedicated Gd-loaded string is placed at the center to study the capture of thermalized neutrons and their impact on detector backgrounds.

The simulation allows for:

- Muon-induced neutron generation using MUSUN files
- Detailed tracking of neutron interactions and capture processes
- Energy deposition analysis in HPGe detectors
- Time-resolved coincidence analysis between muon veto and Ge events
- Estimation of signal-to-background ratios and neutron flux

## How to Run

### Prerequisites

- CMake ≥ 3.16
- GEANT4 (with GDML support)
- Python 3 (for post-processing)
- MUSUN file (https://doi.org/10.1016/j.cpc.2008.10.013)
- Gamma cascades in gadolinium isotopes from Peter Gramayr (https://zenodo.org/records/7458654)

### Build the simulation

```bash
mkdir build && cd build
cmake ..
make -j
```

### Run the simulation
```bash
./sim run.mac
```

### Output csv files of the simulation
- output0_gerdaSetup_16x16x20_musun_nt_GadoliniumHits.csv
- output0_gerdaSetup_16x16x20_musun_nt_sensitiveDetectorHits.csv
