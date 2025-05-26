# GEM_hyptpc
**triple GEM simulation for kek server**
by HeeJeong BYEON, Kyungpook National University

---
## Reauirements
- gmsh
- ROOT
- Garfield

---
## Download
1. git clone https://github.com/iv3ptun3/GEM_hyptpc.git
2. cd GEM_hyptpc

---
## make mesh file
1. set parameter at gem.geo
2. gmsh -3 -order 2 gem.geo
3. ElmerGrid 14 2 gem.msh -autoclean

---
## Solve ElectricField
1. change permittivity at gem/dielectrics.dat
2. set Voltage at gem.sif
3. ElmerSolver gem.sif

---
## build and run
1. mkdir build
2. cmake ../
3. make
4. chmod +x run.sh
5. ./gem or ./run.sh
