#pragma once
#include <iostream>
#include <cmath>
#include <string>
#include "Ray.hpp"
#include "GlassLib.hpp"
#include "GOST_GlassLib.hpp"
#include "OptSurf.hpp"

#define NUM_WAVELENGTHS 10
#define MAX_NUM_SURF 100
#define REF_IND_SIZE NUM_WAVELENGTHS*MAX_NUM_SURF
#define RAY_IS_EXIST 1
#define RAY_DONT_EXIST 0

//===============================================================================================================================
// класс оптической системы
class OptSystem{
    private:
        double aperture; 
        double field;
        double wavelengthsArray[NUM_WAVELENGTHS];
        OptSurf surfaceArray[MAX_NUM_SURF];
        int surfaceCount;
        double refIndArray[REF_IND_SIZE]; // массив показателей преломления, последовательно записаны показатели для каждой поверхности для каждой длины волны
        int wavelengthsCount;
        GlassLib* glassLib;
    public:
        OptSystem();
        OptSystem(
            double aperture, 
            double field, 
            double* wavelengths,
            int wavelengthsCount, 
            GlassLib* glassLib
        );
        double getAperture();
        double getField();
        void appendSurface(OptSurf* optSurf);
        void printOptSystem();
        Ray passRayThroughtOS(Ray ray, int wvlIndex, bool thinComp);
        double findSpotRadius(int wvlIndexex, double FOV_deg, bool RMS, bool thinComp, int numRays); // FOV - угол отклоненияч луча в градусах 
                                                                                    // NumRays - число лучей по высоте входного зрачка
        double findRearFocalLength(int wvlIndex, int thinComp);
        OptSystem cloneOptSystem();
};

    OptSystem::OptSystem(){
        this->aperture = 0;
        this->field = 0;
        for(int i = 0; i < NUM_WAVELENGTHS; i++){
             this->wavelengthsArray[i] = 0;
        }
        this->wavelengthsCount = 0;
        for(int i = 0; i < MAX_NUM_SURF; i++){
            surfaceArray[i] = OptSurf(0, 0, "");
        }
        this->surfaceCount = 0;
        this->glassLib = nullptr;
        for(int i = 0; i < REF_IND_SIZE;i++){
            this->refIndArray[i] = 1.0;
        }

    }
