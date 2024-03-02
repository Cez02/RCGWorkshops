#pragma once

#include "transform.hpp"

namespace CandlelightRTC {

    class Camera {
    private:
        transform_t m_Transform;
        float m_NearPlane;
        float m_MaxRayDistance;

    public:
        transform_t &getTransform();
        float &getNearPlane();
        float &getMaxRayDistance();

        Camera(transform_t transform, float nearPlane, float maxRayDistance);
    };
}