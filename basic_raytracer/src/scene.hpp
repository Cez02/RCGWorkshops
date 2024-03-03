#pragma once

#include <embree3/rtcore.h>
#include <vector>

#include "objects.hpp"
#include "camera.hpp"
#include "GLDrawer.hpp"

namespace CandlelightRTC {

    class Scene {
    private:
        RTCScene m_RTCScene;
        RTCDevice m_RTCDevice;
        GLDrawer *m_Drawer;
        Camera m_Camera;

        std::vector<PObjectPtr> m_ObjectsInScene;

        void AttachObject(PObjectPtr obj);

    public:
        void Setup(RTCDevice rtcDevice, GLDrawer *drawer, Camera camera);
        void DrawScene(int screenWidth, int screenHeight);

        Camera &getCamera();
    };
}