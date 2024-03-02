#pragma once

#include <array>
#include <glad/glad.h>

#include <chrono>

using colorrgba_t = std::array<GLubyte, 4>;

static std::chrono::time_point START_TIME = std::chrono::high_resolution_clock::now();
