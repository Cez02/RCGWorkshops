#include "objects.hpp"

namespace CandlelightRTC {
    transform_t &PObject::getTransform()
    {
        return m_Transform;
    }

    ModelPtr &PObject::getModel()
    {
        return m_Model;
    }

    material_t &PObject::getMaterial()
    {
        return m_Material;
    }
    
    u_int &PObject::getInstanceID()
    {
        return m_InstanceID;
    }
}