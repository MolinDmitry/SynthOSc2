#pragma once
#include <iostream>
#include <string>
#include <cmath>
#include "Ray.hpp"

#define M_PI 3.14159265358979323846

//===============================================================================================================================
// класс оптической поверхности
class OptSurf{
    private:
        double R;
        double t;
        std::string glass;
    public:
        OptSurf();
        OptSurf(double R, double t, std::string glass);
        void printOptSurf();
        double getR();
        std::string getGlass();
        double getT();
        void setR(double R);
        void setGlass(std::string glass);
        void setT(double t);
        Ray PassRayThrougtSurface(Ray ray, double n0, double n1);
};

OptSurf::OptSurf(){
        this->R = 0;
        this->t = 0;
        this->glass = "";
    }

//==============================================================================================================

    OptSurf::OptSurf(double R, double t, std::string glass){
        this->R = R;
        this->t = t;
        this->glass = glass;
    }

    void OptSurf::printOptSurf(){
        std::string sign_r = "";
        if (this->R<0) sign_r = "-";
        std::string str_r = sign_r + "INF";
        if (fabs(this->R)<1e+12) str_r = std::to_string(this->R);
        std::string str_t = "INF";
        if (this->t<1e+12) str_t = std::to_string(this->t);
        std::cout << "R: " << str_r << ";\t\tt: " << str_t << ";\t\tglass: " << this->glass << std::endl;  
    
    }

    std::string OptSurf::getGlass()
    {
        return this->glass;
    }

    double OptSurf::getR()
    {
        return this->R;
    }

    double OptSurf::getT(){        
        return this->t;
    }

    void  OptSurf::setR(double R){
       this->R = R;
    }

    void  OptSurf::setGlass(std::string glass){
       this -> glass = glass;
    }

    void  OptSurf::setT(double t){
       this->t = t;
    }
    
    Ray OptSurf::PassRayThrougtSurface(Ray ray, double n0, double n1){
        Ray outRay = Ray();
        if (fabs(this->R) > 0.01){                                           // радиус не нулевой
            double D = this->R*this->R - ray.getH()*ray.getH();
            if (D < 0) return outRay;                                       //выходной луч не существует
            double xA = this->R - (this->R>=0 ? 1: -1)*sqrt(D);             //абсцисса точки пересечения луча и поверхности
            double denom = 1-xA/this->R;
            if (fabs(denom)<1e-14){
                outRay.setH(ray.getH());
                outRay.setW(ray.getW());
                outRay.setExist();
                return outRay;  // луч проходит по касательной, не преломляясь
            }
            double bn = ray.getH()/(denom);                                // параметр нормали к окружности в точке падения луча
            double kn = -1*bn/this->R;                                      // параметр нормали к окружности в точке падения луча
            double fi = ray.getW() - atan(kn);                             //угол между нормалью и падающим лучом
            if (fabs(fi) > M_PI/2) return outRay;                           // выходной луч не существует
            double sin_out_ray = n0*sin(fi)/n1;
            if (sin_out_ray > 1.0) return outRay;                           //полное внутреннее отражение
            double fi1 = asin(sin_out_ray);                                 //угол луча к нормали после преломления
            double w1 = atan(kn) + fi1;                                     //угол луча к ОО после преломления
            outRay.setH(ray.getH());
            outRay.setW(w1);
            outRay.setExist();
        }
        return outRay;
    }