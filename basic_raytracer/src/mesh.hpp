#pragma once

#include <embree3/rtcore.h>

#include "transform.hpp"

#include <memory>

typedef float* floatPtr;
typedef u_int* uintPtr;

namespace CandlelightRTC {
    class Mesh {
    private:
        RTCGeometry m_RTCGeometry;
        int m_VertexCount, m_IndexCount;
        float *m_Vertices;
        u_int *m_Indices;

    public:
        floatPtr &getVertices();
        uintPtr &getIndices();
        RTCGeometry &getRTCGeometry();

        void ApplyTransform(transform_t transform);

        static std::shared_ptr<Mesh> getSphereMesh(RTCDevice &device);
    };

    typedef std::shared_ptr<Mesh> MeshPtr;
}