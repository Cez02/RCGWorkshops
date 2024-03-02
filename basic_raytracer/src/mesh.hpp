#pragma once

#include <embree4/rtcore.h>

#include <memory>

typedef float* floatPtr;
typedef u_int* uintPtr;

namespace CandlelightRTC {
    class Mesh {
    private:
        RTCGeometry m_RTCGeometry;
        float *m_Vertices;
        u_int *m_Indices;

    public:
        floatPtr &getVertices();
        uintPtr &getIndices();
        RTCGeometry &getRTCGeometry();

        static std::shared_ptr<Mesh> getSphereMesh(RTCDevice &device);
    };

    typedef std::shared_ptr<Mesh> MeshPtr;
}