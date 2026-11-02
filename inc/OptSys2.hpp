#pragma once
#include <iostream>
#include <cmath>
#include "Ray.hpp"
#include "GlassLib.hpp"
#include "GOST_GlassLib.hpp"
#include "OptSurf.hpp"

#define NUM_WAVELENGTHS 10
#define MAX_NUM_SURF 100

//===============================================================================================================================
// класс оптической системы
class OptSystem{
    private:
        double aperture; 
        double field;
        double wavelengthsArray[NUM_WAVELENGTHS];
        OptSurf surfaceArray[MAX_NUM_SURF];
        int surfaceCount;
        double refIndArray[NUM_WAVELENGTHS];
        int wavelengthsCount;
        GlassLib* glassLib;
    public:
        OptSystem();
        OptSystem(
            double aperture, 
            double field, 
            double wavelengths[NUM_WAVELENGTHS],
            int wavelengthsCount, 
            GlassLib* glassLib
        );
        double getAperture();
        double getField();
        void appendSurface(OptSurf* optSurf);
        void printOptSystem() const;
        Ray* passRayThroughtOS(Ray* ray, int wvlInd, bool thinComp);
        double findRMSSpotRadius(int wvlInd, double FOV, bool thinComp, int numRays); // FOV - угол отклоненияч луча в градусах 
                                                                                    // NumRays - число лучей по высоте входного зрачка
        OptSystem* cloneOptSystem();
};

    OptSystem::OptSystem(){
        this->aperture = 0;
        this->field = 0;
        for(int i = 0; i < NUM_WAVELENGTHS; i++){
             this->wavelengthsArray[i] = 0;
        }
        this->wavelengthsCount = 0;
        for(int i = 0; i < MAX_NUM_SURF; i++){
            surfaceArray[MAX_NUM_SURF] = OptSurf(0, 0, "");
        }
        this->surfaceCount = 0;
        this->glassLib = nullptr;
        for(int i = 0; i < NUM_WAVELENGTHS;i++){
            this->refIndArray[i] = 1.0;
        }

    }
