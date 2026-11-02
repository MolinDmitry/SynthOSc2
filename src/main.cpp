#include <iostream>
#include <string>
#include <fstream>



void log(std::string str){
    std::cout << str << std::endl;
}

int main(int argc, char* argv[]){
    std::cout << "Synthesis optical systems" << std::endl;
    std::string line;

    std::ifstream inputStream("settings.conf");
    if (inputStream.is_open()){
        log("File is open");
        while(std::getline(inputStream, line)){
            std::cout << line << std::endl;
        }
    }
    else{
        log("File is not open");
    }
    inputStream.close();




    return 0;
}