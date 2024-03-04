#include "scene.hpp"

#include "mesh.hpp"
#include "camera.hpp"
#include "logger.hpp"

#include <glm/gtx/string_cast.hpp>

namespace CandlelightRTC {

    int geoCount = 0;

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
        newObject->getTransform().Position = glm::vec3(-0.8f, 0.8f, -0.5f);

        PObjectPtr newObject2 = std::make_shared<PObject>();
        newObject2->getMesh() = Mesh::getSphereMesh(rtcDevice);
        newObject2->getTransform().Position = glm::vec3(0.8f, 0.8f, -1.5f);
        // newObject2->getTransform().Scale *= 0.5f;

        PObjectPtr ground = std::make_shared<PObject>();
        ground->getMesh() = Mesh::getPlaneMesh(rtcDevice);
        ground->getTransform().Position = glm::vec3(0, 0, 0);
        // ground->getTransform().Rotation = glm::quat(glm::vec3(glm::radians(180), 0,))
        ground->getTransform().Scale *= 6;

        CandlelightRTC::LogInfo("Attaching geometry and comitting...");

        AttachObject(newObject, 0);
        AttachObject(newObject2, 1);
        AttachObject(ground, 2);

        m_Materials[0] = material_t(materialtype_t::DIFFUSE, glm::vec3(1, 1, 1));
        m_Materials[1] = material_t(materialtype_t::DIFFUSE, glm::vec3(0.4, 1.0, 0.4));
        m_Materials[2] = material_t(materialtype_t::DIFFUSE, glm::vec3(0.7, 0.7, 0.7));

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
            // throw std::runtime_error("Incorrect scene setup. Test ray did not hit sphere.");
        }

    }
    
    void Scene::AttachObject(PObjectPtr obj, int id)
    {
        // apply transform to mesh and commit

        obj->getMesh()->ApplyTransform(obj->getTransform());
        rtcCommitGeometry(obj->getMesh()->getRTCGeometry());

        rtcAttachGeometryByID(m_RTCScene, obj->getMesh()->getRTCGeometry(), id);
        rtcReleaseGeometry(obj->getMesh()->getRTCGeometry());
    }


    // Ideally this should land in a totally seperate class/header file
    void Scene::DrawScene(int screenWidth, int screenHeight)
    {

    }

    std::map<int, material_t> &Scene::getMaterials() 
    {
        return m_Materials;
    }

    Camera &Scene::getCamera()
    {
        return m_Camera;
    }

    RTCScene &Scene::getRTCScene()
    {
        return m_RTCScene;
    }
}