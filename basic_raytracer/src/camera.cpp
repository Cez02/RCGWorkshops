#include "camera.hpp"

namespace CandlelightRTC {
    transform_t &Camera::getTransform()
    {
        return m_Transform;
    }

    float &Camera::getNearPlane()
    {
        return m_NearPlane;
    }

    float &Camera::getMaxRayDistance()
    {
        return m_MaxRayDistance;
    }

    Camera::Camera(transform_t transform, float nearPlane, float maxRayDistance)
        : m_Transform(transform)
        , m_NearPlane(nearPlane)
        , m_MaxRayDistance(maxRayDistance) { }
}