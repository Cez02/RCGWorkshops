#include "mesh.hpp"

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include <cstring>

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

    RTCScene &Mesh::getRTCScene()
    {
        return m_RTCScene;
    }

    static MeshPtr SPHERE_MESH = nullptr;
    static MeshPtr PLANE_MESH = nullptr;

    std::shared_ptr<Mesh> Mesh::getSphereMesh(RTCDevice &device)
    {
        if(SPHERE_MESH != nullptr)
            return SPHERE_MESH;

        SPHERE_MESH = std::make_shared<Mesh>();

        float radius = 1;
        float sectorCount = 50;
        float stackCount = 60;

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
                    indices.push_back(k1 + 1);
                    indices.push_back(k1);
                    indices.push_back(k2);
                }

                // k1+1 => k2 => k2+1
                if(i != (stackCount-1))
                {
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                    indices.push_back(k1 + 1);
                }
            }
        }

        CandlelightRTC::LogInfo("Creating embree geometry for sphere...");

        RTCGeometry geo = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

        SPHERE_MESH->getIndices() = (uintPtr)rtcSetNewGeometryBuffer(geo,
            RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3*sizeof(u_int), indices.size() / 3);
        memcpy(SPHERE_MESH->getIndices(), indices.data(), indices.size() * sizeof(u_int));

        SPHERE_MESH->getVertices() = (floatPtr)rtcSetNewGeometryBuffer(geo,
            RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3*sizeof(float), verts.size() / 3);
        memcpy(SPHERE_MESH->getVertices(), verts.data(), verts.size() * sizeof(float));

        CandlelightRTC::LogInfo("Finishing...");

        SPHERE_MESH->m_VertexCount = verts.size();
        SPHERE_MESH->m_IndexCount = indices.size();

        SPHERE_MESH->m_RTCScene = rtcNewScene(device);
        rtcAttachGeometry(SPHERE_MESH->m_RTCScene, geo);
        rtcCommitGeometry(geo);
        rtcCommitScene(SPHERE_MESH->m_RTCScene);
        rtcReleaseGeometry(geo);

        return SPHERE_MESH;
    }

    std::shared_ptr<Mesh> Mesh::getPlaneMesh(RTCDevice &device)
    {
        if(PLANE_MESH != nullptr)
            return PLANE_MESH;

        PLANE_MESH = std::make_shared<Mesh>();


        CandlelightRTC::LogInfo("Creating plane mesh...");

        std::vector<float> verts{
            -1, 0, -1,
            -1, 0,  1,
             1, 0,  1,
             1, 0, -1,
        };

        std::vector<int> indices{
            0, 1, 2,
            1, 2, 3
        };


        CandlelightRTC::LogInfo("Creating embree geometry for plane...");

        RTCGeometry geo = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

        PLANE_MESH->getIndices() = (uintPtr)rtcSetNewGeometryBuffer(geo,
            RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3*sizeof(u_int), indices.size() / 3);
        memcpy(PLANE_MESH->getIndices(), indices.data(), indices.size() * sizeof(u_int));

        PLANE_MESH->getVertices() = (floatPtr)rtcSetNewGeometryBuffer(geo,
            RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3*sizeof(float), verts.size() / 3);
        memcpy(PLANE_MESH->getVertices(), verts.data(), verts.size() * sizeof(float));

        CandlelightRTC::LogInfo("Finishing...");

        PLANE_MESH->m_VertexCount = verts.size();
        PLANE_MESH->m_IndexCount = indices.size();

        PLANE_MESH->m_RTCScene = rtcNewScene(device);

        rtcAttachGeometry(PLANE_MESH->m_RTCScene, geo);

        rtcCommitGeometry(geo);
        rtcCommitScene(PLANE_MESH->m_RTCScene);

        rtcReleaseGeometry(geo);

        return PLANE_MESH;
    }

    std::shared_ptr<Mesh> Mesh::getMeshFromFile(RTCDevice &device, std::string modelName)
    {
        return std::shared_ptr<Mesh>();
    }

    void Mesh::ApplyTransform(transform_t transform){
        auto trn = transform.toMat4();

        for(int i = 0; i<m_VertexCount; i += 3){
            glm::vec3 newCoord = trn * glm::vec4(m_Vertices[i], m_Vertices[i + 1], m_Vertices[i + 2], 1.0f) ;

            m_Vertices[i] = newCoord.x;
            m_Vertices[i + 1] = newCoord.y;
            m_Vertices[i + 2] = newCoord.z;
        }
    }

}