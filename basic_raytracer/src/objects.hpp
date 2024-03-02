#pragma once

#include <embree4/rtcore.h>

#include "transform.hpp"

namespace CandlelightRTC {

    /// @brief A physical object with geometry and a material
    class PObject {
    private:
        transform_t m_Transform;

    public:
        transform_t &getTransform();
    };


}