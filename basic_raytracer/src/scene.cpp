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
        newObject->getMesh() = Mesh::getSphereMesh(rtcDevice);

        newObject->getTransform().Position = glm::vec3(0, 0, 5);



        CandlelightRTC::LogInfo("Attaching geometry and comitting...");

        AttachObject(newObject);

        CandlelightRTC::LogInfo("   ... commiting scene ...");

        rtcCommitScene(m_RTCScene);

        CandlelightRTC::LogInfo("--- Test raycast ---");

        RTCRayHit rayhit; 
        rayhit.ray.org_x  = 0; rayhit.ray.org_y = 0; rayhit.ray.org_z = 0;
        rayhit.ray.dir_x  = 0; rayhit.ray.dir_y = 0; rayhit.ray.dir_z = 1;
        rayhit.ray.tnear  = 0.f;
        rayhit.ray.tfar   = std::numeric_limits<float>::infinity();
        rayhit.ray.mask = -1;
        rayhit.ray.flags = 0;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

        RTCIntersectContext context;
        rtcInitIntersectContext(&context);

        rtcIntersect1(m_RTCScene, &context, &rayhit);

        if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
            LogInfo("Hit sphere as expected at " + std::to_string(rayhit.hit.Ng_x) + " " + std::to_string(rayhit.hit.Ng_y) + " " + std::to_string(rayhit.hit.Ng_z));
        }
        else {
            LogError("Ray did not hit sphere.");
            throw std::runtime_error("Incorrect scene setup. Test ray did not hit sphere.");
        }

    }
    
    void Scene::AttachObject(PObjectPtr obj)
    {
        // apply transform to mesh and commit

        obj->getMesh()->ApplyTransform(obj->getTransform());
        rtcCommitGeometry(obj->getMesh()->getRTCGeometry());

        rtcAttachGeometry(m_RTCScene, obj->getMesh()->getRTCGeometry());
        rtcReleaseGeometry(obj->getMesh()->getRTCGeometry());
    }


    void Scene::DrawScene(int screenWidth, int screenHeight)
    {



        RTCRayHit rayhit; 

        rayhit.ray.org_x  = 0; rayhit.ray.org_y = 0; rayhit.ray.org_z = 0;
        rayhit.ray.dir_x  = 0; rayhit.ray.dir_y = 0; rayhit.ray.dir_z = 1;
        rayhit.ray.tnear  = 0.f;
        rayhit.ray.tfar   = std::numeric_limits<float>::infinity();
        rayhit.ray.mask = -1;
        rayhit.ray.flags = 0;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
        
        RTCIntersectContext context;


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

                pointPos = glm::vec4(pointPos.x, pointPos.y, pointPos.z, 1.0) * m_Camera.getTransform().toMat4();


                glm::vec3 rayDir = (pointPos - positionToRenderFrom);


                // RTCRayHit rayhit; 
                rayhit.ray.org_x  = positionToRenderFrom.x; rayhit.ray.org_y = positionToRenderFrom.y; rayhit.ray.org_z = positionToRenderFrom.z;
                rayhit.ray.dir_x  = rayDir.x; rayhit.ray.dir_y = rayDir.y; rayhit.ray.dir_z =  rayDir.z;
                rayhit.ray.tnear  = 0.f;
                rayhit.ray.tfar   = std::numeric_limits<float>::infinity();
                rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

                // RTCIntersectContext context;
                rtcInitIntersectContext(&context);

                rtcIntersect1(m_RTCScene, &context, &rayhit);

                if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
                    glm::vec3 n = glm::vec3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);
                    glm::vec3 r = rayDir - 2.0f*glm::dot(rayDir, n)*n;

                    r = (r + glm::vec3(1, 1, 1)) * 0.5f;
                    // LogInfo("Calculated r: " + glm::to_string(r));
                    m_Drawer->SetCanvasPixel(i, j, r);
                }
                else {
                    m_Drawer->SetCanvasPixel(i, j, glm::vec3(0.3, 0.3, 0.3));
                }

            }
        }

    }
    Camera &Scene::getCamera()
    {
        return m_Camera;
    }
}