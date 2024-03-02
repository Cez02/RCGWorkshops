#pragma once

#include "transform.hpp"

namespace CandlelightRTC {

    class Camera {
    private:
        transform_t m_Transform;
        float m_NearPlaneDistance;
        float m_AspectRatio;
        float m_MaxRayDistance;

    public:
        transform_t &getTransform();
        float &getNearPlaneDistance();
        float &getAspectRatio();
        float &getMaxRayDistance();

        Camera(transform_t transform, float maxRayDistance);
        Camera();
    };
}