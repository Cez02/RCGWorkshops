#include "mesh.hpp"

#include <vector>

#include <glm/gtc/constants.hpp>

namespace CandlelightRTC {
    floatPtr &Mesh::getVertices()
    {
        return m_Vertices;
    }

    uintPtr &Mesh::getIndices()
    {
        return m_Indices;
    }

    RTCGeometry &Mesh::getRTCGeometry()
    {
        return m_RTCGeometry;
    }

    static MeshPtr SPHERE_MESH = nullptr;

    std::shared_ptr<Mesh> Mesh::getSphereMesh(RTCDevice &device)
    {
        SPHERE_MESH = std::make_shared<Mesh>();

        int parallels = 50;
        int meridians = 50;

        float radius = 1;
        float sectorCount = 20;
        float stackCount = 30;

        float x, y, z, xy;                              // vertex position
        float nx, ny, nz, lengthInv = 1.0f / 1.0f;    // vertex normal
        float s, t;                                     // vertex texCoord

        float sectorStep = 2 * glm::pi<float>() / sectorCount;
        float stackStep = glm::pi<float>() / stackCount;
        float sectorAngle, stackAngle;

        std::vector<float> verts;
        std::vector<int> indices;

        for(int i = 0; i <= stackCount; ++i)
        {
            stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
            xy = radius * cosf(stackAngle);             // r * cos(u)
            z = radius * sinf(stackAngle);              // r * sin(u)

            // add (sectorCount+1) vertices per stack
            // first and last vertices have same position and normal, but different tex coords
            for(int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                verts.push_back(x);
                verts.push_back(y);
                verts.push_back(z);
            }
        }

        int k1, k2;
        for(int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);     // beginning of current stack
            k2 = k1 + sectorCount + 1;      // beginning of next stack

            for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                // k1 => k2 => k1+1
                if(i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                // k1+1 => k2 => k2+1
                if(i != (stackCount-1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        SPHERE_MESH->getRTCGeometry() = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

        SPHERE_MESH->getIndices() = (uintPtr)rtcSetNewGeometryBuffer(SPHERE_MESH->getRTCGeometry(),
            RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3*sizeof(u_int), indices.size() / 3);
        memcpy(SPHERE_MESH->getIndices(), indices.data(), indices.size() * sizeof(u_int));

        SPHERE_MESH->getVertices() = (floatPtr)rtcSetNewGeometryBuffer(SPHERE_MESH->getRTCGeometry(),
            RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3*sizeof(float), verts.size() / 3);
        memcpy(SPHERE_MESH->getVertices(), verts.data(), verts.size() * sizeof(float));

        rtcCommitGeometry(SPHERE_MESH->getRTCGeometry());
    }
}