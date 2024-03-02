#include "scene.hpp"

#include "mesh.hpp"
#include "camera.hpp"

namespace CandlelightRTC {
    void Scene::Setup(RTCScene rtcScene, RTCDevice rtcDevice, GLDrawer *drawer, Camera camera)
    {
        m_RTCScene = rtcScene;
        m_RTCDevice = rtcDevice;
        m_Drawer = drawer;
        m_Camera = camera;

        PObjectPtr newObject = std::make_shared<PObject>();
        newObject->getMesh() = Mesh::getSphereMesh(rtcDevice);

        rtcAttachGeometry(rtcScene, newObject->getMesh()->getRTCGeometry());
        rtcReleaseGeometry(newObject->getMesh()->getRTCGeometry());
        rtcCommitScene(rtcScene);
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
                     -0.5f / m_Camera.getAspectRatio() + (float)i  / (float)(screenHeight - 1),
                    m_Camera.getNearPlaneDistance()
                );

                // cast the ray and stuff

            }
        }

    }
}