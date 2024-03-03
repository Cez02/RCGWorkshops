#pragma once

#include <embree3/rtcore.h>
#include <vector>

#include <memory>

#include "objects.hpp"
#include "camera.hpp"
#include "GLDrawer.hpp"

#include "material.hpp"

namespace CandlelightRTC {

    class Scene {
    private:
        RTCScene m_RTCScene;
        RTCDevice m_RTCDevice;
        GLDrawer *m_Drawer;
        Camera m_Camera;

        std::vector<PObjectPtr> m_ObjectsInScene;

        void AttachObject(PObjectPtr obj, int id);

        std::map<int, material_t> m_Materials;

    public:
        void Setup(RTCDevice rtcDevice, GLDrawer *drawer, Camera camera);
        void DrawScene(int screenWidth, int screenHeight);

        Camera &getCamera();
        RTCScene &getRTCScene();
        std::map<int, material_t> &getMaterials();
    };

    typedef std::shared_ptr<Scene> ScenePtr;
}