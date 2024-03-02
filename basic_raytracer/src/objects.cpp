#include "objects.hpp"

namespace CandlelightRTC {
    transform_t &PObject::getTransform()
    {
        return m_Transform;
    }

    MeshPtr &PObject::getMesh()
    {
        return m_Mesh;
    }
}