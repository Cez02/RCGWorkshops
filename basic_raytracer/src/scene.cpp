#include "scene.hpp"

#include "mesh.hpp"
#include "camera.hpp"
#include "logger.hpp"
#include "model.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace CandlelightRTC {

    int geoCount = 0;

    void Scene::Setup(RTCDevice rtcDevice, GLDrawer *drawer, Camera camera)
    {
        m_RTCScene = rtcNewScene(rtcDevice);
        m_RTCDevice = rtcDevice;
        m_Drawer = drawer;
        m_Camera = camera;

        rtcSetSceneFlags(m_RTCScene, RTC_SCENE_FLAG_ROBUST);

        CandlelightRTC::LogInfo("Creating objects");


        PObjectPtr newObject = std::make_shared<PObject>();
        newObject->getModel() = Model::GetSphereModel(rtcDevice);
        newObject->getTransform().Position = glm::vec3(-0.8f, 0.8f, -0.5f);
        newObject->getMaterial() = material_t(materialtype_t::DIFFUSE, glm::vec3(1, 1, 1));
        newObject->getInstanceID() = 1;

        // PObjectPtr newObject2 = std::make_shared<PObject>();
        // newObject2->getModel() = Model::GetSphereModel(rtcDevice);
        // newObject2->getTransform().Position = glm::vec3(0.8f, 0.8f, -1.5f);
        // newObject2->getMaterial() = material_t(materialtype_t::DIFFUSE, glm::vec3(0.4, 1.0, 0.4));
        // newObject2->getInstanceID() = 2;

        // PObjectPtr ground = std::make_shared<PObject>();
        // ground->getModel() = Model::GetPlaneModel(rtcDevice);
        // ground->getTransform().Position = glm::vec3(0, 0, 0);
        // ground->getTransform().Scale *= 6;
        // ground->getMaterial() = material_t(materialtype_t::DIFFUSE, glm::vec3(0.7, 0.7, 0.7));
        // ground->getInstanceID() = 3;


        CandlelightRTC::LogInfo("Attaching geometry and comitting...");


        AttachObject(newObject);
        // AttachObject(newObject2);
        // AttachObject(ground);




        // created instanced geom

        for(int i = 0; i<m_ObjectsInScene.size(); i++){
            // attach instance
            auto transMat = m_ObjectsInScene[i]->getTransform().toMat4();

            for(int j = 0; j<m_ObjectsInScene[i]->getModel()->getMeshes().size(); j++){
                RTCGeometry geom = rtcNewGeometry(rtcDevice, RTC_GEOMETRY_TYPE_INSTANCE);
                rtcSetGeometryInstancedScene(geom, m_ObjectsInScene[i]->getModel()->getMeshes()[j]->getRTCScene());
                rtcSetGeometryTimeStepCount(geom, 1);
                rtcSetGeometryTransform(geom,
                                        0,
                                        RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR,
                                        glm::value_ptr(transMat));
                rtcCommitGeometry(geom);

                CandlelightRTC::LogInfo("Attaching instance: " + std::to_string(m_ObjectsInScene[i]->getInstanceID()));

                rtcAttachGeometryByID(m_RTCScene, geom, m_ObjectsInScene[i]->getInstanceID());
                rtcReleaseGeometry(geom);

                m_ObjectsInSceneMap[m_ObjectsInScene[i]->getInstanceID()] = m_ObjectsInScene[i];
            }
        }


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

        CandlelightRTC::LogInfo("Creating context");
        RTCIntersectContext context;
        rtcInitIntersectContext(&context);

        // LogInfo("Rayhit instance: " + std::to_string(context.instStackSize));

        CandlelightRTC::LogInfo("Shooting ray");

        rtcIntersect1(m_RTCScene, &context, &rayhit);

        if(rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
            LogInfo("Hit sphere as expected at " + std::to_string(rayhit.hit.Ng_x) + " " + std::to_string(rayhit.hit.Ng_y) + " " + std::to_string(rayhit.hit.Ng_z));
        }
        else {
            LogError("Ray did not hit sphere.");
            // throw std::runtime_error("Incorrect scene setup. Test ray did not hit sphere.");
        }

        // release meshes 
        rtcReleaseScene(Mesh::getPlaneMesh(rtcDevice)->getRTCScene());
        rtcReleaseScene(Mesh::getSphereMesh(rtcDevice)->getRTCScene());

    }
    
    void Scene::AttachObject(PObjectPtr obj)
    {
        // apply transform to mesh and commit
        m_ObjectsInScene.push_back(obj);
    }

    material_t &Scene::getMaterialForObject(int instID)
    {
        if(m_ObjectsInSceneMap.count(instID) <= 0){
            LogError("Could not find material for object " + std::to_string(instID));
            throw std::runtime_error("Could not find material");
        }
        return m_ObjectsInSceneMap[instID]->getMaterial();
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