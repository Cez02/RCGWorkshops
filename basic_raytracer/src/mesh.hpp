#pragma once

#include <embree3/rtcore.h>

#include "transform.hpp"

#include <memory>

typedef float* floatPtr;
typedef u_int* uintPtr;

namespace CandlelightRTC {
    class Mesh {
    private:
        RTCScene m_RTCScene;
        int m_VertexCount, m_IndexCount;
        float *m_Vertices;
        u_int *m_Indices;

    public:
        floatPtr &getVertices();
        uintPtr &getIndices();
        RTCScene &getRTCScene();

        int &getVertexCount();
        int &getIndexCount();

        void ApplyTransform(transform_t transform);

        static std::shared_ptr<Mesh> getSphereMesh(RTCDevice &device);
        static std::shared_ptr<Mesh> getPlaneMesh(RTCDevice &device);
        static std::shared_ptr<Mesh> getMeshFromFile(RTCDevice &device, std::string modelName);
    };

    typedef std::shared_ptr<Mesh> MeshPtr;
}