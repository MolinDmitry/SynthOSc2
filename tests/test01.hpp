#pragma once

#include <iostream>
#include "../inc/Ray.hpp"
#include "../inc/GOST_GlassLib.hpp"
#include "../inc/OptSurf.hpp"
#include "../inc/OptSys2.hpp"

#define USE_THIN_COMP 0
namespace test01 {

    void log(std::string message){
        std::cout << message << std::endl;
    }

    void test01(){
    test01::log("Test 1");
    double wavelengthArray[10];
    wavelengthArray[0] = 0.480;
    wavelengthArray[1] = 0.546;
    wavelengthArray[2] = 0.644;

    GOST_GlassLib glassLib = GOST_GlassLib();
    OptSystem optSys = OptSystem(20,5, wavelengthArray, 3, &glassLib);
    OptSurf optSurf = OptSurf(100,10,"K8");
    optSys.appendSurface(&optSurf);
    optSurf.setR(-100);
    optSurf.setT(0);
    optSurf.setGlass("Air");
    optSys.appendSurface(&optSurf);
    test01::log("Optical system:");
    optSys.printOptSystem();
    
    OptSystem clonedOS = optSys.cloneOptSystem();
    test01::log("Cloned optical system:");
    clonedOS.printOptSystem();

    Ray inputRay = Ray(1e-7, 0);
    Ray outputRay = optSys.passRayThroughtOS(inputRay, 1, USE_THIN_COMP);
    log("outRay is exist: " + std::to_string(outputRay.isExist()));
    log("h: " + std::to_string(outputRay.getH()));    
    log("w: " + std::to_string(outputRay.getW()));
    double f = optSys.findRearFocalLength(1, USE_THIN_COMP);
    log("f': " + std::to_string(f));
    inputRay.setH(10);
    outputRay = optSys.passRayThroughtOS(inputRay, 1, USE_THIN_COMP);
    log("outRay is exist: " + std::to_string(outputRay.isExist()));
    log("h: " + std::to_string(outputRay.getH()));    
    log("w: " + std::to_string(outputRay.getW()));
    double fh = -1*inputRay.getH()/tan(outputRay.getW());
    log("f': " + std::to_string(fh));
    double dx = f - fh;
    log("dx: " + std::to_string(dx));
    double dy = dx*tan(outputRay.getW());
    log("dy: " + std::to_string(dy));
    log("");
    double geo_spot_radius = optSys.findSpotRadius(1, 0, false, USE_THIN_COMP, 10);
    double rms_spot_radius = optSys.findSpotRadius(1, 0 , true, USE_THIN_COMP, 10);
    log("GEO spot radius: " + std::to_string(geo_spot_radius));
    log("RMS spot radius: " + std::to_string(rms_spot_radius)); 

}
}


