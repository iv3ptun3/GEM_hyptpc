# GEM_hyptpc
triple GEM simulation for kek server

you have to install gmsh & garfield

1. git clone https://github.com/iv3ptun3/GEM_hyptpc.git
2. cd GEM_hyptpc
3. gmsh -3 -order 2 gem.geo
4. ElmerGrid 14 2 gem.msh -autoclean
5. ElmerSolver gem.sif
6. mkdir build
7. cmake ../
8. make
9. chmod +x run.sh
10. ./gem or ./run.sh
