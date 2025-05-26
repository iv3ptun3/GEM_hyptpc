// This is the code to gett gain of GEM, example code modified 
// by HeeJeong BYEON, Kyungpook National University
// contact: gos1231@knu.ac.kr

#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TH1F.h>

#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/AvalancheMC.hh"
#include "Garfield/ComponentElmer.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/ViewField.hh"
#include "Garfield/ViewFEMesh.hh"
#include "Garfield/ViewDrift.hh"


using namespace Garfield;

int main(int argc, char *argv[]) {

     TApplication app("app", &argc, argv);

    std::string filename = "root/gem_gain_output.root";

    if ( argc > 1 ){

        filename = "root/"+ std::string(argv[1])+".root";
        
    }

    // Setup the gas.
    MediumMagboltz gas("ar", 90., "ch4", 10.);
    gas.SetTemperature(293.15);
    gas.SetPressure(760.);
    constexpr double rPenning = 0.212;
    constexpr double lambdaPenning = 0.0;
    gas.EnablePenningTransfer(rPenning, lambdaPenning, "ar");

    gas.Initialise(true);
    

    // Load the field map.
    ComponentElmer fm;
    if (!fm.Initialise("elmer/mesh.header",
                        "elmer/mesh.elements",
                        "elmer/mesh.nodes",
                        "elmer/dielectrics.dat",
                        "elmer/gem.result")) {
        printf("Error: Failed to initialize ComponentElmer. Exiting...\n");
        return 1;
    }
    
    fm.EnableMirrorPeriodicityX();
    fm.EnableMirrorPeriodicityY();
    fm.SetGas(&gas);
    fm.PrintMaterials();

    double x0, y0, z0, x1, y1, z1;
    fm.GetBoundingBox(x0, y0, z0, x1, y1, z1);
    printf("z: %lf\n",z1);

    //sleep(10);

    // parameter [cm]
    constexpr double pitch = 0.014;    // GEM pitch
    constexpr double tD = 0.005;       // Dielectric thickness
    constexpr double tD1 = 0.01;       // Dielectric thickness
    constexpr double dz12 = 0.2;       // Gap between GEM1 and GEM2
    constexpr double dz23 = 0.2;       // Gap between GEM2 and GEM3
    constexpr double dzU = 0.2;        // Drift region height
    constexpr double dzPad = 0.2;     // Readout pad plane
    constexpr double tpc = 30;     // tpc x, y size

    // voltage set [V]
    constexpr double eDrift = 130.;
    constexpr double eTrans = 1995.;
    constexpr double eInduction = 2550.;
    constexpr double dvg1 = 450;
    constexpr double dvg2 = 300.;
    constexpr double dvg3 = 300.;

    constexpr double dZp = dzPad;
    
    // Create canvases for visualization
    /*
    TCanvas* cf1 = new TCanvas("cf1", "GEM1 Electric Field", 600, 600);
    TCanvas* cf2 = new TCanvas("cf2", "GEM2 Electric Field", 600, 600);
    TCanvas* cf3 = new TCanvas("cf3", "GEM3 Electric Field", 600, 600);
    TCanvas* cf5 = new TCanvas("cf5", "Potential line with GEM", 600, 600);
    */
    TCanvas* cf4 = new TCanvas("cf4", "Drift Trajectory with GEM", 600, 600);
    
    // For gain study
    TH1F* hGGain = new TH1F("hGasGain", "Gas Gain (Avalanche Size);Gain (n_e);Entries", 100000, 0, 100000);
    TH1F* hEGain = new TH1F("hEffectiveGain", "Effective Gain ;Pad Electrons;Entries", 100000, 0, 100000);

    /*
    // ======= [1] Display GEM1 Electric potential line =======
    ViewField fieldView1(&fm);
    fieldView1.SetCanvas(cf1);
    fieldView1.SetPlaneXZ();

    const double vmin1 = -dZp * eInduction - dvg3 -dvg2 - dz23 * eTrans - dz12 * eTrans - dvg1 ;
    const double vmax1 = -dZp * eInduction - dvg3 -dvg2 - dz23 * eTrans - dz12 * eTrans + dvg1 ;

    fieldView1.SetVoltageRange(vmin1, vmax1);
    fieldView1.SetArea(-0.02, dzU + dz12 + dz23 - 0.02, 0.02, dzU + dz12 + dz23 + 0.02);
    fieldView1.SetNumberOfContours(50); 
    fieldView1.PlotContour();

    // Highlight GEM1: Copper (Orange) and kapton (Light Gray)
    ViewFEMesh meshView1(&fm);
    meshView1.SetCanvas(cf1);
    meshView1.SetPlaneXZ();
    meshView1.SetFillMesh(true);
    meshView1.SetColor(1, kOrange);    // GEM1 Upper Copper
    meshView1.SetColor(2, kOrange);    // GEM1 Lower Copper
    meshView1.SetColor(3, kGray + 1);  // GEM1 Dielectric (kapton)
    meshView1.SetArea(-0.02, dzU + dz12 + dz23 - 0.02, 0.02, dzU + dz12 + dz23 + 0.02);
    meshView1.Plot();

    // ======= [2] Display GEM2 Electric Field =======
    ViewField fieldView2(&fm);
    fieldView2.SetCanvas(cf2);
    fieldView2.SetPlaneXZ();

    const double vmin2 = -dZp * eInduction - dvg3 - dz23 * eTrans - dvg2 ;
    const double vmax2 = -dZp * eInduction - dvg3 - dz23 * eTrans + dvg2 ; 
    fieldView2.SetVoltageRange(vmin2,vmax2);
    fieldView2.SetArea(-0.02, dzU + dz12 - 0.02, 0.02, dzU + dz12 + 0.02);
    fieldView2.SetNumberOfContours(50); 
    fieldView2.PlotContour();

    // Highlight GEM2: Copper (Orange) and Glass (Light Gray)
    ViewFEMesh meshView2(&fm);
    meshView2.SetCanvas(cf2);
    meshView2.SetPlaneXZ();
    meshView2.SetFillMesh(true);
    meshView2.SetColor(4, kOrange);    // GEM2 Upper Copper
    meshView2.SetColor(5, kOrange);    // GEM2 Lower Copper
    meshView2.SetColor(6, kGray + 1);  // GEM2 Dielectric (Glass)
    meshView2.SetArea(-0.02, dzU + dz12 - 0.02, 0.02, dzU + dz12 + 0.02);
    meshView2.Plot();

    // ======= [3] Display GEM3 Electric Field =======
    ViewField fieldView3(&fm);
    fieldView3.SetCanvas(cf3);
    fieldView3.SetPlaneXZ();

    const double vmin3 = -dZp * eInduction -  dvg3 ;
    const double vmax3 = -dZp * eInduction +  dvg3 ;
    fieldView3.SetVoltageRange(vmin3,vmax3);
    fieldView3.SetArea(-0.02, dzU - 0.02, 0.02, dzU + 0.02);
    fieldView3.SetNumberOfContours(50); 
    fieldView3.PlotContour();

    // Highlight GEM3: Copper (Orange) and Glass (Light Gray)
    ViewFEMesh meshView3(&fm);
    meshView3.SetCanvas(cf3);
    meshView3.SetPlaneXZ();
    meshView3.SetFillMesh(true);
    meshView3.SetColor(7, kOrange);    // GEM3 Upper Copper
    meshView3.SetColor(8, kOrange);    // GEM3 Lower Copper
    meshView3.SetColor(9, kGray + 1); // GEM3 Dielectric (Glass)
    meshView3.SetArea(-0.02, dzU - 0.02, 0.02, dzU + 0.02);
    meshView3.Plot();
    */
    
    // ======= [4] Display Drift Lines with GEM =======
    Sensor sensor(&fm);
    sensor.SetArea(-tpc/2, -tpc/2, -0.05, tpc/2, tpc/2 , 0.8);

    AvalancheMicroscopic aval(&sensor);
    aval.SetCollisionSteps(1000);    // skipping plot

    AvalancheMC drift(&sensor);
    drift.SetDistanceSteps(2.e-2);

    // Enable drift trajectory visualization
    ViewDrift driftView;
    aval.EnablePlotting(&driftView);
    drift.EnablePlotting(&driftView);

    
   // Inject electron from root file information
    std::vector<int> avalList;
    std::vector<int> padList;
    double totalAval = 0;
    int padsum = 0;

    // paremeter
    double x, y, t, dx, dy, dz = 0.;
    double z = 0.7;
    double e = 0.5; //eV
    int nEntries = 10;

    for (int i = 0; i < nEntries; ++i) {


        aval.AvalancheElectron(x, y, z, t, e, dx, dy, dz);
        int ne, ni;

        aval.GetAvalancheSize(ne, ni);
        totalAval += ne;
        avalList.push_back(ne);
        double padCollected = 0;

        for (int j = 0; j < ne; ++j) {
            double x0, y0, z0, t0, e0;
            double x1, y1, z1, t1, e1;
            int status;

            aval.GetElectronEndpoint(j, x0, y0, z0, t0, e0, x1, y1, z1, t1, e1, status);
            if (z1 <= 0.02) { 
                padCollected++;
                padsum++;
            }
            
        }

        //printf("event %d/%d : %d\n",i+1,nEntries, ne);
        padList.push_back(padCollected);
        //printf("pad readout : %lf\n", padCollected);
        hGGain->Fill(ne);
        hEGain->Fill(padCollected);

    }

    double mean = totalAval/nEntries;
    double mean_pad = padsum/nEntries;

    double sq_sum = 0.0;
    for (const auto& val : avalList) {
        sq_sum += (val - mean) * (val - mean);
    }
    double stddev = std::sqrt(sq_sum / nEntries);

    double sq_pad_sum = 0.0;
    for (const auto& val : padList) {
        sq_pad_sum += (val - mean_pad) * (val - mean_pad);
    }
    double pad_stddev = std::sqrt(sq_pad_sum / nEntries);

    printf("Simulation done!\n");
    printf("Avalanvhe Electron Mean : %lf\n", mean);
    printf("Avalanvhe Electron Stddev : %lf\n", stddev);

    printf("Pad Collected Electron Mean : %lf\n", mean_pad);
    printf("Pad Collected Electron Stddev : %lf\n", pad_stddev);

    TFile* outFile = new TFile(filename.c_str(), "RECREATE");
    hGGain->Write();
    hEGain->Write();
    outFile->Close();
    printf("ROOT histograms saved to gem_gain_output.root\n");


    // Display drift lines **with GEM layers visible**
    /*
    ViewFEMesh meshView;
    meshView.SetCanvas(cf4);
    cf4->SetLeftMargin(0.16);
    meshView.SetComponent(&fm);
    meshView.SetPlane(0, -1, 0, 0, 0, 0);   // XZ plane
    meshView.SetArea(-0.4, 0, 0.4, dzU + dz12 + dz23 + 0.2);

    meshView.SetFillMesh(true);
    meshView.SetColor(3, kGray + 1);   
    meshView.SetColor(6, kGray + 1);   
    meshView.SetColor(9, kGray + 1);    
    meshView.EnableAxes();

    meshView.SetViewDrift(&driftView);

    constexpr bool twod = true;
    meshView.Plot(twod);
  */
    

/*
// ======= [5] Display Potential Lines with GEM =======
    ViewField fieldView5(&fm);
    fieldView5.SetCanvas(cf5);
    fieldView5.SetPlaneXZ();
    fieldView5.SetNumberOfContours(100);  
    fieldView5.SetVoltageRange(-2800., 0.);  // Show full potential range
    fieldView5.SetArea(-0.1, 0.0, 0.1, dzU + dz12 + dz23 + 0.2);
    fieldView5.PlotContour();

    // Overlay GEM mesh for reference
    ViewFEMesh meshView5(&fm);
    meshView5.SetCanvas(cf5);
    meshView5.SetPlaneXZ();
    meshView5.SetFillMesh(true);
    meshView5.SetComponent(&fm);
    meshView5.SetColor(3, kGray + 1);   // GEM1
    meshView5.SetColor(6, kGray + 1);   // GEM2 
    meshView5.SetColor(9, kGray + 1);   // GEM3
    meshView5.SetArea(-0.1, 0.0, 0.1, dzU + dz12 + dz23 + 0.2);
    meshView5.Plot();
*/

    app.Run();

} 
