#pragma once
#include <iostream>

/**
 * @brief
 * Класс описания оптического луча
 *  */


 //===============================================================================================================================
// класс луча
class Ray{
    private:
        double h;
        double w;
    public:
        Ray();
        Ray(double h, double w);
        double getH();
        double getW();
        void printRay();
};

    Ray::Ray(){
        this->h = 0;
        this->w = 0;
    }
    
    Ray::Ray(double h, double w){
        this->h = h;
        this->w = w;
    }

    double Ray::getH(){
        return this->h;
    }

    double Ray::getW(){
        return this->w;
    }


    
    void Ray::printRay(){
        std::cout << "Ray height: " << this->getH() << "; Ray angle:" << this->getW() << std::endl;
    }


