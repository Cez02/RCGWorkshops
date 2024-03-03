#include "scene.hpp"

#include "mesh.hpp"
#include "camera.hpp"
#include "logger.hpp"

#include <glm/gtx/string_cast.hpp>

namespace CandlelightRTC {
    void Scene::Setup(RTCDevice rtcDevice, GLDrawer *drawer, Camera camera)
    {
        m_RTCScene = rtcNewScene(rtcDevice);
        m_RTCDevice = rtcDevice;
        m_Drawer = drawer;
        m_Camera = camera;

        rtcSetSceneFlags(m_RTCScene, RTC_SCENE_FLAG_ROBUST);

        CandlelightRTC::LogInfo("Creating geometry");

        PObjectPtr newObject = std::make_shared<PObject>();

        CandlelightRTC::LogInfo("   ... creating sphere mesh ...");

        newObject->getMesh() = Mesh::getSphereMesh(rtcDevice);

        CandlelightRTC::LogInfo("Attaching geometry and comitting...");
        CandlelightRTC::LogInfo("   ... attaching ...");

        for(int i = 0; i<9; i += 3){
            LogInfo("Vertex " + std::to_string(i) + ": " + std::to_string(newObject->getMesh()->getVertices()[i]) + " " + std::to_string(newObject->getMesh()->getVertices()[i + 1]) + " " + std::to_string(newObject->getMesh()->getVertices()[i + 2]));
        }
        rtcAttachGeometry(m_RTCScene, newObject->getMesh()->getRTCGeometry());
        CandlelightRTC::LogInfo("   ... releasing geometry ...");

        rtcReleaseGeometry(newObject->getMesh()->getRTCGeometry());
        CandlelightRTC::LogInfo("   ... commiting scene ...");

        rtcCommitScene(m_RTCScene);

        CandlelightRTC::LogInfo("--- Test raycast ---");

        RTCRayHit rayhit; 
        rayhit.ray.org_x  = 0; rayhit.ray.org_y = 0; rayhit.ray.org_z = 0;
        rayhit.ray.dir_x  = 0; rayhit.ray.dir_y = 0; rayhit.ray.dir_z = 1;
        rayhit.ray.tnear  = 0.f;
        rayhit.ray.tfar   = std::numeric_limits<float>::infinity();
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        
        // RTCIntersectArguments args;
        // rtcInitIntersectArguments(&args);

        rtcIntersect1(m_RTCScene, &rayhit);

        if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
            LogInfo("Hit sphere as expected at " + std::to_string(rayhit.hit.Ng_x) + " " + std::to_string(rayhit.hit.Ng_y) + " " + std::to_string(rayhit.hit.Ng_z));
        }
        else {
            LogError("Ray did not hit sphere.");
            throw std::runtime_error("Incorrect scene setup. Test ray did not hit sphere.");
        }

    }

    void Scene::DrawScene(int screenWidth, int screenHeight)
    {
        glm::vec3 positionToRenderFrom = m_Camera.getTransform().Position;

        for(int i = 0; i<screenWidth; i++){
            for(int j = 0; j<screenHeight; j++){

                // Cast ray from pos to pixel at near plane
                
                // near plane is always width 1

                // calculate point position on the near plane
                glm::vec3 pointPos = glm::vec3(
                    -0.5f + (float)i  / (float)(screenWidth - 1),
                     -0.5f / m_Camera.getAspectRatio() + (float)j  / (float)(screenHeight - 1),
                    m_Camera.getNearPlaneDistance()
                );

                // LogInfo("Starting point: " + glm::to_string(positionToRenderFrom) + ". Direction: " + glm::to_string(pointPos));

                // cast the ray and stuff

                glm::vec3 rayDir = glm::normalize(pointPos - positionToRenderFrom);

                RTCRayHit rayhit; 
                rayhit.ray.org_x  = positionToRenderFrom.x; rayhit.ray.org_y = positionToRenderFrom.y; rayhit.ray.org_z = positionToRenderFrom.z;
                rayhit.ray.dir_x  = rayDir.x; rayhit.ray.dir_y = rayDir.y; rayhit.ray.dir_z =  rayDir.z;
                rayhit.ray.tnear  = 0.f;
                rayhit.ray.tfar   = std::numeric_limits<float>::infinity();
                rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
                
                rtcIntersect1(m_RTCScene, &rayhit);

                if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
                    m_Drawer->SetCanvasPixel(i, j, glm::vec3(1, 0, 0));
                }
                else {
                    m_Drawer->SetCanvasPixel(i, j, glm::vec3(0.3, 0.3, 0.3));
                }

            }
        }

    }
}