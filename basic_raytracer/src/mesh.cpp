#include "mesh.hpp"

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include "logger.hpp"

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
        if(SPHERE_MESH != nullptr)
            return SPHERE_MESH;

        SPHERE_MESH = std::make_shared<Mesh>();


        SPHERE_MESH->getRTCGeometry() = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

        SPHERE_MESH->getIndices() = (uintPtr)rtcSetNewGeometryBuffer(SPHERE_MESH->getRTCGeometry(),
            RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3*sizeof(u_int), 1);

        SPHERE_MESH->getIndices()[0] = 0;
        SPHERE_MESH->getIndices()[1] = 1;
        SPHERE_MESH->getIndices()[2] = 2;

        SPHERE_MESH->getVertices() = (floatPtr)rtcSetNewGeometryBuffer(SPHERE_MESH->getRTCGeometry(),
            RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3*sizeof(float), 3);
        SPHERE_MESH->getVertices()[0] = -1;
        SPHERE_MESH->getVertices()[1] = -1;
        SPHERE_MESH->getVertices()[2] = 5;

        SPHERE_MESH->getVertices()[3] = 0;
        SPHERE_MESH->getVertices()[4] = 1;
        SPHERE_MESH->getVertices()[5] = 5;

        SPHERE_MESH->getVertices()[6] = 1;
        SPHERE_MESH->getVertices()[7] = -1;
        SPHERE_MESH->getVertices()[8] = 5;

        rtcCommitGeometry(SPHERE_MESH->getRTCGeometry());

        return SPHERE_MESH;


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

        for(int i = 2; i<verts.size(); i += 3)
            verts[i] += 5;

        for(int i = 0; i<verts.size(); i += 3){
            LogInfo("Vertex " + std::to_string(i) + ": " + std::to_string(verts[i]) + " " + std::to_string(verts[i + 1]) + " " + std::to_string(verts[i + 2]));
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

        CandlelightRTC::LogInfo("Creating embree geometry...");

        SPHERE_MESH->getRTCGeometry() = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

        SPHERE_MESH->getIndices() = (uintPtr)rtcSetNewGeometryBuffer(SPHERE_MESH->getRTCGeometry(),
            RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3*sizeof(u_int), indices.size() / 3);
        memcpy(SPHERE_MESH->getIndices(), indices.data(), indices.size() * sizeof(u_int));

        SPHERE_MESH->getVertices() = (floatPtr)rtcSetNewGeometryBuffer(SPHERE_MESH->getRTCGeometry(),
            RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3*sizeof(float), verts.size() / 3);
        memcpy(SPHERE_MESH->getVertices(), verts.data(), verts.size() * sizeof(float));

        for(int i = 0; i<verts.size(); i += 3){
            LogInfo("Vertex " + std::to_string(i) + ": " + std::to_string(SPHERE_MESH->getVertices()[i]) + " " + std::to_string(SPHERE_MESH->getVertices()[i + 1]) + " " + std::to_string(SPHERE_MESH->getVertices()[i + 2]));
        }

        rtcCommitGeometry(SPHERE_MESH->getRTCGeometry());

        return SPHERE_MESH;

    }
}