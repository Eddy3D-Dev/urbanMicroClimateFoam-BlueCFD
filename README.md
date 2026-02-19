# urbanMicroclimateFoam

An open-source solver for coupled physical processes modeling urban microclimate based on OpenFOAM. Based on original [contributions](https://gitlab.ethz.ch/openfoam-cbp/solvers/urbanmicroclimatefoam) from ETH researchers.

> urbanMicroclimateFoam is a multi-region solver consisting of an air subdomain together with subdomains for porous urban building materials. A computational fluid dynamics (CFD) model solves the turbulent, convective air flow and heat and moisture transport in the air subdomain. A coupled heat and moisture (HAM) transport model solves the absorption, transport and storage of heat and moisture in the porous building materials. A radiation model determines the net longwave and shortwave radiative heat fluxes for each surface using a view factor approach.

---

## Installation with `BlueCFD-Core-2020-1`

Execute the following commands in the BlueCFD terminal:

```bash
git clone --branch of-org_v8.0-bluecfd https://github.com/Eddy3D-Dev/urbanMicroClimateFoam-BlueCFD
cd urbanMicroClimateFoam-BlueCFD
git checkout 98d62585a7f9f97c7bc6a3a5b6e7e00e70819494
./Allwclean && ./Allwmake
```

A successful compilation will end with a message similar to:

```
C:/PROGRA~1/BLUECF~1/ofuser-of8/platforms/mingw_w64GccDPInt32Opt/bin/urbanMicroclimateFoam.pdb: cannot load PDB helper DLL
Error occurred with cv2pdb, have stripped binary as a workaround.
```

This is expected — the solver has compiled correctly.
