#pragma once

#include "common.hpp"

#include <string>
#include <iostream>
#include <chrono>

namespace CandlelightRTC {

    void LogInfo(std::string message);
    void LogError(std::string message);
    void LogWarning(std::string message);

}