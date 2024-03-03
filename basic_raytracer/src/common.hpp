#pragma once

#include <array>
#include <glad/glad.h>

#include <chrono>

#include <glm/glm.hpp>

using colorrgba_t = std::array<GLubyte, 4>;

using colorrgba_v_t = glm::vec3;

static std::chrono::time_point START_TIME = std::chrono::high_resolution_clock::now();
