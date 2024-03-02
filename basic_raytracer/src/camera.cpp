#include "camera.hpp"

namespace CandlelightRTC {
    transform_t &Camera::getTransform()
    {
        return m_Transform;
    }

    float &Camera::getNearPlaneDistance()
    {
        return m_NearPlaneDistance;
    }

    float &Camera::getAspectRatio()
    {
        return m_AspectRatio;
    }

    float &Camera::getMaxRayDistance()
    {
        return m_MaxRayDistance;
    }

    Camera::Camera(transform_t transform, float maxRayDistance)
        : m_Transform(transform)
        , m_MaxRayDistance(maxRayDistance) { }

    Camera::Camera() {}
}