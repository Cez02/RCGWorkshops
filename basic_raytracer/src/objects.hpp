#pragma once

#include <embree3/rtcore.h>

#include <memory>

#include "transform.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "material.hpp"

namespace CandlelightRTC {

    class PObject {
    private:
        ModelPtr m_Model;
        transform_t m_Transform;
        material_t m_Material;

        u_int m_InstanceID;

    public:
        transform_t &getTransform();
        ModelPtr &getModel();
        material_t &getMaterial();
        u_int &getInstanceID();
    };

    typedef std::shared_ptr<PObject> PObjectPtr;
}