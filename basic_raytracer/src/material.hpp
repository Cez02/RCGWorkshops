#pragma once

#include "common.hpp"

namespace CandlelightRTC {

    enum class materialtype_t {
        DIFFUSE
    };

    class material_t {
    private:
    public:
        materialtype_t Type;
        colorrgba_v_t Color;

        material_t(materialtype_t type = materialtype_t::DIFFUSE, colorrgba_v_t color = glm::vec3(0.5f, 0.5f, 0.5f)){
            Type = type;
            Color = color;
        }
    };

}