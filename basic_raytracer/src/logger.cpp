#include "logger.hpp"

#include "common.hpp"
#include <chrono>
#include <iomanip>

namespace CandlelightRTC{

    float getSecondsSinceStart(){
        return (float)std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::high_resolution_clock::now() - START_TIME)).count() / 1000.0;
    }

    void LogInfo(std::string message){
        std::cout << "[LOG     | " << std::setw(7) << getSecondsSinceStart();
        std::cout << "] " << message << std::endl;
    }

    void LogError(std::string message){
        std::cout << "\033[1;31m" << "[ERROR   | " << std::setw(7) << getSecondsSinceStart();
        std::cout << "] " << message << "\033[0m" << std::endl;
    }

    void LogWarning(std::string message){
        std::cout << "\033[1;33m" << "[WARNING | " << std::setw(7) << getSecondsSinceStart();
        std::cout << "] " << message << "\033[0m" << std::endl;

    }

}