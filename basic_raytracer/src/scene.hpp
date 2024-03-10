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

        std::map<int, PObjectPtr> m_ObjectsInSceneMap;

        void AttachObject(PObjectPtr obj);

    public:
        void Setup(RTCDevice rtcDevice, GLDrawer *drawer, Camera camera);

        Camera &getCamera();
        RTCScene &getRTCScene();
        material_t &getMaterialForObject(int instID);
    };

    typedef Scene* ScenePtr;
}