//==============================================================================================================
    OptSystem::OptSystem(
            double aperture, 
            double field, 
            double* wavelengths, 
            int wavelengthsCount, 
            GlassLib* glassLib) {
        this->aperture = aperture;
        this->field = field;
        this->wavelengthsCount = wavelengthsCount;
        for(int i =0; i< wavelengthsCount; i++){
             this->wavelengthsArray[i] = wavelengths[i];
        }        
        for(int i = 0; i < MAX_NUM_SURF; i++){
            surfaceArray[i] = OptSurf(0, 0, "NONE");
        }
        this->surfaceCount = 0;
        this->glassLib = glassLib;
        for(int i = 0; i < REF_IND_SIZE;i++){
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
        for(int i=0; i < this->wavelengthsCount; i++){
            double wvl = this->wavelengthsArray[i];
            this->refIndArray[this->surfaceCount*this->wavelengthsCount + i] = this->glassLib->getRefIndex(optSurf->getGlass(), wvl);
        }
        this->surfaceCount++;
    }

    void OptSystem::printOptSystem(){
        std::cout << std::endl;
        std::cout << "=======================================================================" <<std::endl;
        std::cout << "Optical system data: " <<std::endl;
        std::cout << "Aperture: " << this->aperture << " mm" <<std::endl;
        std::cout << "Field (2w): " << 2*this->field << " deg" << std::endl;
        for(int i = 0; i<this->wavelengthsCount; i++){
            std::cout << "wavelength " << i << ": " << this->wavelengthsArray[i] << " um" <<std::endl;
        }
        if (this->surfaceCount == 0){
            std::cout << "There are no optical surfaces " <<std::endl;
        }
        else{
            for(int i = 0; i<this->surfaceCount; i++){
                this->surfaceArray[i].printOptSurf();     
            }
        }
        
        std::cout << "=======================================================================" <<std::endl;
        }
    
    Ray OptSystem::passRayThroughtOS(Ray ray, int wvlIndex, bool thinComp){
        Ray outRay = Ray(); // создаем луч с признаком не существования
        Ray inRay = Ray(ray.getH(),ray.getW());
        for(int i = 0; i < this->surfaceCount; i++){
            // находим луч после прохождения каждой поверхности
            double n0 = 1;
            if (i > 0) n0 = this->refIndArray[(i-1)*this->wavelengthsCount + wvlIndex];
            outRay = this->surfaceArray[i].PassRayThrougtSurface(
                inRay,
                n0,
                this->refIndArray[i*this->wavelengthsCount + wvlIndex]
            );
            if (outRay.isExist()){
                double nextH = outRay.getH(); // для тонкого компонента высота на следующей поверхности не меняется 
                if (!thinComp){
                    // находим высоту на следующей поверхности по упрощенной формуле
                    nextH = nextH+tan(outRay.getW())*this->surfaceArray[i].getT();
                }                
                inRay = Ray(nextH, outRay.getW()); // луч для следующей поверхности                
            }
            else {
                return outRay; // возвращаем луч с признаком несущесмтвующего
            }            
        }       
        return outRay;
    }

        /**
         * @brief Вычисление значения радиуса пятна рассеяния в фокальной плоскости
         * @param wvlInd - индекс длины волны в векторе длин волн
         * @param FOV_deg - угол отклонения луча в градусах
         * @param RMS - true - сренеквадратический радиус, false - геометрический радиус
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
    double OptSystem::findSpotRadius(int wvlIndex, double FOV_deg, bool RMS, bool thinComp, int numRays){
        if (numRays <= 1) return 0.0;
        if (numRays%2 != 0) numRays++; // для расчета по четному числу чтобы луч не проходил по центру зрачка
        // т.е. пятно от него будет 0, а в общем количестве он учитывается, уменьшая RMS при малых количествах лучей
        // вычисляем положение фокальной плоскости
        Ray ray = Ray(1e-7, 0.0); // нулевой луч
        Ray outRay = this->passRayThroughtOS(ray, wvlIndex, thinComp);
        if (!outRay.isExist()) return 1e+15;
        if (fabs(outRay.getW())<1e-15) // фокальная плоскость в бесконечности
            return 1e+15;
        double xFplane = -1*outRay.getH()/tan(outRay.getW()); // координата фокальной плоскости относительно последней поверхности
        double curField = FOV_deg*M_PI/180;
        ray = Ray(1e-7, curField);
        outRay = this->passRayThroughtOS(ray, wvlIndex, thinComp);
        if (!outRay.isExist()) return 1e+15;
        double lastR = this->surfaceArray[this->surfaceCount-1].getR();
        double dX = fabs(lastR) - sqrt(lastR*lastR - outRay.getH()*outRay.getH());
        double xf = xFplane - dX*(lastR >= 0 ? 1:-1); // координата фокальной плоскости в системе координат, связанной с точкой выхода луча после последней поверхности
        double y0 = outRay.getH() + xf*tan(outRay.getW()); //высота нулевого луча на пересечении с фокальной плоскостью            
        double da = this->aperture/(numRays-1);
        double radius = 0;
        double curH = 0;
        if (RMS){
            int numPassedRays = 0;
            for(int i = 0; i < numRays;i++){
                curH = i*da - this->aperture/2;
                ray = Ray(curH, curField);
                outRay = this->passRayThroughtOS(ray, wvlIndex, thinComp);
                if (outRay.isExist()){
                    double lastR = this->surfaceArray[this->surfaceCount-1].getR();
                    double dX = fabs(lastR) - sqrt(lastR*lastR - outRay.getH()*outRay.getH());
                    double xf = xFplane - dX*(lastR>=0 ? 1:-1);// координата фокальной плоскости в системе координат, связанной с точкой выхода луча после последней поверхности
                    double dy = outRay.getH() + xf*tan(outRay.getW()) - y0;
                    numPassedRays++;
                    radius+=dy*dy;
                }
            }
            radius = sqrt(radius/numPassedRays);
        }
        else{
            curH = this->aperture/2;
            ray = Ray(curH, curField);
            outRay = this->passRayThroughtOS(ray, wvlIndex, thinComp);
            if (outRay.isExist()){
                    double lastR = this->surfaceArray[this->surfaceCount-1].getR();
                    double dX = fabs(lastR) - sqrt(lastR*lastR - outRay.getH()*outRay.getH());
                    double xf = xFplane - dX*(lastR>=0 ? 1:-1);// координата фокальной плоскости в системе координат, связанной с точкой выхода луча после последней поверхности
                    double dy = outRay.getH() + xf*tan(outRay.getW()) - y0;
                    radius =fabs(dy);
                }
        }        
        return radius;
    }

    /**
     * @brief возвращает значение заднего фокусного расстояния ОС
     * 
     */

    double OptSystem::findRearFocalLength(int wvlIndex, int thinComp){
        Ray inputRay = Ray(1e-7, 0);
        Ray outputRay = this->passRayThroughtOS(inputRay, wvlIndex, thinComp);
        return -1*inputRay.getH()/tan(outputRay.getW());
    }

    

//==============================================================================================================
    OptSystem OptSystem::cloneOptSystem(){
        OptSystem clonedOptSys = OptSystem(this->aperture,this->field, this->wavelengthsArray, this->wavelengthsCount, this->glassLib);
        for(int i = 0; i < this->surfaceCount; i++){
            clonedOptSys.appendSurface(&this->surfaceArray[i]);
        }
        return clonedOptSys;
    }
