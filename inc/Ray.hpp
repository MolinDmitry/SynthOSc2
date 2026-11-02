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
        int rayIsExist; // флаг существования луча
    public:
        Ray();
        Ray(double h, double w);
        double getH();
        double getW();
        int isExist();
        void setH(double h);
        void setW(double w);
        void setExist();
        void resetExist();
        void printRay();
};

    Ray::Ray(){
        this->h = 0;
        this->w = 0;
        this->rayIsExist = 0;
    }
    
    Ray::Ray(double h, double w){
        this->h = h;
        this->w = w;
        this->rayIsExist = 1;
    }

    double Ray::getH(){
        return this->h;
    }

    double Ray::getW(){
        return this->w;
    }

    void Ray::setH(double h){
        this->h = h;
    }

    void Ray::setW(double w){
         this->w = w;
    }

    void Ray::setExist(){
        this->rayIsExist = 1;
    }

    void Ray::resetExist(){
        this->rayIsExist = 0;
    }


    int Ray::isExist(){
        return this->rayIsExist;
    }


    
    void Ray::printRay(){
        std::cout << "Ray height: " << this->getH() << "; Ray angle:" << this->getW() << std::endl;
    }


