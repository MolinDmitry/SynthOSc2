#pragma once
#include <iostream>




//===============================================================================================================================
// абстрактный класс библиотеки стекол
class GlassLib{
    public:
        virtual double getRefIndex(std::string glassName, double wavelength) const = 0;
};