//==============================================================================================================
    OptSystem::OptSystem(
            double aperture, 
            double field, 
            double wavelengths[NUM_WAVELENGTHS], 
            int wavelengthsCount, 
            GlassLib* glassLib) {
        this->aperture = aperture;
        this->field = field;
        for(int i =0; i< NUM_WAVELENGTHS; i++){
             this->wavelengthsArray[i] = wavelengths[i];
        }
        this->wavelengthsCount = wavelengthsCount;
        for(int i = 0; i < MAX_NUM_SURF; i++){
            surfaceArray[MAX_NUM_SURF] = OptSurf(0, 0, "");
        }
        this->surfaceCount = 0;
        this->glassLib = glassLib;
        for(int i = 0; i<NUM_WAVELENGTHS;i++){
            this->refIndArray[i] = 1.0;
        }
    }

    double OptSystem::getAperture(){
        return this->aperture;
    }

    double OptSystem::getField(){
        return this->field;
    }

    void OptSystem::appendSurface(OptSurf* optSurf){
        this->surfaceArray[this->surfaceCount].setR( optSurf->getR());
        this->surfaceArray[this->surfaceCount].setT( optSurf->getT());
        this->surfaceArray[this->surfaceCount].setGlass( optSurf->getGlass());

        // добавляем показатели преломления в вектор refIndVector
        std::vector<double> ref_vec;
        for(double wvl: *this->wavelengthsVector){
            double refInd = this->glassLib->getRefIndex(optSurf->getGlass(),wvl);    
            ref_vec.push_back(refInd);
        }
        this->refIndVector.push_back(ref_vec);
    }

    void OptSystem::printOptSystem() const{
        std::cout << "Aperture: " << this->aperture <<std::endl;
        std::cout << "Field: " << this->field <<std::endl;
        for(int i = 0; i<this->wavelengthsVector->size(); i++){
            std::cout << "wavelength " << i << ": " << this->wavelengthsVector->at(i) <<std::endl;
        }
        for(auto surf: this->sutfaceVector){
            surf.printOptSurf();
        }

        }
    
    std::shared_ptr<Ray> OptSystem::passRayThroughtOS(std::shared_ptr<Ray> ray, int wvlInd, bool thinComp){
        std::shared_ptr<Ray> outRay = nullptr;
        std::shared_ptr<Ray> inRay = std::make_shared<Ray>(ray->getH(),ray->getW());
        int sfVecSize = this->sutfaceVector.size();
        for(int i = 0; i<sfVecSize;i++){
            // находим луч после прохождения каждой поверхности
            outRay = this->sutfaceVector.at(i).PassRayThrougtSurface(
                inRay,
                this->refIndVector.at(i).at(wvlInd),
                this->refIndVector.at(i+1).at(wvlInd)
            );
            if (outRay != nullptr){
                double nextH = outRay->getH();
                if (!thinComp){
                    // находим высоту на следующей поверхности по упрощенной формуле
                    nextH = nextH+tan(outRay->getW())*this->sutfaceVector.at(i).getT();
                }                
                std::shared_ptr<Ray> newRay = std::make_shared<Ray>(nextH,outRay->getW());                
                inRay = newRay;
            }
            else return nullptr;            
        }       
        return outRay;
        }

        /**
         * @brief Вычисление среднеквадратического значения радиуса пятна рассеяния в фокальной плоскости
         * @param wvlInd - индекс длины волны в векторе длин волн
         * @param FOV - угол отклонения луча в градусах
         * @param thinComp - флаг расчета для оптической системы из тонких компонентов
         * @param numRays - число лучей для расчета по высоте входного зрачка
         * @return Среднеквадратическое значение радиуса пятна рассяения в фокальной плоскости
         * @details
         *      - Вычисляем положение фокальной плоскости
         *      - Для центра поля вычисляем ход луча с нулевым углом для апертуры от нуля до полного значания
         *      - Для края поля:
         *          - вычисляем нулевой луч с углом по краю поля зрения и находим координату точки пересечения с фокальной плоскостью
         *          - вычисляем лучи по краю поля с апертурой от -aperture/2 до +aperture/2
         *          - вычисляем RMS радиус пятна относительно точки пересечения наклонного нулевого луча с фокальной плоскостью
         * 
         */
        double OptSystem::findRMSSpotRadius(int wvlInd, double FOV, bool thinComp, int numRays)
        {
            if (numRays <= 1) return 0.0;
            if (numRays%2 != 0) numRays++; // для расчета по четному числу чтобы луч не проходил по центру зрачка
            // т.е. пятно от него будет 0, а в общем количестве он учитывается, уменьшая RMS при малых количествах лучей
            // вычисляем положение фокальной плоскости
            std::shared_ptr<Ray> ray = std::make_shared<Ray>(1e-7,0.0); // нулевой луч
            std::shared_ptr<Ray> outRay = this->passRayThroughtOS(ray,wvlInd,thinComp);
            if (outRay == nullptr) return 1e+15;
            if (fabs(outRay->getW())<1e-15) // фокальная плоскость в бесконечности
                return 1e+15;
            double xFplane = -1*outRay->getH()/tan(outRay->getW()); // координата фокальной плоскости относительно последней поверхности
            double curField = FOV*M_PI/180;
            ray = std::make_shared<Ray> (1e-7,curField);
            outRay = this->passRayThroughtOS(ray,wvlInd,thinComp);
            if (outRay == nullptr) return 1e+15;
            double lastR = this->sutfaceVector.at(this->sutfaceVector.size()-1).getR();
            double dX = fabs(lastR) - sqrt(lastR*lastR - outRay->getH()*outRay->getH());
            double xf = xFplane - dX*(lastR>=0 ? 1:-1); // координата фокальной плоскости в системе координат, связанной с точкой выхода луча после последней поверхности
            double y0 = outRay->getH() + xf*tan(outRay->getW()); //высота нулевого луча на пересечении с фокальной плоскостью            
            double da = this->aperture/(numRays-1);
            double RMSRadius = 0;
            int numPassedRays = 0;
            for(int i = 0;i<numRays;i++){
                double curH = i*da - this->aperture/2;
                ray = std::make_shared<Ray>(curH,curField);
                outRay = this->passRayThroughtOS(ray,wvlInd,thinComp);
                if (outRay != nullptr){
                    double lastR = this->sutfaceVector.at(this->sutfaceVector.size()-1).getR();
                    double dX = fabs(lastR) - sqrt(lastR*lastR - outRay->getH()*outRay->getH());
                    double xf = xFplane - dX*(lastR>=0 ? 1:-1);// координата фокальной плоскости в системе координат, связанной с точкой выхода луча после последней поверхности
                    double dy = outRay->getH() + xf*tan(outRay->getW()) - y0;
                    numPassedRays++;
                    RMSRadius+=dy*dy;
                }
            }
            RMSRadius = sqrt(RMSRadius/numPassedRays);
            return RMSRadius;
        }

    

//==============================================================================================================
 std::shared_ptr<OptSystem> OptSystem::cloneOptSystem(){
    return nullptr;
 }
