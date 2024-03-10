#pragma once

#include <embree3/rtcore.h>
#include <memory>

#include "transform.hpp"
#include "mesh.hpp"
#include "material.hpp"

namespace CandlelightRTC {

    class PObject {
    private:
        MeshPtr m_Mesh;
        transform_t m_Transform;
        material_t m_Material;

        u_int m_InstanceID;

    public:
        transform_t &getTransform();
        MeshPtr &getMesh();
        material_t &getMaterial();
        u_int &getInstanceID();
    };

    typedef std::shared_ptr<PObject> PObjectPtr;
}