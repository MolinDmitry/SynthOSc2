#pragma once
#include <iostream>
#include "GlassLib.hpp"

//===============================================================================================================================
//  класс библиотеки стекол ГОСТ
class GOST_GlassLib: public GlassLib{
    private:
        double min_wl;
        double max_wl;
        double base_wl;
        double d_lamda;
        double refIndModel(double base_n, double nu, double wl) const;
    public:
        GOST_GlassLib();        
        double getRefIndex(std::string glassName, double wavelength) const override;
};


GOST_GlassLib::GOST_GlassLib()
    {
        this->min_wl = 0.47999;
        this->max_wl = 0.6438469;
        this->base_wl = 0.546074;
        this->d_lamda =  this->max_wl - this->min_wl;
    }

    double GOST_GlassLib::refIndModel(double base_n, double nu, double wl) const
    {
        double delta_n = (base_n-1.0)/nu;
        double n_x = base_n-delta_n*(wl-this->base_wl)/this->d_lamda;
        return n_x;    
    }
   

    double GOST_GlassLib::getRefIndex(std::string glassName, double wavelength) const
    {
        double refInd = -1;
        if (glassName=="Air")   refInd = this->refIndModel(1.0, 1.0, wavelength);
        if (glassName=="K8")    refInd = this->refIndModel(1.5183, 63.83, wavelength);
        if (glassName=="LK6")   refInd = this->refIndModel(1.4721, 66.68, wavelength);
        if (glassName=="LK7")   refInd = this->refIndModel(1.4846, 66.20, wavelength);
        if (glassName=="BK6")   refInd = this->refIndModel(1.5421, 59.38, wavelength);
        if (glassName=="BK8")   refInd = this->refIndModel(1.5489, 62.59, wavelength);
        if (glassName=="BK10")  refInd = this->refIndModel(1.5713, 55.79, wavelength);
        if (glassName=="TK2")   refInd = this->refIndModel(1.5749, 57.20, wavelength);
        if (glassName=="TK14")  refInd = this->refIndModel(1.6155, 60.34, wavelength);
        if (glassName=="TK16")  refInd = this->refIndModel(1.6152, 58.09, wavelength);
        if (glassName=="TK20")  refInd = this->refIndModel(1.6247, 56.43, wavelength);
        if (glassName=="TK21")  refInd = this->refIndModel(1.6600, 50.81, wavelength);
        if (glassName=="TK23")  refInd = this->refIndModel(1.5915, 61.98, wavelength);
        if (glassName=="KF4")   refInd = this->refIndModel(1.5203, 58.72, wavelength);
        if (glassName=="BF12")  refInd = this->refIndModel(1.6298, 38.83, wavelength);
        if (glassName=="BF16")  refInd = this->refIndModel(1.6744, 47.00, wavelength);
        if (glassName=="BF24")  refInd = this->refIndModel(1.6386, 36.49, wavelength);
        if (glassName=="LF5")   refInd = this->refIndModel(1.5783, 41.04, wavelength);
        if (glassName=="F1")    refInd = this->refIndModel(1.6169, 36.70, wavelength);
        if (glassName=="F6")    refInd = this->refIndModel(1.6070, 37.68, wavelength);
        if (glassName=="TF1")   refInd = this->refIndModel(1.6522, 33.62, wavelength);
        if (glassName=="TF3")   refInd = this->refIndModel(1.7232, 29.29, wavelength);
        if (glassName=="TF5")   refInd = this->refIndModel(1.7617, 27.32, wavelength);

        if (refInd < 0)
            throw "Glass is missing in library";
        else return refInd;
    }