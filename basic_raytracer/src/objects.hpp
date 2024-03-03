#pragma once

#include <embree3/rtcore.h>
#include <memory>

#include "transform.hpp"
#include "mesh.hpp"

namespace CandlelightRTC {

    class PObject {
    private:
        MeshPtr m_Mesh;
        transform_t m_Transform;

    public:
        transform_t &getTransform();
        MeshPtr &getMesh();
    };

    typedef std::shared_ptr<PObject> PObjectPtr;
}